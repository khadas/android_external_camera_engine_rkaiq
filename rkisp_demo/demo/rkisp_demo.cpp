/*
 * V4L2 video capture example
 * AUTHOT : Jacob Chen
 * DATA : 2018-02-25
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h> /* getopt_long() */
#include <fcntl.h> /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <dlfcn.h>
#include <signal.h>
#include <linux/videodev2.h>

#include "drmDsp.h"
#include "rk_aiq_user_api_sysctl.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))
#define FMT_NUM_PLANES 1

#define BUFFER_COUNT 4

struct buffer {
        void *start;
        size_t length;
};

static char out_file[255];
static char dev_name[255];
static int width = 640;
static int height = 480;
static int format = V4L2_PIX_FMT_NV12;
static int fd = -1;
static enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
struct buffer *buffers;
static unsigned int n_buffers;
static int frame_count = -1;
FILE *fp;
static rk_aiq_sys_ctx_t* aiq_ctx = NULL;
static int silent = 0;
static int vop = 0;
static int rkaiq = 0;
static int writeFile = 0;


#define DBG(...) do { if(!silent) printf(__VA_ARGS__); } while(0)
#define ERR(...) do { fprintf(stderr, __VA_ARGS__); } while (0)

static void errno_exit(const char *s)
{
        ERR("%s error %d, %s\n", s, errno, strerror(errno));
        exit(EXIT_FAILURE);
}

static int xioctl(int fh, int request, void *arg)
{
        int r;
        do {
                r = ioctl(fh, request, arg);
        } while (-1 == r && EINTR == errno);
        return r;
}

static void process_image(const void *p, int size)
{
	fwrite(p, size, 1, fp);
	fflush(fp);
}

static int read_frame()
{
        struct v4l2_buffer buf;
        int i, bytesused;

        CLEAR(buf);

        buf.type = buf_type;
        buf.memory = V4L2_MEMORY_MMAP;

        struct v4l2_plane planes[FMT_NUM_PLANES];
        if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
            buf.m.planes = planes;
            buf.length = FMT_NUM_PLANES;
        }

        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
                errno_exit("VIDIOC_DQBUF");

        i = buf.index;

        if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type)
            bytesused = buf.m.planes[0].bytesused;
        else
            bytesused = buf.bytesused;

	if (vop) {
	    drmDspFrame(width, height, buffers[i].start, DRM_FORMAT_NV12);
	}

	if (writeFile && buf.sequence > 50 && buf.sequence <= 52) {
	    printf("---->>>>Write frame[%d] to file!\n", buf.sequence);
	    process_image(buffers[i].start, bytesused);
	}

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
            errno_exit("VIDIOC_QBUF");

        return 1;
}

static void mainloop(void)
{
    bool loop_inf = frame_count == -1 ? true : false;

    while (loop_inf || (frame_count-- > 0)) {
        read_frame();
    }
}

static void stop_capturing(void)
{
        enum v4l2_buf_type type;

        type = buf_type;
        if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
            errno_exit("VIDIOC_STREAMOFF");
}

static void start_capturing(void)
{
        unsigned int i;
        enum v4l2_buf_type type;

        for (i = 0; i < n_buffers; ++i) {
                struct v4l2_buffer buf;

                CLEAR(buf);
                buf.type = buf_type;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;

                if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
                    struct v4l2_plane planes[FMT_NUM_PLANES];

                    buf.m.planes = planes;
                    buf.length = FMT_NUM_PLANES;
                }
                if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                        errno_exit("VIDIOC_QBUF");
        }
        type = buf_type;
        if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                errno_exit("VIDIOC_STREAMON");
}

static void uninit_device(void)
{
        unsigned int i;

        for (i = 0; i < n_buffers; ++i)
                if (-1 == munmap(buffers[i].start, buffers[i].length))
                        errno_exit("munmap");

        free(buffers);
}

static void init_mmap(void)
{
        struct v4l2_requestbuffers req;

        CLEAR(req);

        req.count = BUFFER_COUNT;
        req.type = buf_type;
        req.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        ERR("%s does not support "
                                 "memory mapping\n", dev_name);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        if (req.count < 2) {
                ERR("Insufficient buffer memory on %s\n",
                         dev_name);
                exit(EXIT_FAILURE);
        }

        buffers = (struct buffer*)calloc(req.count, sizeof(*buffers));

        if (!buffers) {
                ERR("Out of memory\n");
                exit(EXIT_FAILURE);
        }

        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                struct v4l2_buffer buf;
                struct v4l2_plane planes[FMT_NUM_PLANES];
                CLEAR(buf);
                CLEAR(planes);

                buf.type = buf_type;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = n_buffers;

                if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
                    buf.m.planes = planes;
                    buf.length = FMT_NUM_PLANES;
                }

                if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
                        errno_exit("VIDIOC_QUERYBUF");

                if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
                    buffers[n_buffers].length = buf.m.planes[0].length;
                    buffers[n_buffers].start =
                        mmap(NULL /* start anywhere */,
                              buf.m.planes[0].length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd, buf.m.planes[0].m.mem_offset);
                } else {
                    buffers[n_buffers].length = buf.length;
                    buffers[n_buffers].start =
                        mmap(NULL /* start anywhere */,
                              buf.length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd, buf.m.offset);
                }

                if (MAP_FAILED == buffers[n_buffers].start)
                        errno_exit("mmap");
        }
}

static void init_device(void)
{
        struct v4l2_capability cap;
        struct v4l2_format fmt;

        if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
                if (EINVAL == errno) {
                        ERR("%s is no V4L2 device\n",
                                 dev_name);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_QUERYCAP");
                }
        }

        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
                !(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)) {
            ERR("%s is not a video capture device, capabilities: %x\n",
                         dev_name, cap.capabilities);
                exit(EXIT_FAILURE);
        }

        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                ERR("%s does not support streaming i/o\n",
                    dev_name);
                exit(EXIT_FAILURE);
        }

        if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
            buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        else if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)
            buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

        CLEAR(fmt);
        fmt.type = buf_type;
        fmt.fmt.pix.width = width;
        fmt.fmt.pix.height = height;
        fmt.fmt.pix.pixelformat = format;
        fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
                errno_exit("VIDIOC_S_FMT");

	init_mmap();
}

static void close_device(void)
{
        if (-1 == close(fd))
                errno_exit("close");

        fd = -1;
}

static void open_device(void)
{
        fd = open(dev_name, O_RDWR /* required */ /*| O_NONBLOCK*/, 0);

        if (-1 == fd) {
                ERR("Cannot open '%s': %d, %s\n",
                         dev_name, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }
}

void parse_args(int argc, char **argv)
{
   int c;
   int digit_optind = 0;

   while (1) {
       int this_option_optind = optind ? optind : 1;
       int option_index = 0;
       static struct option long_options[] = {
           {"width",    required_argument, 0, 'w' },
           {"height",   required_argument, 0, 'h' },
           {"format",   required_argument, 0, 'f' },
           {"device",   required_argument, 0, 'd' },
           //{"output",   required_argument, 0, 'o' },
           {"count",    required_argument, 0, 'c' },
           {"help",     no_argument,       0, 'p' },
           {"silent",   no_argument,       0, 's' },
           {"vop",   no_argument,       0, 'v' },
           {"rkaiq",   no_argument,       0, 'r' },
           {"writefile",   no_argument,       0, 'e' },
           {0,          0,                 0,  0  }
       };

       //c = getopt_long(argc, argv, "w:h:f:i:d:o:c:ps",
	c = getopt_long(argc, argv, "w:h:f:i:d:ps",
           long_options, &option_index);
       if (c == -1)
           break;

       switch (c) {
       case 'c':
           frame_count = atoi(optarg);
           break;
       case 'w':
           width = atoi(optarg);
           break;
       case 'h':
           height = atoi(optarg);
           break;
       case 'f':
           format = v4l2_fourcc(optarg[0], optarg[1], optarg[2], optarg[3]);
           break;
       case 'd':
           strcpy(dev_name, optarg);
           break;
       /*case 'o':
           strcpy(out_file, optarg);
           break;*/
       case 's':
           silent = 1;
           break;
       case 'v':
           vop = 1;
           break;
       case 'r':
           rkaiq = 1;
           break;
       case 'e':
           writeFile = 1;
           break;
       case '?':
       case 'p':
           ERR("Usage: %s to capture rkisp1 frames\n"
                  "         --width,  default 640,             optional, width of image\n"
                  "         --height, default 480,             optional, height of image\n"
                  "         --format, default NV12,            optional, fourcc of format\n"
                  "         --count,  default 1000,            optional, how many frames to capture\n"
                  "         --device,                          required, path of video device\n"
                  "         --output,                          required, output file path, if <file> is '-', then the data is written to stdout\n"
                  "         --vop,                             optional, drm display\n"
                  "         --rkaiq,                           optional, auto image quality\n",
                  "         --silent,                          optional, subpress debug log\n",
                  argv[0]);
           exit(-1);

       default:
           ERR("?? getopt returned character code 0%o ??\n", c);
       }
   }

   if (strlen(dev_name) == 0) {
        ERR("arguments --output and --device are required\n");
        exit(-1);
   }

}

static void signal_handle(int signo)
{
    printf("force exit !!!\n");
	if (aiq_ctx) {
        printf("-------- stop aiq -------------\n");
		rk_aiq_uapi_sysctl_stop(aiq_ctx);
        printf("-------- deinit aiq -------------\n");
		rk_aiq_uapi_sysctl_deinit(aiq_ctx);
        printf("-------- deinit aiq end -------------\n");
	}

    stop_capturing();
	if (writeFile)
	    fclose(fp);
    uninit_device();
    close_device();
    exit(0);
}

int main(int argc, char **argv)
{
    signal(SIGINT, signal_handle);
    parse_args(argc, argv);

    open_device();
    init_device();

	if (rkaiq) {
		aiq_ctx = rk_aiq_uapi_sysctl_init("m01_f_ov4689 1-0036", NULL, NULL, NULL);

		if (aiq_ctx) {
			XCamReturn ret = rk_aiq_uapi_sysctl_prepare(aiq_ctx, width, height, RK_AIQ_WORKING_MODE_ISP_HDR3);
			if (ret != XCAM_RETURN_NO_ERROR)
				ERR("rk_aiq_uapi_sysctl_prepare failed: %d\n", ret);
			else {
				start_capturing();
				if (writeFile)
				    fp = fopen( "/tmp/capture_yuv.yuv", "a" );
				ret = rk_aiq_uapi_sysctl_start(aiq_ctx );
			}

			if (vop) {
				if (initDrmDsp() < 0) {
					printf("DRM display init failed\n");
					exit(0);
				}
			}

			usleep(500 * 1000);
		}
	} else {
		if (vop) {
			if (initDrmDsp() < 0) {
				printf("DRM display init failed\n");
				exit(0);
			}
		}

		usleep(500 * 1000);

		start_capturing();
	}

        mainloop();

	if (aiq_ctx) {
        printf("-------- stop aiq -------------\n");
		rk_aiq_uapi_sysctl_stop(aiq_ctx);
        printf("-------- deinit aiq -------------\n");
		rk_aiq_uapi_sysctl_deinit(aiq_ctx);
        printf("-------- deinit aiq end -------------\n");
	}

    stop_capturing();
	if (writeFile)
	    fclose(fp);
    uninit_device();
    close_device();
    return 0;
}

set(acsm_SRC
    rk_aiq_algo_acsm_itf.cpp
    rk_aiq_uapi_acsm.cpp
    )

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rk_aiq_algo_acsm_itf.cpp")
    add_library(rkaiq_acsm STATIC ${acsm_SRC})
    set_target_properties(rkaiq_acsm PROPERTIES FOLDER "algos/acsm")
    set_target_properties(rkaiq_acsm PROPERTIES CLEAN_DIRECT_OUTPUT 1)
    set_target_properties(rkaiq_acsm PROPERTIES C_VISIBILITY_PRESET default)
    set_target_properties(rkaiq_acsm PROPERTIES CXX_VISIBILITY_PRESET default)
else()
    message(STATUS "Can NOT found ${CMAKE_CURRENT_SOURCE_DIR}/rk_aiq_algo_acsm_itf.cpp. Using lib")
    add_library(rkaiq_acsm STATIC IMPORTED)
    set_target_properties(rkaiq_acsm PROPERTIES IMPORTED_LOCATION
        "${PREFIX}rkaiq_acsm.${SUFFIX}")
    set_target_properties(rkaiq_acsm PROPERTIES IMPORTED_IMPLIB
        "${PREFIX}rkaiq_acsm.${SUFFIX}")
endif()

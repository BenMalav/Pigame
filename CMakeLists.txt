# Referenced - https://dev.to/younup/cmake-on-stm32-the-beginning-3766
# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/
# https://interrupt.memfault.com/blog/best-and-worst-gcc-clang-compiler-flags

cmake_minimum_required(VERSION 3.15)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)

project(pi_game)

set(PROJECT_FILES
        Shader.h
        test.cpp
        ShapeGenerator.cpp ShapeGenerator.h
        DispmanCapture.cpp DispmanCapture.h
        GraphicsContext.cpp GraphicsContext.h)

set(EXECUTABLE ${PROJECT_NAME}.out)

add_executable(${EXECUTABLE} ${PROJECT_FILES})

#target_compile_definitions(${EXECUTABLE} PRIVATE
#        -DSTM32L4
#        )

find_package(glm REQUIRED)

find_library(LIBBCM PATHS /opt/vc/lib/
            NAMES bcm_host)

if(${LIBBCM} STREQUAL LIBBCM-NOTFOUND)
    message(FATAL_ERROR "bcm_host library not found!")
endif()


target_link_libraries(${EXECUTABLE} PRIVATE
        glm
        ${LIBBCM})

target_include_directories(${EXECUTABLE} PRIVATE
        ./
        /usr/include/libdrm
        /usr/include/GLES2
        /opt/vc/include
        /opt/vc/include/interface/vcos/pthreads
        /opt/vc/include/interface/vmcs_host/linux
        ${GLM_INCLUDE_DIRS}
        )

target_compile_options(${EXECUTABLE} PRIVATE

        -march=native
        -mcpu=native
        -mtune=native

        -fdata-sections
        -ffunction-sections
        -fno-common

        -Wall
        -Wextra
        -Wcast-align
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wlogical-op
        -Wsuggest-final-types
        -Wsuggest-final-methods
        -pedantic
        $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
        $<$<COMPILE_LANGUAGE:CXX>:-Wsuggest-override>

        $<$<CONFIG:Debug>:-Og>
        )

target_link_options(${EXECUTABLE} PRIVATE

        -march=native
        -mcpu=native
        -mtune=native

        -lc
        -lgcc

        -ldrm
        -lgbm
        -lEGL
        -lGLESv2
        -lm

        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        -Wl,--print-memory-usage
        -Wl,--print-gc-sections
        )


# Improve clean target
#[[set_target_properties(${EXECUTABLE} PROPERTIES ADDITIONAL_CLEAN_FILES
        "${PROJECT_NAME}.bin;${PROJECT_NAME}.hex;${PROJECT_NAME}.map")]]

# Print executable size
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND size ${EXECUTABLE})

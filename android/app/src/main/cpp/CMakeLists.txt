# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("zlog")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#定义全局 my_source_path 变量
file(GLOB my_source_path
        ${CMAKE_SOURCE_DIR}/*.cpp
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.cc
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/micro-ecc-master/*.c
        ${CMAKE_SOURCE_DIR}/src/micro-ecc-master/*.inc
        ${CMAKE_SOURCE_DIR}/src/micro-ecc-master/*.h)

add_library( # Sets the name of the library.
             zlog-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${my_source_path})

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                        zlog-lib
                        z
                    )
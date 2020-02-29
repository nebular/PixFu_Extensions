cmake_minimum_required(VERSION 3.4.1)
# set targetPlatform, will be passed in from gradle when this sample is completed
# openGL Supportability
# platform         status
#   (0 12)           ES2/ES3 not supported
#   [12, 18)         ES2 only; for ES3, app do dynamic load/detection
#                    this applies to the situations that:
#                        - minimum API is set to less than 18. In this case
#                          there is no ES3 header/lib support inside NDK
#                        - the built APK might be running on newer API phones
#                    with dynamic loading of ES3, the same APK would still be able
#                    to use ES3. Otherwise, app would stuck with ES2 even phone is
#                    is newer than the minimum API level (for example, Android-27 etc).
#
#   [18, 24)         ES2 & ES3
#                    If app is built to only support API-18 or later,
#                    set minimum api level to 18 is good enough, NDK supprts ES3
#                    with the right header and lib files. No need to use ES3 dynamic
#                    detection.
#   [24, infinite)   ES2 & ES3 & Vulkan

include_directories(../pixFu/src)
include_directories(../pixFu/src/core/headers)
include_directories(../pixFu/src/core/headers)
include_directories(../pixFu/src/items/headers)
include_directories(../pixFu/src/input/headers)
include_directories(../pixFu/src/thirdparty/glm)

include_directories(${CMAKE_SOURCE_DIR})

include_directories(World/)
include_directories(World/core/headers)
include_directories(World/geom)
include_directories(World/worlds/ballworld/headers)
include_directories(Sprites/headers)

## PIXENGINE

add_library(pixFu_ext SHARED
        World/core/Camera.cpp
        World/core/ObjectCluster.cpp
        World/core/ObjectShader.cpp
        World/core/ObjLoader.cpp
        World/core/Terrain.cpp
        World/core/TerrainShader.cpp
        World/core/World.cpp
        World/worlds/ballworld/Ball.cpp
        World/worlds/ballworld/BallWorld.cpp
        World/worlds/ballworld/Player.cpp
        Sprites/SpriteSheets.cpp
        Sprites/SpriteSheet.cpp
        )


# Include libraries needed for gles3jni lib

target_link_libraries(pixFu_ext
        pixFu
        m)

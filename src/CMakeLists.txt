add_library(snowboarding_lib STATIC
    "player.cpp"
    "camera.cpp"
    "mesh_navigator.cpp"
    "obstacle.cpp"
    "accelerator.cpp"
)
set_property(TARGET snowboarding_lib PROPERTY CXX_STANDARD 20)

target_include_directories(snowboarding_lib PRIVATE ${MONA_INCLUDE_DIRECTORY} ${THIRD_PARTY_INCLUDE_DIRECTORIES} "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(snowboarding_lib PRIVATE MonaEngine)

set(CMAKE_LEGACY_CYGWIN_WIN32 OFF)
cmake_minimum_required(VERSION 3.15)
project(Snowboarding C CXX)

add_subdirectory(extern/MonaEngine)
add_subdirectory(src)

set(snowboarding_lib_INCLUDE_DIRECTORY include)

add_executable(Snowboarding main.cpp) 

set_property(TARGET Snowboarding  PROPERTY CXX_STANDARD 20)
target_link_libraries(Snowboarding  PRIVATE MonaEngine snowboarding_lib)
target_include_directories(Snowboarding  PRIVATE  ${MONA_INCLUDE_DIRECTORY} ${THIRD_PARTY_INCLUDE_DIRECTORIES} ${snowboarding_lib_INCLUDE_DIRECTORY})
add_custom_command(TARGET Snowboarding POST_BUILD        
		COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        $<TARGET_FILE:OpenAL> $<TARGET_FILE_DIR:Snowboarding>)

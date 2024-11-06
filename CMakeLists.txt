set(CMAKE_LEGACY_CYGWIN_WIN32 OFF)
cmake_minimum_required(VERSION 3.15)
project(Snowboarding C CXX)

option(MONA_BUILD_EXAMPLES "" OFF)
option(MONA_INSTALL "" OFF)
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

set(APPLICATION_ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)
set(ENGINE_ASSETS_DIR ${CMAKE_SOURCE_DIR}/extern/MonaEngine/EngineAssets)
configure_file(${CMAKE_SOURCE_DIR}/extern/MonaEngine/config.json.in config.json)

add_custom_command(TARGET Snowboarding POST_BUILD        
		COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        config.json $<TARGET_FILE_DIR:Snowboarding>)


include(GNUInstallDirs)
install(TARGETS Snowboarding
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT Snowboarding_Runtime
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT Snowboarding_Include
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Snowboarding_Runtime
        NAMELINK_COMPONENT Snowboarding_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Snowboarding_Development
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
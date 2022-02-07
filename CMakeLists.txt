cmake_minimum_required(VERSION 3.21)
project(learn_opengl)

function(copy_resources RES_PATH DEST_PATH)
    file(GLOB_RECURSE res_list RELATIVE ${RES_PATH} CONFIGURE_DEPENDS ${RES_PATH}/*)

    foreach(res ${res_list})
        configure_file(${RES_PATH}/${res} "${DEST_PATH}/${res}" COPYONLY)
    endforeach()
endfunction()

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (NOT (EXISTS ${PROJECT_SOURCE_DIR}/lib))
    message(FATAL_ERROR "Dependencies not found. Please run fetch_deps.sh [-G Ninja] from the project root and reconfigure")
endif ()

find_package(glad REQUIRED HINTS ${PROJECT_SOURCE_DIR}/lib)
find_package(glfw3 REQUIRED HINTS ${PROJECT_SOURCE_DIR}/lib)
find_package(glm REQUIRED HINTS ${PROJECT_SOURCE_DIR}/lib)
find_package(stb REQUIRED HINTS ${PROJECT_SOURCE_DIR}/lib)
find_package(tinyobjloader REQUIRED HINTS ${PROJECT_SOURCE_DIR}/lib)

set(LIBS glad::glad glm::glm glfw stb tinyobjloader::tinyobjloader)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

copy_resources(${CMAKE_CURRENT_LIST_DIR}/res ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res)


set(ACTIVITY app)
add_executable(${ACTIVITY} include/shader.hpp include/camera.hpp include/model.hpp src/model.cpp include/file_utils.hpp include/renderer.hpp src/renderer.cpp include/map.hpp)
target_include_directories(${ACTIVITY} PUBLIC include)
target_sources(${ACTIVITY}
        ${target}
        PRIVATE
        src/main.cpp
        )
target_link_libraries(${ACTIVITY} PUBLIC ${LIBS})

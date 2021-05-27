cmake_minimum_required(VERSION 3.8)

set ( CMAKE_EXPORT_COMPILE_COMMANDS TRUE )
set ( PROJECT_NAME ants_meta )
project( ${PROJECT_NAME} )

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/cmake-modules)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/extern/Catch2/contrib)
# list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/extern/Catch2/extras)

add_definitions(/W0)


add_subdirectory(csv_row)
add_subdirectory(memory_pool)
add_subdirectory(transform)

add_subdirectory(geometry)

add_subdirectory(gl_classes)

add_subdirectory(gui_application)
add_subdirectory(im_param)

add_subdirectory(im_param__gl_classes)

add_subdirectory(ants)
add_subdirectory(balls)

add_subdirectory(agents_app)


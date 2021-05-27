cmake_minimum_required(VERSION 3.8)

add_library(
    ${PROJECT_NAME} 
    INTERFACE
)

# target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::GL)
# target_link_libraries(${PROJECT_NAME} PUBLIC GLEW::glew)
# target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
# target_link_libraries(${PROJECT_NAME} PUBLIC glm)
# target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)

# target_link_libraries(${PROJECT_NAME} PUBLIC recursive_traversal_visitor)
# target_link_libraries(${PROJECT_NAME} PUBLIC csv_row)

target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
# target_compile_options(
#     ${PROJECT_NAME}
#     PUBLIC 
#     # glm::vec4().xyz , etc..
#     -DGLM_FORCE_SWIZZLE 
#     -DGLM_FORCE_INLINE 
# )

set(
    PROGRAMS
)
foreach(PROGRAM_NAME ${PROGRAMS})
    set(EXECUTABLE_NAME "${PROJECT_NAME}_${PROGRAM_NAME}")
    add_executable(${EXECUTABLE_NAME} src/${PROGRAM_NAME}.cpp)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${PROJECT_NAME})
endforeach()


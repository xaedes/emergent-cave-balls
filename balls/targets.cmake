cmake_minimum_required(VERSION 3.8)

add_library(
    ${PROJECT_NAME} 
    STATIC
    
    include/${PROJECT_NAME}/version.h
    src/balls.cpp
    src/balls_program.cpp
    src/render_program.cpp
    src/interactions_program.cpp
)

# target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::GL)
# target_link_libraries(${PROJECT_NAME} PUBLIC GLEW::glew)
# target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
# target_link_libraries(${PROJECT_NAME} PUBLIC glm)
# target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)

target_link_libraries(${PROJECT_NAME} PUBLIC geometry)

target_link_libraries(${PROJECT_NAME} PUBLIC im_param)
target_link_libraries(${PROJECT_NAME} PUBLIC im_param__gl_classes)
# target_link_libraries(${PROJECT_NAME} PUBLIC detection_types)
# target_link_libraries(${PROJECT_NAME} PUBLIC kinematic_emwa_filter)
# target_link_libraries(${PROJECT_NAME} PUBLIC multi_object_tracker)
# target_link_libraries(${PROJECT_NAME} PUBLIC CGAL::CGAL)
# target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
# target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})

target_link_libraries(${PROJECT_NAME} PUBLIC gl_classes)
target_link_libraries(${PROJECT_NAME} PUBLIC gui_application)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

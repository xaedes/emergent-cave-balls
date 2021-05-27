cmake_minimum_required(VERSION 3.8)

add_library(
    ${PROJECT_NAME} 
    STATIC
    
    include/${PROJECT_NAME}/version.h
    include/${PROJECT_NAME}/app.h
    
    src/app.cpp
    src/entity_browser.cpp
    src/gui_cameras.cpp
    src/renderer.cpp
    src/time.cpp
    src/simulation_clock.cpp
    src/random.cpp
    
    src/ants.cpp
    src/balls.cpp
)

# target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::GL)
# target_link_libraries(${PROJECT_NAME} PUBLIC GLEW::glew)
# target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
# target_link_libraries(${PROJECT_NAME} PUBLIC glm)
# target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)

# target_link_libraries(${PROJECT_NAME} PUBLIC detection_types)
# target_link_libraries(${PROJECT_NAME} PUBLIC kinematic_emwa_filter)
# target_link_libraries(${PROJECT_NAME} PUBLIC multi_object_tracker)
# target_link_libraries(${PROJECT_NAME} PUBLIC CGAL::CGAL)
# target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
# target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})

target_link_libraries(${PROJECT_NAME} PUBLIC geometry)
target_link_libraries(${PROJECT_NAME} PUBLIC gui_application)
target_link_libraries(${PROJECT_NAME} PUBLIC im_param)

target_link_libraries(${PROJECT_NAME} PUBLIC ants)
target_link_libraries(${PROJECT_NAME} PUBLIC balls)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_executable(${PROJECT_NAME}lication src/main.cpp)

target_link_libraries(${PROJECT_NAME}lication PRIVATE ${PROJECT_NAME})

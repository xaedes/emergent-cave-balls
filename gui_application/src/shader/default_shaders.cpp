#include "gui_application/shader/default_shaders.h"

namespace gui_application {
namespace shader {

    gl_classes::Shader defaultVertexShader()
    {
        using gl_classes::Shader;
        return Shader(Shader::ShaderType::Vertex, "default", defaultVertexShaderCode());
    }

    gl_classes::Shader defaultFragmentShader()
    {
        using gl_classes::Shader;
        return Shader(Shader::ShaderType::Fragment, "default", defaultFragmentShaderCode());
    }
    
    std::string defaultVertexShaderCode()
    {
        // https://github.com/opengl-tutorials/ogl/blob/master/tutorial04_colored_cube/TransformVertexShader.vertexshader        
        return 
        "#version 330 core                                                   \n"
        "                                                                    \n"
        "// Input vertex data, different for all executions of this shader.  \n"
        "layout(location = 0) in vec3 vertexPosition_modelspace;             \n"
        "layout(location = 1) in vec3 vertexColor;                           \n"
        "layout(location = 2) in mat4 projectionViewModelInstances;          \n"
        "// location 2,3,4,5 occupied by projectionViewModelInstances        \n"
        "out vec3 fragmentColor;                                             \n"
        "uniform mat4 projectionViewModel;                                   \n"
        "uniform int instanced = 0;                                          \n"
        "uniform float pointSize = 1;                                        \n"
        "                                                                    \n"
        "void main(){                                                        \n"
        "                                                                    \n"
        "    // Output position of the vertex, in clip space : projection_view_model * position \n"
        "    // either uniform or instance pvm should be set to identity!    \n"
        "    mat4 actualProjectionViewModel = (instanced != 0) ? projectionViewModelInstances : projectionViewModel; \n"
        "    gl_Position =  actualProjectionViewModel * vec4(vertexPosition_modelspace,1);  \n"
        // "    gl_Position =  projectionViewModel * vec4(vertexPosition_modelspace,1);  \n"
        "                                                                    \n"
        "    // The color of each vertex will be interpolated                \n"
        "    // to produce the color of each fragment                        \n"
        "    fragmentColor = vertexColor;                                    \n"
        "    gl_PointSize = pointSize;                                       \n"
        "}                                                                   \n"
        ;
    }

    std::string defaultFragmentShaderCode()
    {
        // https://github.com/opengl-tutorials/ogl/blob/master/tutorial04_colored_cube/ColorFragmentShader.fragmentshader
        return 
        "#version 330 core                                           \n"
        "                                                            \n"
        "// Interpolated values from the vertex shaders              \n"
        "in vec3 fragmentColor;                                      \n"
        "                                                            \n"
        "// Ouput data                                               \n"
        "out vec3 color;                                             \n"
        "                                                            \n"
        "void main(){                                                \n"
        "                                                            \n"
        "    // Output color = color specified in the vertex shader, \n"
        "    // interpolated between all 3 surrounding vertices      \n"
        "    color = fragmentColor;                                  \n"
        "                                                            \n"
        "}                                                           \n"
        ;
    }

}
}
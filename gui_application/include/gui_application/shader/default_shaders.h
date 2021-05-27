#pragma once

#include <string>
#include "gl_classes/shader.h"

namespace gui_application {
namespace shader {

    gl_classes::Shader defaultVertexShader();
    gl_classes::Shader defaultFragmentShader();

    std::string defaultVertexShaderCode();
    std::string defaultFragmentShaderCode();

}
}

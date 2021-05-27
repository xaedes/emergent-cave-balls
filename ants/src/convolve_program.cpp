#include "ants/convolve_program.h"
#include "ants/ant.h"

#include "im_param/backends/glsl_struct_generator_backend.h"

namespace ants {

    void ConvolveProgram::setup()
    {
        m_shaders = {Shader(Shader::ShaderType::Compute, code())};
        m_shaders[0].setup();
        Program::setup();
        dx.init(getGlProgram(), "dx");
        dy.init(getGlProgram(), "dy");
        width.init(getGlProgram(), "width");
        height.init(getGlProgram(), "height");
        kernel_width.init(getGlProgram(), "kernel_width");
        kernel_height.init(getGlProgram(), "kernel_height");
        multiplier.init(getGlProgram(), "multiplier");
        dt.init(getGlProgram(), "dt");
        gain.init(getGlProgram(), "gain");
        reference_dt.init(getGlProgram(), "reference_dt");
        checkGLError();
    }

    void ConvolveProgram::dispatch(int width, int height, float dt)
    {
        this->width.set(width);
        this->height.set(height);
        this->dt.set(dt);
        ComputeProgram::dispatch(width, height, 1, 16, 16 ,1);
    }

    std::string ConvolveProgram::code() const
    {
        return (
R"(
#version 440
#define GROUPSIZE_X 16
#define GROUPSIZE_Y 16
#define GROUPSIZE_Z 1
#define GROUPSIZE (GROUPSIZE_X*GROUPSIZE_Y*GROUPSIZE_Z)
layout(local_size_x=GROUPSIZE_X, local_size_y=GROUPSIZE_Y, local_size_z=GROUPSIZE_Z) in;

layout (binding = 0, rgba32f) uniform readonly  image2D src;
layout (binding = 1, rgba32f) uniform writeonly image2D dst;
layout (binding = 2, rgba32f) uniform readonly  image2D kernel;


uniform int width;
uniform int height;

uniform int dx;
uniform int dy;

uniform int kernel_width;
uniform int kernel_height;

uniform vec4 multiplier;
uniform vec4 gain;
uniform float dt;
uniform float reference_dt;

void main() {
    uint workgroup_idx = 
        gl_WorkGroupID.z * gl_NumWorkGroups.x * gl_NumWorkGroups.y +
        gl_WorkGroupID.y * gl_NumWorkGroups.x +
        gl_WorkGroupID.x;
    uint global_idx = gl_LocalInvocationIndex + workgroup_idx * GROUPSIZE;
    if (global_idx >= width*height) return;
    int x_idx = int(global_idx % width);
    int y_idx = int((global_idx - x_idx) / width);
    vec4 sum = vec4(0,0,0,0);
    vec4 w_sum = vec4(0,0,0,0);

    vec4 denom = (reference_dt / gain) + dt - reference_dt;
    vec4 the_gain = vec4(
        (abs(gain.x) < 1e-9) ? 0 : ( (abs(denom.x) < 1e-9) ? 1 : (dt / denom.x) ),
        (abs(gain.y) < 1e-9) ? 0 : ( (abs(denom.y) < 1e-9) ? 1 : (dt / denom.y) ),
        (abs(gain.z) < 1e-9) ? 0 : ( (abs(denom.z) < 1e-9) ? 1 : (dt / denom.z) ),
        (abs(gain.w) < 1e-9) ? 0 : ( (abs(denom.w) < 1e-9) ? 1 : (dt / denom.w) )
    );

    for (int y = 0; y <= kernel_height; ++y)
    {
        for (int x = 0; x < kernel_width; ++x)
        {
            vec4 w = imageLoad(kernel, ivec2(x, y));
            int u = x_idx+dx+x;
            int v = y_idx+dy+y;
            if ((0 <= u) && (u < width) && (0 <= v) && (v < height))
            {
                sum += imageLoad(src, ivec2(x_idx+dx+x, y_idx+dy+y)) * w;
                w_sum += w;
            }
        }        
    }
    ivec2 coord = ivec2(x_idx, y_idx);
    vec4 val = imageLoad(src, coord);
    vec4 new_val = multiplier * sum * (1/w_sum);
    imageStore(dst, coord, new_val * the_gain + (1-the_gain) * val );
}
)"
        );
    }

} // namespace ants

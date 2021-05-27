#include "ants/ants_build_grid_linked_list_program.h"
#include "ants/ant.h"

#include "im_param/backends/glsl_struct_generator_backend.h"

namespace ants {

    void AntsBuildGridLinkedListProgram::setup()
    {
        Ant ant;
        im_param::GlslStructGeneratorBackend glslStructGenerator;
        im_param::parameter(glslStructGenerator, "Ant", ant, im_param::TypeHolder<Ant>());

        m_shaders = {Shader(Shader::ShaderType::Compute, code())};
        m_shaders[0].setup({{"##ANT_STRUCT##", glslStructGenerator.glsl_string(1)}});

        Program::setup();
        num_items.init(getGlProgram(), "num_items");
        width.init(getGlProgram(), "width");
        height.init(getGlProgram(), "height");

        checkGLError();
    }

    void AntsBuildGridLinkedListProgram::dispatch(int num_items)
    {
        this->num_items.set(num_items);
        ComputeProgram::dispatch(num_items, 1, 1, 1024, 1 ,1);
    }

    std::string AntsBuildGridLinkedListProgram::code() const
    {
        return (
R"(
#version 440
#define GROUPSIZE_X 1024
#define GROUPSIZE_Y 1
#define GROUPSIZE_Z 1
#define GROUPSIZE (GROUPSIZE_X*GROUPSIZE_Y*GROUPSIZE_Z)
layout(local_size_x=GROUPSIZE_X, local_size_y=GROUPSIZE_Y, local_size_z=GROUPSIZE_Z) in;

##ANT_STRUCT##

layout (std430, binding = 0) buffer buf_ants
{
    Ant ants[];
};

layout (std430, binding = 1) buffer buf_linked_list_heads
{
    int ll_head[]; // points into ants and into ll_next
};

layout (std430, binding = 2) buffer buf_linked_list_nodes
{
    int ll_next[]; // one item per ant, points into ants and into ll_next
};

uniform uint num_items;
uniform uint width;
uniform uint height;

uvec2 to_grid_coord(vec2 pos)
{
    return uvec2(
        clamp(uint(floor(pos.x)), 0, width-1),
        clamp(uint(floor(pos.y)), 0, height-1)
    );
}

void main() {
    uint workgroup_idx = 
        gl_WorkGroupID.z * gl_NumWorkGroups.x * gl_NumWorkGroups.y +
        gl_WorkGroupID.y * gl_NumWorkGroups.x +
        gl_WorkGroupID.x;
    uint global_idx = gl_LocalInvocationIndex + workgroup_idx * GROUPSIZE;
    if (global_idx >= num_items) return;

    Ant ant = ants[global_idx];

    uvec2 pos = to_grid_coord(ant.pos_vel.xy);
    uint grid_idx = pos.x + pos.y * width;

    ll_next[global_idx] = atomicExchange(ll_head[grid_idx], int(global_idx));
}
)"
        );
    }

} // namespace ants

#include "ants/ants_apply_environment_output_program.h"
#include "ants/ant.h"

#include "im_param/backends/glsl_struct_generator_backend.h"

namespace ants {

    void AntsApplyEnvironmentOutputProgram::setup()
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
        max_traverse.init(getGlProgram(), "max_traverse");

        checkGLError();
    }

    void AntsApplyEnvironmentOutputProgram::dispatch(int num_items)
    {
        this->num_items.set(num_items);
        ComputeProgram::dispatch(num_items, 1, 1, 32, 1 ,1);
    }

    std::string AntsApplyEnvironmentOutputProgram::code() const
    {
        return (
R"(
#version 440
#define GROUPSIZE_X 32
#define GROUPSIZE_Y 1
#define GROUPSIZE_Z 1
#define GROUPSIZE (GROUPSIZE_X*GROUPSIZE_Y*GROUPSIZE_Z)
layout(local_size_x=GROUPSIZE_X, local_size_y=GROUPSIZE_Y, local_size_z=GROUPSIZE_Z) in;

layout (binding = 0, rgba32f) uniform image2D texEnv;
layout (binding = 1, rgba32f) uniform image2D texDisplay;

##ANT_STRUCT##

layout (std430, binding = 0) buffer buf_ants // shape [M]
{
    Ant ants[];
};

layout (std430, binding = 1) buffer buf_linked_list_heads // shape [W*H]
{
    int ll_head[]; // points into ants and into ll_next
};

layout (std430, binding = 2) buffer buf_linked_list_nodes // shape [M]
{
    int ll_next[]; // one item per ant, points into ants and into ll_next
};

layout (std430, binding = 3) buffer buf_linked_list_toprocess // shape[N]
{
    // contains the indices of heads to be processed
    int ll_toprocess[]; // points into ll_head
};

layout (std430, binding = 4) buffer buf_linked_list_remaining // shape[N]
{
    // after reaching maximum traversal count, the shader ends
    // without completing the traversal of all linked lists.
    // the linked lists which didn't reach its end, will be stored here.
    int ll_remaining[]; // points into ll_head
};

layout (std430, binding = 5) buffer buf_num_remaining // shape [1]
{
    int num_remaining[];
};


uniform uint num_items;
uniform uint width;
uniform uint height;
uniform uint max_traverse;

// shared uint remaining_shared[GROUPSIZE];
shared int remaining_in_group;
shared int global_remaining_idx;


void main() {

    if (gl_LocalInvocationIndex == 0)
    {
        remaining_in_group = 0;
    }
    groupMemoryBarrier();

    uint workgroup_idx = 
        gl_WorkGroupID.z * gl_NumWorkGroups.x * gl_NumWorkGroups.y +
        gl_WorkGroupID.y * gl_NumWorkGroups.x +
        gl_WorkGroupID.x;
    uint global_idx = gl_LocalInvocationIndex + workgroup_idx * GROUPSIZE;
    if (global_idx >= num_items) return;
    int idx = ll_toprocess[global_idx];
    if (idx < 0) return;

    int y = int(floor(idx / width));
    int x = idx - y*int(width);
    ivec2 coord = ivec2(x,y);

    // each invocation is the only one which accesses by idx, or by coord
    int current = ll_head[idx];
    vec4 env = imageLoad(texEnv, coord);
    vec4 display = imageLoad(texDisplay, coord);

    for(uint i=0; i<max_traverse; ++i)
    {
        if (current < 0)
        {
            break;
        }

        env += ants[current].environment_delta;
        display = max(display, ants[current].display_value);

        current = ll_next[current];
    }
    ll_head[idx] = current;
    imageStore(texEnv, coord, env);
    imageStore(texDisplay, coord, display);

    // when end not reached store idx into ll_remaining
    // first collect into shared buffer
    // then perform atomicAdd between all shader invocations
    // to get offsets for final placement in output buffer ll_remaining
    int local_remaining_idx = -1;
    if (current >= 0)
    {
        // ll_remaining[atomicAdd(num_remaining[0], 1)] = idx;

        local_remaining_idx = atomicAdd(remaining_in_group, 1);

        
        // remaining_shared[local_remaining_idx] = idx;
    }
    // memoryBarrier();
    // memoryBarrierShared();
    groupMemoryBarrier();
    barrier();
    if ((gl_LocalInvocationIndex == 0) && (remaining_in_group > 0))
    {
        global_remaining_idx = atomicAdd(num_remaining[0], remaining_in_group);
    }
    // memoryBarrier();
    // memoryBarrierShared();
    groupMemoryBarrier();
    barrier();
    if (local_remaining_idx >= 0)
    {
        ll_remaining[local_remaining_idx + global_remaining_idx] = idx;
    }
}
)"
        );
    }

} // namespace ants

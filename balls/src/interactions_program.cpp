#include "balls/interactions_program.h"
#include "balls/ball.h"

#include "geometry/pi.h"

#include "im_param/backends/glsl_struct_generator_backend.h"

namespace balls {

    void InteractionsProgram::setup(uint32_t width_, uint32_t height_)
    {
        Ball ball;
        im_param::GlslStructGeneratorBackend glslStructGenerator;
        im_param::parameter(glslStructGenerator, "Ball", ball, im_param::TypeHolder<Ball>());

        m_shaders = {Shader(Shader::ShaderType::Compute, code())};
        m_shaders[0].setup({{"##BALL_STRUCT##", glslStructGenerator.glsl_string(1)}});
        Program::setup();
        num_items.init(getGlProgram(), "num_items");
        max_iterations.init(getGlProgram(), "max_iterations", 255);
        width.init(getGlProgram(), "width", width_);
        height.init(getGlProgram(), "height", height_);
        max_abs_dx.init(getGlProgram(), "max_abs_dx", 1);
        max_abs_dy.init(getGlProgram(), "max_abs_dy", 1);
        force_strength.init(getGlProgram(), "force_strength", 1);
        dt.init(getGlProgram(), "dt");
        is_first_invocation.init(getGlProgram(), "is_first_invocation");

        checkGLError();
    }

    void InteractionsProgram::dispatch(uint32_t num_items, float dt, bool is_first_invocation)
    {
        this->num_items.set(num_items);
        this->dt.set(dt);
        this->is_first_invocation.set(is_first_invocation);
        checkGLError();
        ComputeProgram::dispatch(num_items, 1, 1, 1024, 1 ,1);
    }

    std::string InteractionsProgram::code() const
    {
        return (
R"(
#version 440
#define GROUPSIZE_X 1024
#define GROUPSIZE_Y 1
#define GROUPSIZE_Z 1
#define GROUPSIZE (GROUPSIZE_X*GROUPSIZE_Y*GROUPSIZE_Z)
layout(local_size_x=GROUPSIZE_X, local_size_y=GROUPSIZE_Y, local_size_z=GROUPSIZE_Z) in;

#define M_PI 3.1415926535897932384626433832795
#define D2R (M_PI/180)

##BALL_STRUCT##

// since we don't process all items (only the ones in toprocess), we need a readwrite
// buffer, so we don't have to copy over non-processed items.
// we write in other fields than we read and only one invocation writes one item
// therefore we can read and write from one buffer.
layout (std430, binding = 0) buffer buf_balls                     // shape[N]
{
    Ball balls[];
};

layout (std430, binding = 1) buffer buf_linked_list_heads         // shape[W*H]
{
    int ll_head[]; // points into balls and into ll_next
};

layout (std430, binding = 2) buffer buf_linked_list_next          // shape[N]
{
    int ll_next[]; // one item per ball, points into balls and into ll_next
};

layout (std430, binding = 3) buffer buf_linked_list_toprocess     // shape[N]
{
    // contains the indices of balls to be processed
    int toprocess[]; // points into balls
};

layout (std430, binding = 4) buffer buf_linked_list_remaining     // shape[N]
{
    // after reaching maximum traversal count, the shader ends
    // without completing the traversal of all linked lists.
    // the balls which didn't reach its end, will be stored here.
    int remaining[]; // points into balls
};

layout (std430, binding = 5) buffer buf_num_remaining             // shape[1]
{
    // after reaching maximum traversal count, the shader ends
    // without completing the traversal of all linked lists.
    // the balls which didn't reach its end, will be stored here.
    int num_remaining[]; // points into balls
};

layout (std430, binding = 6) buffer buf_neighbor_iterators        // shape[N]
{
    ivec4 neighbor_iterators[]; // one item per ball: item.xy points into ll_head, item.z points into balls
};

uniform uint num_items;
uniform uint max_iterations;
uniform uint width;
uniform uint height;

// defines size of neighborhood
uniform int max_abs_dx = 1;
uniform int max_abs_dy = 1;

uniform float force_strength = 1;
uniform float dt;
uniform bool is_first_invocation;

Ball our_ball;
int cx;
int cy;
int min_nx;
int max_nx;
int min_ny;
int max_ny;

uvec2 to_grid_coord(vec2 pos)
{
    return uvec2(
        clamp(uint(floor(pos.x)), uint(0), uint(width-1)),
        clamp(uint(floor(pos.y)), uint(0), uint(height-1))
    );
}

ivec4 first_neighbor()
{
    int cell_idx = min_nx + min_ny * int(width);
    int ball_idx = ll_head[cell_idx];
    // if cell is empty, it will be iterated over
    return ivec4(min_nx, min_ny, ball_idx,0);
}

ivec4 advance_neighbor(ivec4 it)
{
    // either the next of ll in current cell
    if (it.z >= 0)
    {
        it.z = ll_next[it.z];
        return it;
    }
    // or the head of the next cell
    it.x = it.x + 1;
    if (it.x > max_nx)
    {
        it.x = min_nx;
        it.y = it.y + 1;
        if (it.y > max_ny)
        {
            // no more cells
            return ivec4(-1,-1,-1,0);
        }
    }
    int cell_idx = it.x + it.y * int(width);
    it.z = ll_head[cell_idx];
    return it;
}

bool is_valid(ivec4 it)
{
    return (it.x >= 0) && (it.y >= 0) && (it.z >= 0);
}

bool has_ended(ivec4 it)
{
    return (it.x < 0) || (it.y < 0);
}

int process_pair(int idx_other)
{
    int num_processed = 0;
    Ball other = balls[idx_other];
    // NOTE:
    // don't read from other.fields that we write into our_ball: impulse
    // these fields may be changed by other invocations!
    
    vec2 delta_pos = other.pos - our_ball.pos;
    float dist = length(delta_pos);
    const float eps = 1e-6f;
    if (dist < our_ball.radius + other.radius)
    {
        // apply forces to our_ball
        vec2 force_direction = vec2(0,0);
        float strength = force_strength;
        if (dist > eps)
        {
            force_direction = -normalize(delta_pos);
            float dist_norm = (dist / (our_ball.radius + other.radius));
            strength *= 1 / dist_norm;
        }
        else
        {
            // very close together, use pseudo random direction. noise1: The
            // return value(s) are always in the range [-1.0,1.0], and cover at
            // least the range [-0.6, 0.6], with a Gaussian-like distribution.
            // scale it up a lot and rely on 2*pi wrapping of angles, to get roughly
            // uniform distribution over angles
            float random_angle = noise1(other.pos)*M_PI*2*8*8; 
            force_direction = vec2(cos(random_angle), sin(random_angle));
            strength *= 1000;
        }
        vec2 force = force_direction * strength;
        our_ball.impulse += force * dt;
        num_processed = 1;
    }
    return num_processed;
}


void main() {
    uint workgroup_idx = 
        gl_WorkGroupID.z * gl_NumWorkGroups.x * gl_NumWorkGroups.y +
        gl_WorkGroupID.y * gl_NumWorkGroups.x +
        gl_WorkGroupID.x;
    uint global_idx = gl_LocalInvocationIndex + workgroup_idx * GROUPSIZE;
    if (global_idx >= num_items) return;
    //return;
    //*
    int ball_idx = toprocess[global_idx];
    if (ball_idx < 0) return;

    our_ball = balls[ball_idx];

    uvec2 grid_pos = to_grid_coord(our_ball.pos);
    cx = int(grid_pos.x);
    cy = int(grid_pos.y);
    min_nx = cx - max_abs_dx; if (min_nx < 0) min_nx = 0;
    max_nx = cx + max_abs_dx; if (min_nx > width-1) min_nx = int(width)-1;
    min_ny = cy - max_abs_dy; if (min_ny < 0) min_ny = 0;
    max_ny = cy + max_abs_dy; if (max_ny > height-1) max_ny = int(height)-1;


    ivec4 it = neighbor_iterators[ball_idx];
    if (is_first_invocation)
    {
        it = first_neighbor();
    }

    //*
    int num_processed = 0;
    for (uint i=0; i<max_iterations; ++i)
    {

        if (is_valid(it) && (it.z != ball_idx))
        {
            num_processed += process_pair(it.z);
        }
        it = advance_neighbor(it);
        if (has_ended(it)) break;
    }
    //*/
    it.w += num_processed;
    // this invocation is the only one accessing neighbor_iterators[ball_idx]
    // therefore it is safe to write.
    neighbor_iterators[ball_idx] = it;

    if (!has_ended(it))
    {
        int idx_remaining = atomicAdd(num_remaining[0], 1);
        remaining[idx_remaining] = ball_idx;
    }
    
    atomicAdd(num_remaining[1], num_processed);

    balls[ball_idx] = our_ball;
    //*/

}
)"
        );
    }    
} // namespace balls

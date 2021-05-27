#include "balls/balls_program.h"
#include "balls/ball.h"

#include "geometry/pi.h"

#include "im_param/backends/glsl_struct_generator_backend.h"

namespace balls {

    void BallsProgram::setup(int32_t width_, int32_t height_)
    {
        Ball ball;
        im_param::GlslStructGeneratorBackend glslStructGenerator;
        im_param::parameter(glslStructGenerator, "Ball", ball, im_param::TypeHolder<Ball>());

        m_shaders = {Shader(Shader::ShaderType::Compute, code())};
        m_shaders[0].setup({{"##BALL_STRUCT##", glslStructGenerator.glsl_string(1)}});
        Program::setup();
        num_items.init(getGlProgram(), "num_items");
        dt.init(getGlProgram(), "dt");
        width.init(getGlProgram(), "width", width_);
        height.init(getGlProgram(), "height", height_);
        ref_dt.init(getGlProgram(), "ref_dt", 0.1);
        gain_velocity_dampening.init(getGlProgram(), "gain_velocity_dampening", 0.01);
        gravity_x.init(getGlProgram(), "gravity_x", 0);
        gravity_y.init(getGlProgram(), "gravity_y", 0);
        gravity_t.init(getGlProgram(), "gravity_t", 0);
        gravity_t2.init(getGlProgram(), "gravity_t2", 0);
        gravity_tx.init(getGlProgram(), "gravity_tx", width_/2);
        gravity_ty.init(getGlProgram(), "gravity_ty", height_/2);
        gravity_tr.init(getGlProgram(), "gravity_tr", 0);
        gravity_tm.init(getGlProgram(), "gravity_tm", 0);
        gravity_td.init(getGlProgram(), "gravity_td", 1);
        max_speed.init(getGlProgram(), "max_speed", 10);

        checkGLError();
    }

    void BallsProgram::dispatch(uint32_t num_items, float dt)
    {
        this->num_items.set(num_items);
        this->dt.set(dt);
        checkGLError();
        ComputeProgram::dispatch(num_items, 1, 1, 1024, 1 ,1);
    }

    std::string BallsProgram::code() const
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

layout (std430, binding = 0) buffer buf_balls
{
    Ball balls[];
};

layout (std430, binding = 1) buffer buf_out_balls
{
    Ball out_balls[];
};

layout (std430, binding = 2) buffer buf_out_points
{
    vec4 out_points[];
};

layout (std430, binding = 3) buffer buf_out_colors
{
    vec4 out_colors[];
};

layout (std430, binding = 4) buffer buf_linked_list_heads
{
    int ll_head[]; // points into balls and into ll_next
};

layout (std430, binding = 5) buffer buf_linked_list_nodes
{
    int ll_next[]; // one item per ball, points into balls and into ll_next
};

layout (std430, binding = 6) buffer buf_linked_list_counts
{
    int ll_count[]; // one item per ball
};

uniform uint num_items;
uniform float dt;
uniform int width;
uniform int height;

uniform float ref_dt;
uniform float gain_velocity_dampening;

uniform float gravity_x;
uniform float gravity_y;

uniform float gravity_t;
uniform float gravity_t2;
uniform float gravity_tx;
uniform float gravity_ty;
uniform float gravity_tr;
uniform float gravity_tm;
uniform float gravity_td;
uniform float max_speed;

float gain_for_dt(float gain, float ref_dt, float dt)
{
    float denom = (abs(gain) < 1e-9) ? 0 : ((ref_dt / gain) + dt - ref_dt);
    return (abs(gain) < 1e-9) ? 0 : ( (abs(denom) < 1e-9) ? 1 : (dt / denom) );
}

float emwa(float gain, float ref_dt, float dt, float old_val, float new_val)
{
    float gain_dt = gain_for_dt(gain, ref_dt, dt);
    return new_val * gain_dt + (1-gain_dt) * old_val;
}

vec2 emwa(float gain, float ref_dt, float dt, vec2 old_val, vec2 new_val)
{
    float gain_dt = gain_for_dt(gain, ref_dt, dt);
    return new_val * gain_dt + (1-gain_dt) * old_val;
}

uvec2 to_grid_coord(vec2 pos)
{
    return uvec2(
        clamp(uint(floor(pos.x)), uint(0), uint(width-1)),
        clamp(uint(floor(pos.y)), uint(0), uint(height-1))
    );
}

vec2 compute_gravity(Ball ball)
{
    vec2 gravity = vec2(gravity_x, gravity_y); // directional gravity
    // gravity to target
    vec2 target = vec2(gravity_tx, gravity_ty);
    vec2 delta_pos = ball.pos - target;
    float dist = length(delta_pos);
    if(dist < 1e-6) return gravity;
    // gravity to target distance
    dist = abs(dist - gravity_tr);
    if(dist < 1e-6) return gravity;
    vec2 direction = normalize(delta_pos);
    target = target + direction * gravity_tr;
    delta_pos = ball.pos - target;
    dist = length(delta_pos);
    if(dist < 1e-6) return gravity;
    direction = normalize(delta_pos);
    float gravity_t_strength = abs(gravity_t) / dist + gravity_t2 / (dist * dist);
    gravity_t_strength = gravity_td * min(gravity_t_strength, gravity_tm);
    
    gravity += direction * gravity_t_strength;
    return gravity;
}

// https://github.com/Jam3/glsl-hsl2rgb/blob/master/index.glsl
float hue2rgb(float f1, float f2, float hue) {
    if (hue < 0.0)
        hue += 1.0;
    else if (hue > 1.0)
        hue -= 1.0;
    float res;
    if ((6.0 * hue) < 1.0)
        res = f1 + (f2 - f1) * 6.0 * hue;
    else if ((2.0 * hue) < 1.0)
        res = f2;
    else if ((3.0 * hue) < 2.0)
        res = f1 + (f2 - f1) * ((2.0 / 3.0) - hue) * 6.0;
    else
        res = f1;
    return res;
}
vec3 hsl2rgb(vec3 hsl) {
    vec3 rgb;
    
    if (hsl.y == 0.0) {
        rgb = vec3(hsl.z); // Luminance
    } else {
        float f2;
        
        if (hsl.z < 0.5)
            f2 = hsl.z * (1.0 + hsl.y);
        else
            f2 = hsl.z + hsl.y - hsl.y * hsl.z;
            
        float f1 = 2.0 * hsl.z - f2;
        
        rgb.r = hue2rgb(f1, f2, hsl.x + (1.0/3.0));
        rgb.g = hue2rgb(f1, f2, hsl.x);
        rgb.b = hue2rgb(f1, f2, hsl.x - (1.0/3.0));
    }   
    return rgb;
}
vec3 hsl2rgb(float h, float s, float l) {
    return hsl2rgb(vec3(h, s, l));
}

float arctan2(vec2 dir)
{
    // http://glslsandbox.com/e#26666.0      https://stackoverflow.com/a/27228836/798588
    return dir.x == 0 ? sign(dir.y)*M_PI/2 : (abs(dir.x) > abs(dir.y) ? M_PI/2.0 - atan(dir.x,dir.y) : atan(dir.y,dir.x));
}

void main() {
    uint workgroup_idx = 
        gl_WorkGroupID.z * gl_NumWorkGroups.x * gl_NumWorkGroups.y +
        gl_WorkGroupID.y * gl_NumWorkGroups.x +
        gl_WorkGroupID.x;
    uint global_idx = gl_LocalInvocationIndex + workgroup_idx * GROUPSIZE;
    if (global_idx >= num_items) return;
    Ball ball = balls[global_idx];

    // ball.vel = ball.vel + ball.impulse;

    ball.vel = ball.vel + ball.impulse + compute_gravity(ball)*dt; 
    ball.vel = emwa(gain_velocity_dampening, ref_dt, dt, ball.vel, vec2(0,0));
    float speed = length(ball.vel);
    if (speed > max_speed)
    {
        ball.vel = normalize(ball.vel) * max_speed;
    }
    ball.impulse = vec2(0,0);
    ball.pos = ball.pos + ball.vel * dt;

    
    bool x_in_bounds = (0 <= ball.pos.x) && (ball.pos.x <= width-1);
    bool y_in_bounds = (0 <= ball.pos.y) && (ball.pos.y <= height-1);
    if (!x_in_bounds || !y_in_bounds)
    {
        ball.pos = vec2(
            clamp(ball.pos.x, 0, width-1), 
            clamp(ball.pos.y, 0, height-1)
        );
        ball.vel = ball.vel * vec2(
            (x_in_bounds ? 1 : -1), 
            (y_in_bounds ? 1 : -1)
        );
    }

    uvec2 grid_pos = to_grid_coord(ball.pos);
    uint grid_idx = grid_pos.x + grid_pos.y * width;

    ll_next[global_idx] = atomicExchange(ll_head[grid_idx], int(global_idx));
    atomicAdd(ll_count[grid_idx], 1);

    out_balls[global_idx] = ball;
    out_points[global_idx] = vec4(ball.pos.x, ball.pos.y, ball.radius, 1);

    float normalized_speed = min(1, speed / max_speed);
    float angle = (speed > 1e-6) ? arctan2(ball.vel) : 0;
    // angle = angle * (normalized_speed*normalized_speed) + (1-normalized_speed*normalized_speed) * (2*M_PI*normalized_speed);
    vec3 color = hsl2rgb(
        angle / (2*M_PI),
        // 1, 
        // 0.5
        0.5+0.5*normalized_speed,
        0.5*normalized_speed
    );
    out_colors[global_idx] = vec4(color, 1);
}
)"
        );
    }    
} // namespace balls

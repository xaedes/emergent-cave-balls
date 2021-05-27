#include "ants/ants_program.h"
#include "ants/ant.h"

#include "geometry/pi.h"

#include "im_param/backends/glsl_struct_generator_backend.h"

namespace ants {

    void AntsProgram::setup(int width, int height)
    {
        this->width = width;
        this->height = height;
        
        Ant ant;
        im_param::GlslStructGeneratorBackend glslStructGenerator;
        im_param::parameter(glslStructGenerator, "Ant", ant, im_param::TypeHolder<Ant>());

        m_shaders = {Shader(Shader::ShaderType::Compute, code())};
        m_shaders[0].setup({{"##ANT_STRUCT##", glslStructGenerator.glsl_string(1)}});
        Program::setup();
        num_items                     .init(getGlProgram(), "num_items"                     );
        dt                            .init(getGlProgram(), "dt"                            );
        home_x                        .init(getGlProgram(), "home_x"                        , width/2   );
        home_y                        .init(getGlProgram(), "home_y"                        , height/2  );
        home_radius                   .init(getGlProgram(), "home_radius"                   , 16        );
        pickup_strength               .init(getGlProgram(), "pickup_strength"               , 1         );
        dropdown_strength             .init(getGlProgram(), "dropdown_strength"             , 255       );
        num_sensors                   .init(getGlProgram(), "num_sensors"                   , 3         );
        sensors_fov                   .init(getGlProgram(), "sensors_fov"                   , 45*PI/180 );
        attraction_gain               .init(getGlProgram(), "attraction_gain"               , 0.4       );
        max_carry                     .init(getGlProgram(), "max_carry"                     , 1         );
        resilience                    .init(getGlProgram(), "resilience"                    , 0.1       );
        home_comfort                  .init(getGlProgram(), "home_comfort"                  , 16        );
        food_spread                   .init(getGlProgram(), "food_spread"                   , 16        );
        winner_lead_bonus             .init(getGlProgram(), "winner_lead_bonus"             , 2         );
        min_winner_lead               .init(getGlProgram(), "min_winner_lead"               , 0         );
        focus_distance_min            .init(getGlProgram(), "focus_distance_min"            , 0.5       );
        focus_distance_max            .init(getGlProgram(), "focus_distance_max"            , 16        );
        random_focus_distance         .init(getGlProgram(), "random_focus_distance"         , 5*PI/180  );
        random_focus_distance_gain    .init(getGlProgram(), "random_focus_distance_gain"    , 0.5       );
        random_spread                 .init(getGlProgram(), "random_spread"                 , 45*PI/180 );
        random_spread_gain            .init(getGlProgram(), "random_spread_gain"            , 0.4       );
        random_strength               .init(getGlProgram(), "random_strength"               , 5*PI/180  );
        random_strength_gain          .init(getGlProgram(), "random_strength_gain"          , 0.9       );
        reference_dt                  .init(getGlProgram(), "reference_dt"                  , 0.1       );
        drop_chance                   .init(getGlProgram(), "drop_chance"                   , 0.1       );
        drop_amount                   .init(getGlProgram(), "drop_amount"                   , 1         );
        min_maxage                    .init(getGlProgram(), "min_maxage"                    , 200       );
        max_maxage                    .init(getGlProgram(), "max_maxage"                    , 400       );
        maxage_bonus_finding_food     .init(getGlProgram(), "maxage_bonus_finding_food"     , 200       );
        maxage_bonus_returning        .init(getGlProgram(), "maxage_bonus_returning"        , 200       );
        random_head                   .init(getGlProgram(), "random_head"                   , 5*PI/180  );
        random_head_gain              .init(getGlProgram(), "random_head_gain"              , 0.9       );
        random_grip                   .init(getGlProgram(), "random_grip"                   , 5*PI/180  );
        random_grip_gain              .init(getGlProgram(), "random_grip_gain"              , 0.9       );
        head_spread                   .init(getGlProgram(), "head_spread"                   , 90*PI/180 );
        checkGLError();
    }

    void AntsProgram::dispatch(int num_items, float dt)
    {
        this->num_items.set(num_items);
        this->dt.set(dt);
        ComputeProgram::dispatch(num_items, 1, 1, 1024, 1 ,1);
    }

    std::string AntsProgram::code() const
    {
        return (
R"(
#version 440
#define GROUPSIZE_X 1024
#define GROUPSIZE_Y 1
#define GROUPSIZE_Z 1
#define GROUPSIZE (GROUPSIZE_X*GROUPSIZE_Y*GROUPSIZE_Z)
layout(local_size_x=GROUPSIZE_X, local_size_y=GROUPSIZE_Y, local_size_z=GROUPSIZE_Z) in;

layout (binding = 0, rgba32f) uniform readonly  image2D srcEnv;
layout (binding = 1, rgba32f) uniform readonly  image2D srcDisplay;

// layout (binding = 1, rgba32f) uniform image2D destTex;

#define M_PI 3.1415926535897932384626433832795
#define D2R (M_PI/180)

##ANT_STRUCT##

layout (std430, binding = 0) buffer buf_ants
{
    Ant ants[];
};

layout (std430, binding = 1) buffer buf_out_ants
{
    Ant out_ants[];
};

uniform uint num_items;
uniform float dt;
uniform float home_x;
uniform float home_y;
uniform float home_radius;
uniform float pickup_strength;
uniform float dropdown_strength;

uniform uint num_sensors;
uniform float sensors_fov;
uniform float attraction_gain;

uniform float max_carry;
uniform float resilience;
uniform float home_comfort;
uniform float food_spread;
uniform float winner_lead_bonus;
uniform float min_winner_lead;
uniform float focus_distance_min;
uniform float focus_distance_max;
uniform float random_focus_distance;
uniform float random_focus_distance_gain;

uniform float random_spread;
uniform float random_spread_gain;

uniform float random_strength;
uniform float random_strength_gain;
uniform float reference_dt;

uniform float drop_chance;
uniform float drop_amount;

uniform float min_maxage;
uniform float max_maxage;
uniform float maxage_bonus_finding_food;
uniform float maxage_bonus_returning;

uniform float random_head;
uniform float random_head_gain;
uniform float random_grip;
uniform float random_grip_gain;
uniform float head_spread;

#define STATE_NONE    0
#define STATE_SEARCH  1
#define STATE_COLLECT 2
#define STATE_RETURN  3


uint hash(uint val)
{
    // https://youtu.be/X-iSQQgOd1A?t=701
    // www.cs.ubc.ca/~rbridson/docs/schechter-sca08-turbulence.pdf
    val ^= 2747636419u;
    val *= 2654435769u;
    val ^= val >> 16;
    val *= 2654435769u;
    val ^= val >> 16;
    val *= 2654435769u;
    // max_val = 4294967295
    return val;
}

float hash_to_float(uint hash)
{
    return float(hash) / 4294967295.0f;
}

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

void main() {
    ivec2 imsize = imageSize(srcEnv);
    uint workgroup_idx = 
        gl_WorkGroupID.z * gl_NumWorkGroups.x * gl_NumWorkGroups.y +
        gl_WorkGroupID.y * gl_NumWorkGroups.x +
        gl_WorkGroupID.x;
    uint global_idx = gl_LocalInvocationIndex + workgroup_idx * GROUPSIZE;
    if (global_idx >= num_items) return;
    Ant ant = ants[global_idx];


    float movestep = dt * ant.pos_vel.w;
    float dx = cos(ant.pos_vel.z) * movestep;
    float dy = sin(ant.pos_vel.z) * movestep;
    // float dx = 1;
    // float dy = 1;
    ant.pos_vel = ant.pos_vel + vec4(dx, dy, 0, 0);
    // ivec2 loadPos = ivec2(int(ant.pos_vel.x), int(ant.pos_vel.y));
    // vec4 current = imageLoad(srcEnv, loadPos);
    
    if (!((0 <= ant.pos_vel.x) && (ant.pos_vel.x < imsize.x) && (0 <= ant.pos_vel.y) && (ant.pos_vel.y < imsize.y)))
    {
        ant.pos_vel = vec4(
            clamp(ant.pos_vel.x, 0, imsize.x-1), 
            clamp(ant.pos_vel.y, 0, imsize.y-1), 
            mod(ant.pos_vel.z + 180*D2R, M_PI*2), 
            ant.pos_vel.w 
        );
        // ant.pos_vel = vec4(imsize.x/2,imsize.y/2,ant.pos_vel.z, ant.pos_vel.w);
        //return;
    }

    // ant.carry_head_focus_despair.y = 0;
    // ant.carry_head_focus_despair.z = focus_distance;
    ant.random.x = hash(ant.random.x);
    ant.random.y = hash(ant.random.y);
    ant.random.z = hash(ant.random.z);
    ant.random.w = hash(ant.random.w);

    float rnd_value = (hash_to_float(ant.random.x) - 0.5) * random_spread;
    float random_spread_strength = (cos(ant.homedist_fooddist_rndstr.z)+1)*0.5;

    ant.pos_vel.z = emwa(
        random_spread_gain * random_spread_strength, reference_dt, dt,
        ant.pos_vel.z, ant.pos_vel.z+rnd_value
    );

    rnd_value = hash_to_float(ant.random.y);
    // rnd_value = rnd_value*rnd_value;
    rnd_value = (rnd_value-0.5)*random_strength;

    ant.homedist_fooddist_rndstr.z = emwa(
        random_strength_gain, reference_dt, dt,
        ant.homedist_fooddist_rndstr.z, ant.homedist_fooddist_rndstr.z+rnd_value
    );
    ant.homedist_fooddist_rndstr.z = mod(ant.homedist_fooddist_rndstr.z, 2*M_PI);

    rnd_value = hash_to_float(ant.random.z);
    // rnd_value = rnd_value;
    rnd_value = (rnd_value-0.5)*random_focus_distance;

    ant.carry_head_focus_despair.z = emwa(
        random_focus_distance_gain, reference_dt, dt,
        ant.carry_head_focus_despair.z, ant.carry_head_focus_despair.z+rnd_value
    );
    ant.carry_head_focus_despair.z = mod(ant.carry_head_focus_despair.z, 2*M_PI);
    float focus_distance = focus_distance_min+(focus_distance_max-focus_distance_min)*(cos(ant.carry_head_focus_despair.z)+1)*0.5;
    // float focus_distance = focus_distance_max;

    rnd_value = hash_to_float(ant.random.w);
    // rnd_value = rnd_value;
    rnd_value = (rnd_value-0.5)*random_head;

    ant.carry_head_focus_despair.y = emwa(
        random_head_gain, reference_dt, dt,
        ant.carry_head_focus_despair.y, ant.carry_head_focus_despair.y+rnd_value
    );
    ant.carry_head_focus_despair.y = mod(ant.carry_head_focus_despair.y, 2*M_PI);
    float head = head_spread*(sin(ant.carry_head_focus_despair.y))*0.5;
    // float focus_distance = focus_distance_max;

    float start_direction = ant.pos_vel.z + head - sensors_fov / 2;
    float direction_interval = sensors_fov / (num_sensors-1);
    float best_pheromone = 0;
    float mean_pheromone = 0;
    float best_direction = ant.pos_vel.z + head;
    int best_sensor = 0;
    float abundance = 0;
    for(int i=0; i<num_sensors; ++i)
    {
        float sensor_direction = start_direction + direction_interval * i;
        float sensor_x = ant.pos_vel.x + cos(sensor_direction) * focus_distance;
        float sensor_y = ant.pos_vel.y + sin(sensor_direction) * focus_distance;
        ivec2 sensor_xy = ivec2(int(sensor_x), int(sensor_y));
        vec4 sensor_reading = imageLoad(srcEnv, sensor_xy);
        abundance += sensor_reading.z;
        float pheromone = (ant.state.x == STATE_RETURN) ? sensor_reading.g : ((sensor_reading.b > 0) ? 1 : sensor_reading.r);
        mean_pheromone += pheromone / num_sensors;
        if (pheromone > best_pheromone)
        {
            best_sensor = i;
            best_pheromone = pheromone;
            best_direction = sensor_direction;
        }
    }
    float winner_lead = clamp(winner_lead_bonus*(best_pheromone - mean_pheromone), 0, 1);
    float modified_attraction_gain = (winner_lead < min_winner_lead) ? 0 : clamp(attraction_gain*winner_lead, 0, 1);
    // float _attraction_gain = gain_for_dt(modified_attraction_gain, reference_dt, dt);
    // ant.pos_vel.z = best_direction * _attraction_gain + (1-_attraction_gain) * ant.pos_vel.z;
    ant.pos_vel.z = emwa(
        modified_attraction_gain, reference_dt, dt,
        ant.pos_vel.z, best_direction
    );
    ant.pos_vel.z = mod(ant.pos_vel.z, 2*M_PI);



    // ivec2 storePos = ivec2(global_idx % 256, (global_idx-global_idx % 256) / 256);
    ivec2 storePos = ivec2(int(ant.pos_vel.x), int(ant.pos_vel.y));
    vec4 environment = imageLoad(srcEnv, storePos);
    vec4 display = imageLoad(srcDisplay, storePos);
    vec4 environment_old = environment;
    vec4 display_old = display;
    abundance += environment.z;
    int last_state = ant.state.x;
    if(ant.state.x == STATE_NONE)
    {
        ant.state.x = STATE_SEARCH;
    }
    if(ant.state.x == STATE_SEARCH)
    {
        ant.homedist_fooddist_rndstr.x += movestep;
        if (environment.z > 0)
        {
            ant.state.x = STATE_COLLECT;
        }
    }
    if(ant.state.x == STATE_COLLECT)
    {
        if (environment.z > 0)
        {
            ant.maxage_age_grip.x = ant.maxage_age_grip.x + maxage_bonus_finding_food;
            
            // pickup
            float amount = (dt * pickup_strength > environment.z) ? environment.z : dt * pickup_strength;
            if (ant.carry_head_focus_despair.x + amount > max_carry)
            {
                amount = max_carry - ant.carry_head_focus_despair.x;
            }
            ant.carry_head_focus_despair.x += amount;
            ant.carry_head_focus_despair.w = 0;
            environment.z -= amount;
            abundance -= amount;
            ant.maxage_age_grip.z = 2*M_PI;
        }
        else
        {
            ant.carry_head_focus_despair.w += dt;
        }
        if ((ant.carry_head_focus_despair.x >= max_carry) || (ant.carry_head_focus_despair.w >= resilience))
        {
            ant.state.x = STATE_RETURN;
        }
    }
    if(ant.state.x == STATE_RETURN)
    {
        if (last_state == STATE_COLLECT)
        {
            // look behind 
            ant.pos_vel.z = mod(ant.pos_vel.z + M_PI, 2*M_PI);
            // ant.carry_head_focus_despair.y = M_PI;
        }
        // deposit at home
        float dhx = ant.pos_vel.x - home_x;
        float dhy = ant.pos_vel.y - home_y;
        float hr2 = dhx*dhx + dhy*dhy;
        if (hr2 <= home_radius*home_radius)
        {
            ant.maxage_age_grip.x = ant.maxage_age_grip.x + maxage_bonus_returning;

            // float amount = (dt * dropdown_strength > ant.carry_head_focus_despair.x) ? ant.carry_head_focus_despair.x : dt * dropdown_strength;
            // ant.carry_head_focus_despair.x -= amount;
            ant.carry_head_focus_despair.x = 0;
            ant.homedist_fooddist_rndstr.x = 0;
            ant.state.x = STATE_SEARCH;

            // turn around 
            ant.pos_vel.z = mod(ant.pos_vel.z + M_PI, 2*M_PI);
        }
    }
    ant.homedist_fooddist_rndstr.y = (ant.state.x == STATE_COLLECT) ? 0 : (ant.homedist_fooddist_rndstr.y + movestep);
    // ant.homedist_fooddist_rndstr.y = (abundance > 0) ? 0 : (ant.homedist_fooddist_rndstr.y + movestep);

    if (ant.carry_head_focus_despair.x > 0)
    {
        // carrying food
        ant.random.x = hash(ant.random.x);
        rnd_value = hash_to_float(ant.random.x);
        rnd_value = (rnd_value-0.5)*random_grip;
        ant.maxage_age_grip.z = emwa(
            random_grip_gain, reference_dt, dt,
            ant.maxage_age_grip.z, ant.maxage_age_grip.z+rnd_value
        );
        ant.maxage_age_grip.z = mod(ant.maxage_age_grip.z, 2*M_PI);
        float grip = (cos(ant.maxage_age_grip.z)+1)*0.5;
        if ((grip < drop_chance) && (environment.b == 0))
        {
            float amt = (drop_amount < ant.carry_head_focus_despair.x) ? drop_amount : ant.carry_head_focus_despair.x;
            ant.carry_head_focus_despair.x -= amt;
            environment.b += amt;
        }
    }

    if (ant.carry_head_focus_despair.x > 0)
    // if ((ant.carry_head_focus_despair.x > 0) && (ant.state.x == STATE_COLLECT))
    // if (ant.state.x == STATE_COLLECT)
    {
        // carrying food
        ant.random.x = hash(ant.random.x);
        rnd_value = hash_to_float(ant.random.x);
        rnd_value = (rnd_value-0.5)*random_grip;
        ant.maxage_age_grip.z = emwa(
            random_grip_gain, reference_dt, dt,
            ant.maxage_age_grip.z, ant.maxage_age_grip.z+rnd_value
        );
        ant.maxage_age_grip.z = mod(ant.maxage_age_grip.z, 2*M_PI);
        float grip = (cos(ant.maxage_age_grip.z)+1)*0.5;
        if ((grip < drop_chance) && (environment.b == 0))
        {
            
            float amt = (drop_amount < ant.carry_head_focus_despair.x) ? drop_amount : ant.carry_head_focus_despair.x;
            ant.carry_head_focus_despair.x -= amt;
            environment.b += amt;
        }
        float scaled_dist = (ant.homedist_fooddist_rndstr.y / (food_spread)) + 1;
        float food_pheromone = 2*scaled_dist/(1+scaled_dist*scaled_dist);
        // float food_pheromone = clamp(
            // (ant.carry_head_focus_despair.x - ant.homedist_fooddist_rndstr.y) / (food_spread), 
            // 0, 1
        // );
        if (food_pheromone > environment.x)
            environment.x = food_pheromone;
        // environment.x = 1;
    }
    else
    {
        ant.state.x = STATE_SEARCH;
    }
    if (ant.state.x == STATE_SEARCH)
    {
        // search for food
        float scaled_dist = (ant.homedist_fooddist_rndstr.x / home_comfort) + 1;
        float home_pheromone = 2*scaled_dist/(1+scaled_dist*scaled_dist);
        if (home_pheromone > environment.y)
            environment.y = home_pheromone;
        ant.carry_head_focus_despair.y = 0;
    }


    ant.maxage_age_grip.y = ant.maxage_age_grip.y + dt;

    if ((ant.maxage_age_grip.y >= ant.maxage_age_grip.x) && (environment.b == 0))
    {
        environment.b += ant.carry_head_focus_despair.x;

        // reset ant on maxage
        ant.random.x = hash(ant.random.x);
        ant.random.y = hash(ant.random.y);
        //ant.random.z = hash(ant.random.z);
        //ant.random.w = hash(ant.random.w);
        ant.pos_vel.x = home_x;
        ant.pos_vel.y = home_y;
        ant.pos_vel.z = hash_to_float(ant.random.x)*2*M_PI;

        ant.carry_head_focus_despair = vec4(0,0,M_PI/2,0);
        ant.homedist_fooddist_rndstr = vec4(0,0,M_PI/2,0);
        ant.maxage_age_grip.x = hash_to_float(ant.random.y)*(max_maxage - min_maxage)+min_maxage;
        ant.maxage_age_grip.y = 0;
        ant.state = ivec4(0,0,0,0);
    }


    display = (ant.state.x == STATE_RETURN) ? vec4(1,0,0,1) : vec4(1,1,1,1);
    // texAnts.xyz = vec3(1,1,1);
    // imageStore(dstTexAnts, storePos, texAnts);
    
    // wip: store in linked list per storePos
    // imageStore(dstEnv, storePos, environment);

    ant.environment_delta = environment - environment_old;
    ant.display_value = display;


    out_ants[global_idx] = ant;

    // imageStore(destTex, ivec2(global_idx % 256, 0), vec4(1.0,1.0,1.0,1.0));
    // imageStore(destTex, ivec2(global_idx % 256, 1), vec4(1.0,1.0,1.0,1.0));
    // imageStore(destTex, ivec2(global_idx % 256, 2), vec4(1.0,1.0,1.0,1.0));
    // imageStore(destTex, ivec2(global_idx % 256, 3), vec4(1.0,1.0,1.0,1.0));
}
)"
        );
    }    
} // namespace ants

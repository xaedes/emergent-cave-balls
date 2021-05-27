#pragma once

#include <string>

#include "gl_classes/program.h"
#include "gl_classes/program_uniform.h"
#include "gl_classes/compute_program.h"
#include "gl_classes/shader.h"

#include "im_param/im_param.h"
#include "im_param/gl_classes/program_uniform_specializations.h"

namespace ants {

    class AntsProgram : public gl_classes::ComputeProgram
    {
    public:
        using Program = gl_classes::Program;
        using ComputeProgram = gl_classes::ComputeProgram;
        using Shader = gl_classes::Shader;
        template<class T> using ProgramUniform = gl_classes::ProgramUniform<T>;

        AntsProgram(){}
        ~AntsProgram(){}
        void setup(int width, int height);
        void dispatch(int num_items, float dt);
        std::string code() const;
        
        uint32_t width;
        uint32_t height;

        ProgramUniform<uint32_t> num_items;
        ProgramUniform<float>    dt;
        ProgramUniform<float>    home_x;
        ProgramUniform<float>    home_y;
        ProgramUniform<float>    home_radius;
        ProgramUniform<float>    pickup_strength;
        ProgramUniform<float>    dropdown_strength;
        ProgramUniform<uint32_t> num_sensors;
        ProgramUniform<float>    sensors_fov;
        ProgramUniform<float>    attraction_gain;
        ProgramUniform<float>    max_carry;
        ProgramUniform<float>    resilience;
        ProgramUniform<float>    home_comfort;
        ProgramUniform<float>    food_spread;
        ProgramUniform<float>    winner_lead_bonus;
        ProgramUniform<float>    min_winner_lead;
        ProgramUniform<float>    focus_distance_min;
        ProgramUniform<float>    focus_distance_max;
        ProgramUniform<float>    random_focus_distance;
        ProgramUniform<float>    random_focus_distance_gain;
        ProgramUniform<float>    random_spread;
        ProgramUniform<float>    random_spread_gain;
        ProgramUniform<float>    random_strength;
        ProgramUniform<float>    random_strength_gain;
        ProgramUniform<float>    reference_dt;
        ProgramUniform<float>    drop_chance;
        ProgramUniform<float>    drop_amount;
        ProgramUniform<float>    min_maxage;
        ProgramUniform<float>    max_maxage;
        ProgramUniform<float>    maxage_bonus_finding_food;
        ProgramUniform<float>    maxage_bonus_returning;
        ProgramUniform<float>    random_head;
        ProgramUniform<float>    random_head_gain;
        ProgramUniform<float>    random_grip;
        ProgramUniform<float>    random_grip_gain;
        ProgramUniform<float>    head_spread;
    };
}  // namespace ants


namespace im_param {

    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::ants::AntsProgram& program, 
        const TypeHolder<::ants::AntsProgram>&)
    {
        program.use();
        //parameter(backend, "dt"                         , program.dt                         , );
        parameter(backend, "num_items"                  , program.num_items                  , 0, 10000);
        parameter(backend, "home_x"                     , program.home_x                     , 0, program.width-1);
        parameter(backend, "home_y"                     , program.home_y                     , 0, program.height-1);
        parameter(backend, "home_radius"                , program.home_radius                , 0, sqrt(program.width*program.width+program.height*program.height), 1);
        parameter(backend, "pickup_strength"            , program.pickup_strength            , 0, 10            );
        parameter(backend, "dropdown_strength"          , program.dropdown_strength          , 0, 10            );
        parameter(backend, "num_sensors"                , program.num_sensors                , 1, 5             );
        parameter(backend, "sensors_fov"                , program.sensors_fov                , 0, 180,    PI/180);
        parameter(backend, "attraction_gain"            , program.attraction_gain            , 0, 1             );
        parameter(backend, "max_carry"                  , program.max_carry                  , 0, 10            );
        parameter(backend, "resilience"                 , program.resilience                 , 0, 10            );
        parameter(backend, "home_comfort"               , program.home_comfort               , 0, 64            );
        parameter(backend, "food_spread"                , program.food_spread                , 0, 64            );
        parameter(backend, "winner_lead_bonus"          , program.winner_lead_bonus          , 0, 4             );
        parameter(backend, "min_winner_lead"            , program.min_winner_lead            , 0, 1             );
        parameter(backend, "focus_distance_min"         , program.focus_distance_min         , 0, 64            );
        parameter(backend, "focus_distance_max"         , program.focus_distance_max         , 0, 64            );
        parameter(backend, "random_focus_distance"      , program.random_focus_distance      , 0, 45,     PI/180);
        parameter(backend, "random_focus_distance_gain" , program.random_focus_distance_gain , 0, 1             );
        parameter(backend, "random_spread"              , program.random_spread              , 0, 45,     PI/180);
        parameter(backend, "random_spread_gain"         , program.random_spread_gain         , 0, 1             );
        parameter(backend, "random_strength"            , program.random_strength            , 0, 45,     PI/180);
        parameter(backend, "random_strength_gain"       , program.random_strength_gain       , 0, 1             );
        parameter(backend, "reference_dt"               , program.reference_dt               , 0, 1             );
        parameter(backend, "drop_chance"                , program.drop_chance                , 0, 1             );
        parameter(backend, "drop_amount"                , program.drop_amount                , 0, 2             );
        parameter(backend, "min_maxage"                 , program.min_maxage                 , 0, 1000          );
        parameter(backend, "max_maxage"                 , program.max_maxage                 , 0, 1000          );
        parameter(backend, "maxage_bonus_finding_food"  , program.maxage_bonus_finding_food  , 0, 1000          );
        parameter(backend, "maxage_bonus_returning"     , program.maxage_bonus_returning     , 0, 1000          );
        parameter(backend, "random_head"                , program.random_head                , 0, 45,     PI/180);
        parameter(backend, "random_head_gain"           , program.random_head_gain           , 0, 1             );
        parameter(backend, "random_grip"                , program.random_grip                , 0, 45,     PI/180);
        parameter(backend, "random_grip_gain"           , program.random_grip_gain           , 0, 1             );
        parameter(backend, "head_spread"                , program.head_spread                , 0, 360,    PI/180);
        return backend;
    }
    
} // namespace im_param

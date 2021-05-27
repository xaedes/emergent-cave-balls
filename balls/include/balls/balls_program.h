#pragma once

#include <string>

#include "glm/glm.hpp"

#include "gl_classes/program.h"
#include "gl_classes/program_uniform.h"
#include "gl_classes/compute_program.h"
#include "gl_classes/shader.h"

#include "im_param/im_param.h"
#include "im_param/gl_classes/program_uniform_specializations.h"

namespace balls {

    class BallsProgram : public gl_classes::ComputeProgram
    {
    public:
        using Program = gl_classes::Program;
        using ComputeProgram = gl_classes::ComputeProgram;
        using Shader = gl_classes::Shader;
        template<class T> using ProgramUniform = gl_classes::ProgramUniform<T>;

        BallsProgram() : gl_classes::ComputeProgram() {}
        virtual ~BallsProgram(){}
        void setup(int32_t width, int32_t height);
        void dispatch(uint32_t num_items, float dt);
        std::string code() const;
        
        ProgramUniform<float>     dt;
        ProgramUniform<int32_t>   width;
        ProgramUniform<int32_t>   height;
        ProgramUniform<uint32_t>  num_items;
        ProgramUniform<float>     ref_dt;
        ProgramUniform<float>     gain_velocity_dampening;
        ProgramUniform<float>     gravity_x;
        ProgramUniform<float>     gravity_y;
        ProgramUniform<float>     gravity_t;
        ProgramUniform<float>     gravity_t2;
        ProgramUniform<float>     gravity_tx;
        ProgramUniform<float>     gravity_ty;
        ProgramUniform<float>     gravity_tr;
        ProgramUniform<float>     gravity_tm;
        ProgramUniform<float>     gravity_td;
        ProgramUniform<float>     max_speed;
        // std::string foo;
    };
}  // namespace balls


namespace im_param {

    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::balls::BallsProgram& program, 
        const TypeHolder<::balls::BallsProgram>&)
    {
        program.use();
        parameter(backend, "num_items", program.num_items, 0, 10000);
        parameter(backend, "ref_dt"                  , program.ref_dt                  , 0, 1);
        parameter(backend, "gain_velocity_dampening" , program.gain_velocity_dampening , 0, 1);
        parameter(backend, "gravity.x" , program.gravity_x , -10, +10);
        parameter(backend, "gravity.y" , program.gravity_y , -10, +10);
        parameter(backend, "gravity.t" , program.gravity_t , -10, +10);
        parameter(backend, "gravity.t2" , program.gravity_t2 , -10, +10);
        parameter(backend, "gravity.tx" , program.gravity_tx , -10, +10);
        parameter(backend, "gravity.ty" , program.gravity_ty , -10, +10);
        parameter(backend, "gravity.tr" , program.gravity_tr , -10, +10);
        parameter(backend, "gravity.tm" , program.gravity_tm , 0, +10);
        parameter(backend, "gravity.td" , program.gravity_td , -1, +1);
        parameter(backend, "max_speed" , program.max_speed , 0, +10);
        return backend;
    }
    
} // namespace im_param

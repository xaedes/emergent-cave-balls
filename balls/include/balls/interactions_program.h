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

    // collect balls into ll per cell
    // parallel for each ball:
    // iterate over interaction candidates from items in 3x3 cell neighborhood
    //   if interaction: 
    //     compute forces etc, update aggregates in ball
    class InteractionsProgram : public gl_classes::ComputeProgram
    {
    public:
        using Program = gl_classes::Program;
        using ComputeProgram = gl_classes::ComputeProgram;
        using Shader = gl_classes::Shader;
        template<class T> using ProgramUniform = gl_classes::ProgramUniform<T>;

        InteractionsProgram(){}
        ~InteractionsProgram(){}
        void setup(uint32_t width, uint32_t height);
        void dispatch(uint32_t num_items, float dt, bool is_first_invocation);
        std::string code() const;
        
        ProgramUniform<uint32_t>  num_items;
        ProgramUniform<uint32_t>  max_iterations;
        ProgramUniform<uint32_t>  width;
        ProgramUniform<uint32_t>  height;
        ProgramUniform<int32_t>   max_abs_dx;
        ProgramUniform<int32_t>   max_abs_dy;
        ProgramUniform<float>     force_strength;
        ProgramUniform<float>     dt;
        ProgramUniform<bool>      is_first_invocation;

    };
}  // namespace balls


namespace im_param {

    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::balls::InteractionsProgram& program, 
        const TypeHolder<::balls::InteractionsProgram>&)
    {
        program.use();
        parameter(backend, "max_iterations", program.max_iterations, 0, 1000);
        parameter(backend, "force_strength", program.force_strength, 0, 10);
        parameter(backend, "max_abs_dx", program.max_abs_dx, 0, 1);
        parameter(backend, "max_abs_dy", program.max_abs_dy, 0, 1);
        return backend;
    }
    
} // namespace im_param

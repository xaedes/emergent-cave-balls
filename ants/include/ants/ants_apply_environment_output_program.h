#pragma once

#include <string>

#include "gl_classes/program.h"
#include "gl_classes/program_uniform.h"
#include "gl_classes/compute_program.h"
#include "gl_classes/shader.h"

#include "im_param/im_param.h"
#include "im_param/gl_classes/program_uniform_specializations.h"

namespace ants {

    class AntsApplyEnvironmentOutputProgram : public gl_classes::ComputeProgram
    {
    public:
        using Program = gl_classes::Program;
        using ComputeProgram = gl_classes::ComputeProgram;
        using Shader = gl_classes::Shader;
        template<class T> using ProgramUniform = gl_classes::ProgramUniform<T>;

        AntsApplyEnvironmentOutputProgram(){}
        ~AntsApplyEnvironmentOutputProgram(){}
        void setup();
        void dispatch(int num_items);
        std::string code() const;
        ProgramUniform<uint32_t> num_items;
        ProgramUniform<uint32_t> width;
        ProgramUniform<uint32_t> height;
        ProgramUniform<uint32_t> max_traverse;
    };

}  // namespace ants


namespace im_param {

    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::ants::AntsApplyEnvironmentOutputProgram& program, 
        const TypeHolder<::ants::AntsApplyEnvironmentOutputProgram>&)
    {
        program.use();
        // parameter(backend, "num_items"    , program.num_items    , 0, 10000);
        // parameter(backend, "width"        , program.width        , 0, 2048);
        // parameter(backend, "height"       , program.height       , 0, 2048);
        parameter(backend, "max_traverse" , program.max_traverse , 0, 2048);
        return backend;
    }
    
} // namespace im_param

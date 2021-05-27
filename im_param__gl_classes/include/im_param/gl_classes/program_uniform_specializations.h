#pragma once

#include "gl_classes/program_uniform.h"

namespace im_param {

    template<
        typename backend_type,
        typename value_type,
        typename... Args
    >
    backend_type& parameter(
        backend_type& backend, 
        const std::string& name, 
        gl_classes::ProgramUniform<value_type>& program, 
        Args... args)
    {
        value_type value = program.get();
        parameter(backend, name, value, std::forward<Args>(args)...);
        program.set(value);
        return backend;
    }

} // namespace im_param

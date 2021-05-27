#pragma once
#include "glm/glm.hpp"

namespace im_param {

    template<
        typename backend_type,
        int Dim, class T, glm::qualifier P,
        std::enable_if_t<Dim <= 4, bool> = true
    >
    backend_type& parameter(
        backend_type& backend, 
        const std::string& name, 
        glm::vec<Dim,typename T, P>& value, 
        glm::vec<Dim,typename T, P> min = glm::vec<Dim, typename T, P>(static_cast<T>(0)),
        glm::vec<Dim,typename T, P> max = glm::vec<Dim, typename T, P>(static_cast<T>(1)))
    {
        parameter(backend, name, &value.x, Dim, &min.x, &max.x);
        return backend;
    }

} // namespace im_param

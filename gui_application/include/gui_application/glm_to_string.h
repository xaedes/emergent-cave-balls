#pragma once

#include <string>
#include <ostream>
#include <glm/glm.hpp>

namespace std 
{
    std::ostream& operator<<(std::ostream& os, const glm::vec3& vec);
    std::ostream& operator<<(std::ostream& os, const glm::vec4& vec);
    std::ostream& operator<<(std::ostream& os, const glm::mat3& vec);
    std::ostream& operator<<(std::ostream& os, const glm::mat4& vec);

    std::string to_string(const glm::vec3& vec);
    std::string to_string(const glm::vec4& vec);
    std::string to_string(const glm::mat3& vec);
    std::string to_string(const glm::mat4& vec);
}

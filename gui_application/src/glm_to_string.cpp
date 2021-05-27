#include "gui_application/glm_to_string.h"
#include <sstream>
namespace std 
{
    std::ostream& operator<<(std::ostream& os, const glm::vec3& vec)
    {
        return (os << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]");
    }

    std::ostream& operator<<(std::ostream& os, const glm::vec4& vec)
    {
        return (os << "[" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << "]");
    }

    std::ostream& operator<<(std::ostream& os, const glm::mat3& mat)
    {
        return (os << "["
            << mat[0][0] << ", " << mat[1][0] << ", " << mat[2][0] << "\n"
            << mat[0][1] << ", " << mat[1][1] << ", " << mat[2][1] << "\n"
            << mat[0][2] << ", " << mat[1][2] << ", " << mat[2][2] << "]");
    }

    std::ostream& operator<<(std::ostream& os, const glm::mat4& mat)
    {
        return (os << "["
            << mat[0][0] << ", " << mat[1][0] << ", " << mat[2][0] << ", " << mat[3][0] << "\n"
            << mat[0][1] << ", " << mat[1][1] << ", " << mat[2][1] << ", " << mat[3][1] << "\n"
            << mat[0][2] << ", " << mat[1][2] << ", " << mat[2][2] << ", " << mat[3][2] << "\n"
            << mat[0][3] << ", " << mat[1][3] << ", " << mat[2][3] << ", " << mat[3][3] << "]");
    }



    std::string to_string(const glm::vec3& vec)
    {
        stringstream ss;
        ss << vec;
        return ss.str();
    }

    std::string to_string(const glm::vec4& vec)
    {
        stringstream ss;
        ss << vec;
        return ss.str();
    }

    std::string to_string(const glm::mat3& mat)
    {
        stringstream ss;
        ss << mat;
        return ss.str();
    }

    std::string to_string(const glm::mat4& mat)
    {
        stringstream ss;
        ss << mat;
        return ss.str();
    }

}

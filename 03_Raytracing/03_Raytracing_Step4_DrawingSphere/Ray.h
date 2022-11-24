#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace hlab
{
    class Ray
    {
    public:
        glm::vec3 start; // start position of the ray
        glm::vec3 dir;   // direction of the ray
    };
}

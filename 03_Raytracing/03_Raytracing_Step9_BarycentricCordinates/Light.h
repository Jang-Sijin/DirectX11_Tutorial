#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace hlab
{
    class Light
    {
    public:
        glm::vec3 pos; // 아주 단순화된 형태의 위치만 있는 점 조명
    };
}
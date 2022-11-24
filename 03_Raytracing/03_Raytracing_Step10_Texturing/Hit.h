#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace hlab
{
    using namespace glm;

    class Object;

    class Hit
    {
    public:
        float d;     // 광선의 시작부터 충돌 지점까지의 거리
        vec3 point;  // 충돌한 위치
        vec3 normal; // 충돌한 위치에서 표면의 수직 벡터
        vec2 uv;     // 텍스춰 좌표

        std::shared_ptr<Object> obj; // 나중에 물체의 재질 등을 가져오기 위한 포인터
    };
}
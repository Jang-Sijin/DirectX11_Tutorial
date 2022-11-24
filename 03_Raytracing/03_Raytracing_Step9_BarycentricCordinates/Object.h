#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Hit.h"
#include "Ray.h"

namespace hlab
{
    using namespace glm;

    class Object
    {
    public:
        // Material
        vec3 amb = vec3(0.0f);  // Ambient
        vec3 dif = vec3(0.0f);  // Diffuse
        vec3 spec = vec3(0.0f); // Specular
        float alpha = 10.0f;
        // float ks = 1.0f; // 변수를 줄이기 위해 항상 1.0으로 가정
        // float reflection_ = 0.0;
        // float transparency = 0.0;

        //std::shared_ptr<Texture> ambTexture;
        //std::shared_ptr<Texture> difTexture;

        Object(const vec3 &color = {1.0f, 1.0f, 1.0f})
            : amb(color), dif(color), spec(color)
        {
        }

        virtual Hit CheckRayCollision(Ray &ray) = 0;
    };
}
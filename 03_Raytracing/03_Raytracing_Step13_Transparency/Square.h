#pragma once

#include "Triangle.h"

namespace hlab
{
    class Square : public Object
    {
    public:
        Triangle triangle1, triangle2;

        Square(vec3 v0, vec3 v1, vec3 v2, vec3 v3, vec2 uv0 = vec2(0.0f), vec2 uv1 = vec2(0.0f), vec2 uv2 = vec2(0.0f), vec2 uv3 = vec2(0.0f))
            : triangle1(v0, v1, v2), triangle2(v0, v2, v3)
        {
            triangle1.uv0 = uv0;
            triangle1.uv1 = uv1;
            triangle1.uv2 = uv2;

            triangle2.uv0 = uv0;
            triangle2.uv1 = uv2;
            triangle2.uv2 = uv3;
        }

        virtual Hit CheckRayCollision(Ray &ray)
        {
            auto hit1 = triangle1.CheckRayCollision(ray);
            auto hit2 = triangle2.CheckRayCollision(ray);

            if (hit1.d >= 0.0f && hit2.d >= 0.0f)
            {
                return hit1.d < hit2.d ? hit1 : hit2;
            }
            else if (hit1.d >= 0.0f)
            {
                return hit1;
            }
            else
            {
                return hit2;
            }
        }
    };
}
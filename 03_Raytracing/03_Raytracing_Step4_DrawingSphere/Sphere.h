#pragma once

#include "Hit.h"
#include "Ray.h"

namespace hlab
{
    using namespace glm;

    class Sphere
    {
    public:
        glm::vec3 center;
        float radius;
        glm::vec3 color; // 뒤에서 '재질(material)'로 확장

        Sphere(const glm::vec3 &center, const float radius, const glm::vec3 &color)
            : center(center), color(color), radius(radius)
        {
        }

        // Wikipedia Line–sphere intersection
        // https://en.wikipedia.org/wiki/Line-sphere_intersection
        Hit IntersectRayCollision(Ray &ray)
        {
            Hit hit = Hit{-1.0f, vec3(0.0f), vec3(0.0f)}; // d가 음수이면 충돌을 안한 것으로 가정

            // ray 구의

            /*
             * hit.d = ... // 광선의 시작점으로부터 충돌지점까지의 거리 (float)
             * hit.point = ... // 광선과 구가 충돌한 지점의 위치 (vec3)
             * hit.normal = .. // 충돌 지점에서 구의 단위 법선 벡터(unit normal vector)
             */

            // const float a = glm::dot(ray.dir, ray.dir); // dir이 unit vector라면 a는 1.0f라서 생략 가능

            // [내적]            
            // Vector1 = (x1, y1, z1), Vector2 = (x2, y2, z2)
            // Vector1·Vector2 = (x1 * x2) + (y1 * y2) + (z1 * z2) = |Vector1| |Vector2| cosΘ
            // cosΘ = (x1 * x2) + (y1 * y2) + (z1 * z2) / |Vector1| |Vector2|
            // cosΘ = (x1 * x2) + (y1 * y2) + (z1 * z2) / sqrt(x1^2 + y1^2 + z1^2) * sqrt(x2^2 + y2^2 + z2^2)

            const float b = 2.0f * glm::dot(ray.dir, ray.start - this->center);
            const float c = dot(ray.start - this->center, ray.start - this->center) - (this->radius * this->radius);
            const float nabla = b * b / 4.0f - c;

            if (nabla >= 0.0f)
            {
                const float d1 = -(b / 2.0f) + sqrt(nabla);
                const float d2 = -(b / 2.0f) - sqrt(nabla);

                // 광선과 닿은(hit) 물체 중 거리의 깊이가 짧은 접점의 길이를 distance에 대입 (최소 0회, 최대 2회 광선과 오브젝트가 충돌)
                hit.d = glm::min(d1, d2);

                hit.point = ray.start + ray.dir * hit.d;
                hit.normal = glm::normalize(hit.point - this->center);
            }

            return hit;
        }
    };
}

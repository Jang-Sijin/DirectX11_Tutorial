#pragma once
#include "Object.h"

namespace hlab
{
    using namespace glm;

    class Sphere : public Object
    {
    public:
        glm::vec3 center;
        float radius;

        Sphere(const glm::vec3 &center, const float radius, const glm::vec3 &color)
            : center(center), radius(radius), Object(color)
        {
        }

        // Wikipedia Line–sphere intersection
        // https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
        Hit CheckRayCollision(Ray &ray)
        {
            Hit hit = Hit{-1.0f, vec3(0.0f), vec3(0.0f)};

            // const double a = glm::dot(ray.dir_, ray.dir_); // dir이 unit vector라면 a는 1.0
            const float b = 2.0f * glm::dot(ray.dir, ray.start - this->center);
            const float c = dot(ray.start - this->center, ray.start - this->center) - this->radius * this->radius;

            const float det = b * b - 4.0f * c;
            if (det >= 0.0f)
            {
                const float d1 = (-b - sqrt(det)) / 2.0f;
                const float d2 = (-b + sqrt(det)) / 2.0f;
                hit.d = glm::min(d1, d2);
                hit.point = ray.start + ray.dir * hit.d;
                hit.normal = glm::normalize(hit.point - this->center);
            }

            return hit;
        }
    };
}

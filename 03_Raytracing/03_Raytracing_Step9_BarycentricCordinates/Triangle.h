#pragma once

#include "Object.h"

namespace hlab
{
    using namespace glm;

    class Triangle : public Object
    {
    public:
        vec3 v0, v1, v2;
        vec2 uv0, uv1, uv2; // 뒤에서 텍스춰 좌표계로 사용

    public:
        Triangle()
            : v0(vec3(0.0f)), v1(vec3(0.0f)), v2(vec3(0.0f)), uv0(vec2(0.0f)), uv1(vec2(0.0f)), uv2(vec2(0.0f))
        {
        }

        Triangle(vec3 v0, vec3 v1, vec3 v2, vec2 uv0 = vec2(0.0f), vec2 uv1 = vec2(0.0f), vec2 uv2 = vec2(0.0f))
            : v0(v0), v1(v1), v2(v2), uv0(uv0), uv1(uv1), uv2(uv2)
        {
        }

        virtual Hit CheckRayCollision(Ray &ray)
        {
            Hit hit = Hit{-1.0, vec3(0.0), vec3(0.0)};

            vec3 point, faceNormal;
            float t, w0, w1;
            if (IntersectRayTriangle(ray.start, ray.dir, this->v0, this->v1,
                                     this->v2, point, faceNormal, t, w0, w1))
            {
                hit.d = t;
                hit.point = point; // ray.start + ray.dir * t;
                hit.normal = faceNormal;

                // Barycentric coordinates 확인용
                hit.w = vec2(w0, w1);

                // 텍스춰링(texturing)에서 사용
                // hit.uv = uv0 * w0 + uv1 * w1 + uv2 * (1.0f - w0 - w1);
            }

            return hit;
        }

        // 참고: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
        bool IntersectRayTriangle(const vec3 &orig, const vec3 &dir,
                                  const vec3 &v0, const vec3 &v1,
                                  const vec3 &v2, vec3 &point, vec3 &faceNormal,
                                  float &t, float &w0, float &w1)
        {
            /*
             * 기본 전략
             * - 삼각형이 놓여있는 평면과 광선의 교점을 찾고,
             * - 그 교점이 삼각형 안에 있는지 밖에 있는지를 판단한다.
             */

            /* 1. 삼각형이 놓여 있는 평면의 수직 벡터 계산 */
            faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            //주의: 삼각형의 넓이가 0일 경우에는 계산할 수 없음

            // 삼각형 뒷면을 그리고 싶지 않은 경우 (Backface culling)
            if (dot(-dir, faceNormal) < 0.0f)
                return false;

            // 평면과 광선이 수평에 매우 가깝다면 충돌하지 못하는 것으로 판단
            if (glm::abs(dot(dir, faceNormal)) < 1e-2f)
                return false; // t 계산시 0으로 나누기 방지

            /* 2. 광선과 평면의 충돌 위치 계산 */
            t = (dot(v0, faceNormal) - dot(orig, faceNormal)) /
                (dot(dir, faceNormal));

            // 광선의 시작점 이전에 충돌한다면 렌더링할 필요 없음
            if (t < 0.0f)
                return false;

            point = orig + t * dir; // 충돌점

            /* 3. 그 충돌 위치가 삼각형 안에 들어 있는 지 확인 */

            // 작은 삼각형들 3개의 normal 계산
            // 방향만 확인하면 되기 때문에 normalize() 생략 가능
            const vec3 cross0 = glm::cross(point - v2, v1 - v2);
            const vec3 cross1 = glm::cross(point - v0, v2 - v0);
            const vec3 cross2 = glm::cross(v1 - v0, point - v0);

            if (dot(cross0, faceNormal) < 0.0f)
                return false;
            if (dot(cross1, faceNormal) < 0.0f)
                return false;
            if (dot(cross2, faceNormal) < 0.0f)
                return false;

            // Barycentric coordinates 계산
            // 텍스춰링(texturing)에서 사용
            // 아래에서 cross product의 절대값으로 작은 삼각형들의 넓이 계산

            // const float area0 = ...
            // const float area1 = ...
            // const float area2 = ...

            // const float areaSum = ...

            // 기호에 alpha, beta, gamma 또는 u, v, w 등을 사용하기도 함
            w0 = 0.0f; //임시 값
            w1 = 0.0f; //임시 값

            return true;
        }
    };
} // namespace hlab
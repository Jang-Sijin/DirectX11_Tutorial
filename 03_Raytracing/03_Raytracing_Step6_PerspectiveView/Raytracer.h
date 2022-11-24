#pragma once

#include "Sphere.h"
#include "Ray.h"
#include "Light.h"

#include <vector>

namespace hlab
{
	// 일반적으로는 헤더파일에서 using namespace std를 사용하지 않습니다.
	// 여기서는 강의 동영상에 녹화되는 코드 길이를 줄이기 위해서 사용하였습니다.
	// 예: std::vector -> vector
	using namespace std;
	using namespace glm;

	class Raytracer
	{
	public:
		int width, height;
		Light light;
		std::vector<shared_ptr<Object>> objects;

		Raytracer(const int &width, const int &height)
			: width(width), height(height)
		{
			// 스크린으로부터 거리가 다른 구 3개
			auto sphere1 = make_shared<Sphere>(vec3(0.5f, 0.0f, 0.5f), 0.4f, vec3(0.5f, 0.5f, 0.5f));
			auto sphere2 = make_shared<Sphere>(vec3(0.0f, 0.0f, 1.0f), 0.4f, vec3(0.5f, 0.5f, 0.5f));
			auto sphere3 = make_shared<Sphere>(vec3(-0.5f, 0.0f, 1.5f), 0.4f, vec3(0.5f, 0.5f, 0.5f));

			sphere1->amb = vec3(0.2f);
			sphere1->dif = vec3(1.0f, 0.2f, 0.2f);
			sphere1->spec = vec3(0.5f);
			sphere1->alpha = 10.0f;

			sphere2->amb = vec3(0.2f);
			sphere2->dif = vec3(0.2f, 1.0f, 0.2f);
			sphere2->spec = vec3(0.5f);
			sphere2->alpha = 10.0f;

			sphere3->amb = vec3(0.2f);
			sphere3->dif = vec3(0.2f, 0.2f, 1.0f);
			sphere3->spec = vec3(0.5f);
			sphere3->alpha = 10.0f;

			// 일부러 역순으로 추가
			objects.push_back(sphere3);
			objects.push_back(sphere2);
			objects.push_back(sphere1);

			light = Light{{0.0f, 1.0f, -1.0f}}; // 화면 뒷쪽
		}

		Hit FindClosestCollision(Ray& ray)
		{
			Hit closest_hit = Hit{ -1.0, dvec3(0.0), dvec3(0.0) };

			for (int l = 0; l < objects.size(); l++)
			{
				auto hit = objects[l]->CheckRayCollision(ray);

				if (hit.d >= 0.0f)
				{
					hit.obj = objects[l];
					return hit;
				}
			}

			return closest_hit;
		}

		// 광선이 물체에 닿으면 그 물체의 색 반환
		vec3 traceRay(Ray &ray)
		{
			// Render first hit
			const auto hit = FindClosestCollision(ray);

			if (hit.d >= 0.0f)
			{
				// Diffuse
				const vec3 dirToLight = glm::normalize(light.pos - hit.point);
				const float diff = glm::max(dot(hit.normal, dirToLight), 0.0f);

				// Specular
				const vec3 reflectDir = 2.0f * dot(hit.normal, dirToLight) * hit.normal - dirToLight;
				const float specular = glm::pow(glm::max(glm::dot(-ray.dir, reflectDir), 0.0f), hit.obj->alpha);

				return hit.obj->amb + hit.obj->dif * diff + hit.obj->spec * specular;
			}

			return vec3(0.0f);
		}

		void Render(std::vector<glm::vec4>& pixels)
		{
			std::fill(pixels.begin(), pixels.end(), vec4{ 0.0f, 0.0f, 0.0f, 1.0f });

			const vec3 eyePos(0.0f, 0.0f, -1.5f);

#pragma omp parallel for
			for (int j = 0; j < height; j++)
				for (int i = 0; i < width; i++)
				{
					const vec3 pixelPosWorld = TransformScreenToWorld(vec2(i, j));

					// 광선의 방향 벡터
					// 스크린에 수직인 z방향, 절대값 1.0인 유닉 벡터
					// Orthographic projection (정투영) vs perspective projection (원근투영)

					const auto rayDir = vec3(0.0f, 0.0f, 1.0f);
					Ray pixelRay{ pixelPosWorld, rayDir };

					pixels[i + width * j] = vec4(glm::clamp(traceRay(pixelRay), 0.0f, 1.0f), 1.0f);
				}
		}

		vec3 TransformScreenToWorld(vec2 posScreen)
		{
			const float xScale = 2.0f / this->width;
			const float yScale = 2.0f / this->height;
			const float aspect = float(this->width) / this->height;

			// 3차원 공간으로 확장 (z좌표는 0.0)
			return vec3((posScreen.x * xScale - 1.0f) * aspect, -posScreen.y * yScale + 1.0f, 0.0f);
		}
	};
}
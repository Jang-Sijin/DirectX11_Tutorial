#pragma once

#include "Sphere.h"
#include "Ray.h"
#include "Light.h"
#include "Triangle.h"
#include "Square.h"

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
		vector<shared_ptr<Object>> objects;
		shared_ptr<Object> tempObject;

		Raytracer(const int &width, const int &height)
			: width(width), height(height)
		{
			auto sphere1 = make_shared<Sphere>(vec3(1.0f, 0.0f, 1.5f), 0.4f);

			sphere1->amb = vec3(0.2f);
			sphere1->dif = vec3(1.0f, 0.2f, 0.2f);
			sphere1->spec = vec3(0.5f);
			sphere1->alpha = 10.0f;

			objects.push_back(sphere1);

			auto triangle = make_shared<Triangle>(vec3(-2.0f, -2.0f, 2.0f), vec3(-2.0f, 2.0f, 2.0f), vec3(2.0f, 2.0f, 2.0f));
			triangle->amb = vec3(1.0f);
			triangle->dif = vec3(0.0f);
			triangle->spec = vec3(0.0f);

			tempObject = triangle; // 따로 처리하기 위해 임시로 저장

			objects.push_back(triangle);

			light = Light{{0.0f, 1.0f, 0.5f}}; // 화면 뒷쪽
		}

		Hit FindClosestCollision(Ray &ray)
		{
			float closestD = 1000.0; // inf
			Hit closestHit = Hit{-1.0, dvec3(0.0), dvec3(0.0)};

			for (int l = 0; l < objects.size(); l++)
			{
				auto hit = objects[l]->CheckRayCollision(ray);

				if (hit.d >= 0.0f)
				{
					if (hit.d < closestD)
					{
						closestD = hit.d;
						closestHit = hit;
						closestHit.obj = objects[l];

						// Barycentric coordinates 복사
						closestHit.w = hit.w;

						// 텍스춰 좌표
						// closestHit.uv = hit.uv;
					}
				}
			}

			return closestHit;
		}
		// 광선이 물체에 닿으면 그 물체의 색 반환
		vec3 traceRay(Ray &ray)
		{
			// Render first hit
			const auto hit = FindClosestCollision(ray);

			if (hit.d >= 0.0f)
			{
				glm::vec3 pointColor;

				// Ambient
				pointColor = hit.obj->amb;

				if (hit.obj == this->tempObject) // 임시로 삼각형만 색을 직접 결정
				{
					// Barycentric coordinates를 이용한 색 섞기(interpolation)
					const auto color0 = vec3(1.0f, 0.0f, 0.0f);
					const auto color1 = vec3(0.0f, 1.0f, 0.0f);
					const auto color2 = vec3(0.0f, 0.0f, 1.0f);

					const float w0 = hit.w[0]; // hit.w.x
					const float w1 = hit.w[1]; // hit.w.y
					const float w2 = 1.0f - w0 - w1;

					pointColor = color0 * w0 + color1 * w1 + color2 * w2;
				}

				const vec3 dirToLight = glm::normalize(light.pos - hit.point);

				// 그림자 꺼놓음
				// Ray shadowRay = { hit.point + dirToLight * 1e-4f, dirToLight };
				// if (FindClosestCollision(shadowRay).d < 0.0f)
				{
					const float diff = glm::max(dot(hit.normal, dirToLight), 0.0f);

					// Specular
					const vec3 reflectDir = dirToLight - hit.normal * 2.0f * dot(dirToLight, hit.normal);
					const float specular = glm::pow(glm::max(glm::dot(ray.dir, reflectDir), 0.0f), hit.obj->alpha);

					pointColor += diff * hit.obj->dif;
					pointColor += hit.obj->spec * specular;
				}

				return pointColor;
			}

			return vec3(0.0f);
		}

		void Render(std::vector<glm::vec4> &pixels)
		{
			std::fill(pixels.begin(), pixels.end(), vec4(0.0f, 0.0f, 0.0f, 1.0f));

			const vec3 eyePos(0.0f, 0.0f, -1.5f);

#pragma omp parallel for
			for (int j = 0; j < height; j++)
				for (int i = 0; i < width; i++)
				{
					const vec3 pixelPosWorld = TransformScreenToWorld(vec2(i, j));
					Ray pixelRay{pixelPosWorld, glm::normalize(pixelPosWorld - eyePos)};
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
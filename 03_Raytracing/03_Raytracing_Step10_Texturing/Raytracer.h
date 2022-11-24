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

		Raytracer(const int &width, const int &height)
			: width(width), height(height)
		{
			// sphere1은 강의 내용과는 무관합니다. 빼고 실습하셔도 됩니다.
			auto sphere1 = make_shared<Sphere>(vec3(1.0f, 0.0f, 1.5f), 0.4f);

			sphere1->amb = vec3(0.2f);
			sphere1->dif = vec3(1.0f, 0.2f, 0.2f);
			sphere1->spec = vec3(0.5f);
			sphere1->alpha = 10.0f;

			objects.push_back(sphere1);

			// 간단한 이미지
			std::vector<vec3> textureImage(4 * 4);
			for (int j = 0; j < 4; j++)
				for (int i = 0; i < 4; i++)
				{
					if (i % 4 == 0)
					{
						textureImage[i + 4 * j] = vec3(1.0f, 0.0f, 0.0f) * (1.0f + j) * 0.25f;
					}
					else if (i % 4 == 1)
					{
						textureImage[i + 4 * j] = vec3(0.0f, 1.0f, 0.0f) * (1.0f + j) * 0.25f;
					}
					else if (i % 4 == 2)
					{
						textureImage[i + 4 * j] = vec3(0.0f, 0.0f, 1.0f) * (1.0f + j) * 0.25f;
					}
					else
					{
						textureImage[i + 4 * j] = vec3(1.0f, 1.0f, 1.0f) * (1.0f + j) * 0.25f;
					}
				}

			auto imageTexture = std::make_shared<Texture>(4, 4, textureImage);
			// auto imageTexture = std::make_shared<Texture>("shadertoy_abstract1.jpg");

			auto square = make_shared<Square>(vec3(-2.0f, 2.0f, 2.0f), vec3(2.0f, 2.0f, 2.0f), vec3(2.0f, -2.0f, 2.0f), vec3(-2.0f, -2.0f, 2.0f),
											  vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f));

			square->amb = vec3(1.0f);
			square->dif = vec3(0.0f);
			square->spec = vec3(0.0f);

			square->ambTexture = imageTexture;
			// square->difTexture = imageTexture;

			objects.push_back(square);

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

						// 텍스춰 좌표
						closestHit.uv = hit.uv;
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
				if (hit.obj->ambTexture)
				{
					pointColor = hit.obj->amb * hit.obj->ambTexture->SamplePoint(hit.uv);
				}
				else
				{
					pointColor = hit.obj->amb;
				}

				// Diffuse
				const vec3 dirToLight = glm::normalize(light.pos - hit.point);

				const float diff = glm::max(dot(hit.normal, dirToLight), 0.0f);

				// Specular
				const vec3 reflectDir = dirToLight - hit.normal * 2.0f * dot(dirToLight, hit.normal);
				const float specular = glm::pow(glm::max(glm::dot(ray.dir, reflectDir), 0.0f), hit.obj->alpha);

				if (hit.obj->difTexture)
				{
					pointColor += diff * hit.obj->dif * hit.obj->difTexture->SampleLinear(hit.uv);
				}
				else
				{
					pointColor += diff * hit.obj->dif;
				}

				pointColor += hit.obj->spec * specular;

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
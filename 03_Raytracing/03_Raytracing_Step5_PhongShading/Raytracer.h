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
		shared_ptr<Sphere> sphere;

		Raytracer(const int &width, const int &height)
			: width(width), height(height)
		{
			sphere = make_shared<Sphere>(vec3(0.0f, 0.0f, 0.5f), 0.5f);
			sphere->amb = vec3(0.0f, 0.0f, 0.0f);
			sphere->diff = vec3(0.0f, 0.0f, 1.0f);
			sphere->spec = vec3(1.0f, 1.0f, 1.0f);
			sphere->alpha = 9.0f;
			sphere->ks = 0.8f;

			light = Light{{0.0f, 0.0f, -1.0f}}; // 화면 뒷쪽
		}

		// 광선이 물체에 닿으면 그 물체의 색 반환
		vec3 traceRay(Ray &ray)
		{
			auto hit = sphere->IntersectRayCollision(ray);

			if (hit.d < 0.0f)
			{
				return vec3(0.0f);
			}
			else
			{
				// return sphere->color * hit.d; // <- 앞에서 사용했던 색 결정

				// 여기서 퐁 모델(Phong reflection model)으로 조명 효과 계산!
				// 참고 자료
				// https://en.wikipedia.org/wiki/Phong_reflection_model
				// https://www.scratchapixel.com/lessons/3d-basic-rendering/phong-shader-BRDF

				// Diffuse
				// const vec3 dirToLight = ...
				const float diff = 1.0f;

				// Specular
				// const vec3 reflectDir = ... // r = 2 (n dot l) n - l
				const float specular = 1.0f;

				return sphere->amb + sphere->diff * diff + sphere->spec * specular * sphere->ks;
				// return sphere->diff * diff;
				// return sphere->spec * specular * sphere->ks;
			}
		}

		void Render(std::vector<glm::vec4> &pixels)
		{
			std::fill(pixels.begin(), pixels.end(), vec4{0.0f, 0.0f, 0.0f, 1.0f});

#pragma omp parallel for
			for (int j = 0; j < height; j++)
				for (int i = 0; i < width; i++)
				{
					const vec3 pixelPosWorld = TransformScreenToWorld(vec2(i, j));

					// 광선의 방향 벡터
					// 스크린에 수직인 z방향, 절대값 1.0인 유닉 벡터
					const auto rayDir = vec3(0.0f, 0.0f, 1.0f);

					Ray pixelRay{pixelPosWorld, rayDir};

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
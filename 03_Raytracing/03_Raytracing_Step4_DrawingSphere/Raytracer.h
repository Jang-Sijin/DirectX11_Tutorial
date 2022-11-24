#pragma once

#include "Sphere.h"
#include "Ray.h"

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
		shared_ptr<Sphere> sphere;

		Raytracer(const int &width, const int &height)
			: width(width), height(height)
		{
			sphere = make_shared<Sphere>(vec3(0.0f, 0.0f, 0.5f), 0.4f, vec3(1.0f, 1.0f, 1.0f));
		}

		glm::vec3 TransformScreenToWorld(glm::vec2 posScreen)
		{
			const float xScale = 2.0f / (this->width - 1);
			const float yScale = 2.0f / (this->height - 1);
			const float aspect = float(this->width) / this->height;

			// 3차원 공간으로 확장 (z좌표는 0.0으로 가정)
			return glm::vec3((posScreen.x * xScale - 1.0f) * aspect, -posScreen.y * yScale + 1.0f, 0.0f);
		}

		// 광선이 물체에 닿으면 그 물체의 색 반환
		vec3 traceRay(Ray &ray)
		{
			const Hit hit = sphere->IntersectRayCollision(ray);

			if (hit.d < 0.0f)
			{
				return vec3(0.0f);
			}
			else
			{
				return sphere->color * hit.d; // 깊이를 곱해서 입체감 만들기
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
					// Orthographic projection (정투영) vs perspective projection (원근투영)
					const auto rayDir = vec3(0.0f, 0.0f, 1.0f);

					Ray pixelRay{pixelPosWorld, rayDir};

					// index에는 size_t형 사용 (index가 음수일 수는 없으니까)
					// traceRay()의 반환형은 vec3 (RGB), A는 불필요
					pixels[size_t(i + width * j)] = vec4(traceRay(pixelRay), 1.0f);
				}
		}
	};
}
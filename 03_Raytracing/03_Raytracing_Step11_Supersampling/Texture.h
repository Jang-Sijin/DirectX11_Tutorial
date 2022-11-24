#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace hlab
{
	// Bilinear interpolation reference
	// https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/interpolation/bilinear-filtering

	using namespace glm;

	class Texture
	{
	public:
		int width, height, channels;
		std::vector<uint8_t> image;

		Texture(const std::string &filename);
		Texture(const int &width, const int &height, const std::vector<vec3> &pixels);

		vec3 GetWrapped(int i, int j)
		{
			i %= width; // i가 width면 0으로 바뀜
			j %= height;

			if (i < 0)
				i += width; // i가 -1이면 (width-1)로 바뀜
			if (j < 0)
				j += height;

			const float r = image[(i + width * j) * channels + 0] / 255.0f;
			const float g = image[(i + width * j) * channels + 1] / 255.0f;
			const float b = image[(i + width * j) * channels + 2] / 255.0f;

			return vec3(r, g, b);
		}

		vec3 InterpolateBilinear(
			const float &dx,
			const float &dy,
			const vec3 &c00,
			const vec3 &c10,
			const vec3 &c01,
			const vec3 &c11)
		{
			vec3 a = c00 * (1.0f - dx) + c10 * dx;
			vec3 b = c01 * (1.0f - dx) + c11 * dx;
			return a * (1.0f - dy) + b * dy;
		}

		vec3 SamplePoint(const vec2 &uv) // Nearest sampling이라고 부르기도 함
		{
			// 텍스춰 좌표의 범위 uv [0.0, 1.0] x [0.0, 1.0]
			// 이미지 좌표의 범위 xy [-0.5, width - 1 + 0.5] x [-0.5, height - 1 + 0.5]
			vec2 xy = uv * vec2(width, height) - vec2(0.5f);
			xy += vec2(0.5f);

			int i = int(xy.x);
			int j = int(xy.y);

			return GetWrapped(i, j);
		}

		vec3 SampleLinear(const vec2 &uv)
		{
			// 텍스춰 좌표의 범위 uv [0.0, 1.0] x [0.0, 1.0]
			// 이미지 좌표의 범위 xy [-0.5, width - 1 + 0.5] x [-0.5, height - 1 + 0.5]
			// std::cout << floor(-0.3f) << " " << int(-0.3f) << std::endl; // -1 0

			const vec2 xy = uv * vec2(width, height) - vec2(0.5f);
			const int i = int(floor(xy.x));
			const int j = int(floor(xy.y));
			const float dx = xy.x - float(i);
			const float dy = xy.y - float(j);

			return InterpolateBilinear(dx, dy, GetWrapped(i, j), GetWrapped(i + 1, j), GetWrapped(i, j + 1), GetWrapped(i + 1, j + 1));
		}
	};
}
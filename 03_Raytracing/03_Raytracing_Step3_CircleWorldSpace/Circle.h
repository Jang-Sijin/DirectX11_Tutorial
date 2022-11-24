#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace hlab
{
	class Circle
	{
	public:
		glm::vec2 center;
		float radius;
		glm::vec4 color;

		Circle(const glm::vec2& center, const float radius, const glm::vec4& color)
			: center(center), color(color), radius(radius)
		{}

		// x는 벡터이기 때문에 내부적으로 x좌표와 y좌표를 모두 갖고 있음
		// screen 좌표계에서는 x좌표와 y좌표가 int지만 float로 변환		
		bool IsInside(const glm::vec2& x)
		{
			if (glm::dot(x - center, x - center) - radius * radius > 0.0f)
				return false;
			else
				return true;
		}
	};
}

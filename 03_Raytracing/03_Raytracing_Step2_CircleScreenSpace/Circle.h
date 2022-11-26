#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

namespace hlab
{
	class Circle
	{
	public:
		glm::vec2 center;
		float radius;
		float radiusSquared;
		glm::vec4 color;

		Circle(const glm::vec2 &center, const float radius, const glm::vec4 &color)
			: center(center), color(color), radius(radius), radiusSquared{ radius * radius }
		{
		}

		// center는 벡터이기 때문에 내부적으로 x좌표와 y좌표를 모두 갖고 있음
		// screen 좌표계에서는 x좌표와 y좌표가 int지만 float로 변환
		bool IsInside(const glm::vec2 &getCenter)
		{
			// TODO: 여기에 원의 방정식을 이용해서 center가 원 안에 들어 있는지 아닌지에 따라
			//		true나 false 반환			
			// (x2-x1)^2 + (y2-y1)^2 <= r^2
			// const float distance = sqrt(pow(getCenter.x - center.x, 2) + pow(getCenter.y - center.y, 2));
			// const float distance = glm::length(getCenter - center);
			const float distanceSquared = (pow(getCenter.x - center.x, 2) + pow(getCenter.y - center.y, 2));

			if (distanceSquared <= radiusSquared)
				return true;
			else
				return false;
		}
	};
}

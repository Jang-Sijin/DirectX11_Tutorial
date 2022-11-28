#pragma once

#include "Triangle.h"

namespace hlab
{
	class Square : public Object
	{
	public:
		Square(vec3 v1, vec3 v2, vec3 v3, vec3 v4)
			: t1(v1, v2, v3), t2(v3, v4, v1)
		{
		}
	public:
		virtual Hit CheckRayCollision(Ray& ray)
		{
			auto hit1 = t1.CheckRayCollision(ray);
			auto hit2 = t2.CheckRayCollision(ray);

			if (0.0f <= hit1.d && 0.0f <= hit2.d)
			{
				return hit1.d < hit2.d ? hit2 : hit1;
			}
			else if (0.0f <= hit1.d)
			{
				return hit1;
			}
			else
			{
				return hit2;
			}
		}
	public:
		Triangle t1, t2; // 2 * Triangle = Square
	};
}
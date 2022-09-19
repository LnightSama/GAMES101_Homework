#pragma once
#include "geometry.h"
#include <math.h>
#include <vector>

class Camera {

public:

	vec3 position; //坐标系位置
	vec3 forward; //向前向量
	vec3 rightward; //向右向量
	vec3 upward; //向上向量
	int  width;          //屏幕宽度
	int  height;          //屏幕高度
	int  sight_distance;     //视距

	Camera(
		vec3 position,
		int  width,
		int  height,
		int  sight_distance
	) : position(position),
		width(width),
		height(height),
		sight_distance(sight_distance) {
		this->forward = (vec3(0, 0, 0) - position).normalize();
		int angle = 270;
		//float sin270 = std::sin(angle / 180.0 * acos(-1));
		//float cos270 = std::cos(angle / 180.0 * acos(-1));
		float sin270 = -1.f;
		float cos270 = 0.f;
		mat<2, 2> m;
		m.set_col(0, vec2(cos270, sin270));
		m.set_col(1, vec2(-sin270, cos270));
		vec2 v = m * vec2(forward.x, forward.y);
		if (v.x == 0.f && v.y == 0.f)
		{
			this->rightward = vec3(0, -1, 0);
		}
		else {
			this->rightward = vec3(v.x, v.y, 0).normalize();
		}
		
		this->upward = cross(rightward, forward).normalize();
	}

	vec3 get_projection(vec3 v);

	std::vector<vec3> rotation_formula(std::vector<vec3> points);
};


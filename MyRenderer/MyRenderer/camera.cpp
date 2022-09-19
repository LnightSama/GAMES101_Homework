#include "camera.h"
#include "geometry.h"
#include <iostream>
using namespace std;

vec3 Camera::get_projection(vec3 v) {
	vec3 ret = vec3(v[0], v[1], v[2]);
	//ret = (width > height) ? ret * width / 2 : ret * height / 2;
	ret = ret - position;
	//return vec3((rightward * ret + 1) * width / 2, (upward * ret + 1) * height / 2, (forward * ret + 1) * sight_distance / 2);
	return vec3((rightward * ret), (upward * ret), (forward * ret));
}

std::vector<vec3> Camera::rotation_formula(std::vector<vec3> points) {
	vec3 n1 = vec3(1, 0, 0);
	vec3 n2 = vec3(0, 1, 0);
	vec3 n3 = vec3(0, 0, 1);

	float z_cos = n3 * position / (position.norm() * n3.norm());
	float z_sin = sqrt(1 - z_cos * z_cos);

	float y_cos = n2 * position / (position.norm() * n2.norm());
	float y_sin = sqrt(1 - y_cos * y_cos);

	for (int i = 0;i < points.size();i++) {
		points[i] = points[i] + (1.0 - z_cos) * cross(n3, cross(n3, points[i])) + cross(n3, points[i]) * z_sin;
		points[i] = points[i] + (1.0 - y_cos) * cross(n2, cross(n2, points[i])) + cross(n2, points[i]) * y_sin;
	}
	return points;
}
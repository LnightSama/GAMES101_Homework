#include "geometry.h"
#include "tgaimage.h"
#include "light.h"
#include <iostream>
#include "camera.h"
#include "Texture.h"
using namespace std;

void Light::tinting(std::vector<vec3> vectors, std::vector<vec2> colors, TGAImage& image, Camera camera, Texture texture) {

	TGAColor color = TGAColor(255, 255, 255, 255);

	vectors = camera.rotation_formula(vectors);
	
	//                       Top,Bottom,        Right,left
	std::vector<int> area = { 0, image.height(), 0, image.width() };
	//面的法向量
	vec3 normal = cross(vectors[2] - vectors[0], vectors[1] - vectors[0]).normalize();

	for (int i = 0; i < 3; i++) {
		//在面处于原点为（0，0）时，旋转面
		//放大，并将原点变为（width/2， width/2）
		vectors[i] = (vectors[i] + vec3(1, 1, 1)) * image.width() / 2;
		//获取boundingbox
		if (area[0] < vectors[i].y && vectors[i].y < image.height()) area[0] = vectors[i].y;
		if (area[1] > vectors[i].y && vectors[i].y > 0) area[1] = vectors[i].y;
		if (area[2] < vectors[i].x && vectors[i].x < image.width()) area[2] = vectors[i].x;
		if (area[3] > vectors[i].x && vectors[i].x > 0) area[3] = vectors[i].x;
		//std::cout << vectors[i] << std::endl;
	}

	//TGAColor result_color = flat_shading(color, normal, normal);
	for (int y = area[1]; y <= area[0]; y++) {
		for (int x = area[3]; x <= area[2]; x++) {
			//判断该像素点是否处于面被投影在镜头里的三角形内
			if (inside(vectors, x + 0.5, y + 0.5))
			{
				//std::cout << x << " " << y << std::endl;
				//深度缓存，显示深度值高的
				if (z_buffer(normal, vectors[0], x, y))
				{
					vec3 point;
					for (int i = 3; i--; point[i] = depth[x][y][i]);
					//a 点对面三角形面积
					double a = cross(vectors[2] - point, vectors[1] - point).norm();
					//b 点对面三角形面积
					double b = cross(vectors[0] - point, vectors[2] - point).norm();
					//c 点对面三角形面积
					double c = cross(vectors[1] - point, vectors[0] - point).norm();
					double alpha = a / (a + b + c);
					double beta = b / (a + b + c);
					double gamma = 1 - alpha - beta;

					//cout << alpha << " " << beta << " " << gamma << endl;
					vec2 puv = alpha * colors[0] + beta * colors[1] + gamma * colors[2];
					//TGAColor result_color = texture.getColor(puv[0], puv[1]);
					TGAColor result_color = texture.bilinear_interpolation(puv[0], puv[1]);
					//TGAColor result_color = bump_fragment_shader(get_interpolation(vectors, x, y), texture, puv[0], puv[1]);
					//通过三个顶点向量的插值，获取该点的颜色
					//result_color = phong_shading(result_color, vectors, x, y);
					//std::cout << static_cast<unsigned int>(result_color[2]) << " ";
					//std::cout << static_cast<unsigned int>(result_color[1]) << " ";
					//std::cout << static_cast<unsigned int>(result_color[0]) << " ";
					//std::cout << std::endl;
					//vec3 p = vec3(x - image.width() / 2, y - image.width() / 2, depth[x][y][2] - image.width() / 2);
					vec3 p = vec3(x, y, depth[x][y][2]);
					//p = p + (1.0 - cos_angel) * cross(camera.forward, cross(camera.forward, p)) + cross(camera.forward, p) * sin_angel;
					//p = vec3(p[0] + image.width() / 2, p[1] + image.width() / 2)
					//std::cout << result_color[0] << " " << result_color[1] << " " << result_color[2] << std::endl;
					//std::cout << p[0] << " " << p[1] << std::endl;
					image.set(p[0], p[1], result_color);
					//image.set(x, y, result_color);
				}
				
			}

		}
	}

}

void Light::tinting(std::vector<vec3> vectors, std::vector<vec3> normals, std::vector<vec2> uvs, TGAImage& image, Camera camera, Texture texture) {

	TGAColor color = TGAColor(0, 0, 0, 255);

	vectors = enlarge(vectors, image.width() / 2);
	normals = enlarge(normals, image.width() / 2);

	//                       Top,Bottom,        Right,left
	std::vector<int> area = { 0, image.height(), 0, image.width() };
	//面的法向量
	vec3 normal = cross(vectors[2] - vectors[0], vectors[1] - vectors[0]).normalize();

	std::vector<vec3> projections = {
		camera.get_projection(vectors[0]) + vec3(image.width() / 2, image.width() / 2, 0),
		camera.get_projection(vectors[1]) + vec3(image.width() / 2, image.width() / 2, 0),
		camera.get_projection(vectors[2]) + vec3(image.width() / 2, image.width() / 2, 0)
	};

	for (int i = 0; i < 3; i++) {
		//获取boundingbox
		if (area[0] < projections[i].y && projections[i].y < image.height()) area[0] = projections[i].y;
		if (area[1] > projections[i].y && projections[i].y > 0) area[1] = projections[i].y;
		if (area[2] < projections[i].x && projections[i].x < image.width()) area[2] = projections[i].x;
		if (area[3] > projections[i].x && projections[i].x > 0) area[3] = projections[i].x;
		//std::cout << vectors[i] << std::endl;
	}

	//TGAColor result_color = flat_shading(color, normal);
	for (int y = area[1]; y <= area[0]; y++) {
		for (int x = area[3]; x <= area[2]; x++) {
			//判断该像素点是否处于面被投影在镜头里的三角形内
			if (inside(projections, x + 0.5, y + 0.5))
			{
				//a 点对面三角形面积
				double a = cross(vec2(projections[2].x - x, projections[2].y - y), vec2(projections[1].x - x, projections[1].y - y)) / 2;
				//b 点对面三角形面积
				double b = cross(vec2(projections[0].x - x, projections[0].y - y), vec2(projections[2].x - x, projections[2].y - y)) / 2;
				//c 点对面三角形面积
				double c = cross(vec2(projections[1].x - x, projections[1].y - y), vec2(projections[0].x - x, projections[0].y - y)) / 2;
				double alpha = a / (a + b + c);
				double beta = b / (a + b + c);
				double gamma = 1 - alpha - beta;

				double z = alpha * projections[0][2] + beta * projections[1][2] + gamma * projections[2][2];
				//深度缓存，显示深度值高的
				if (depth[x][y][3] > z)
				{
					depth[x][y][0] = x;
					depth[x][y][1] = y;
					depth[x][y][2] = z;
					depth[x][y][3] = z;
					//通过三个顶点向量的插值，获取该点的法线
					vec3 n = alpha * normals[0] + beta * normals[1] + gamma * normals[2];
					//通过三个顶点向量的插值，获取该点的uv
					vec2 bump_vec = alpha * uvs[0] + beta * uvs[1] + gamma * uvs[2];
					//TGAColor bump_color = TGAColor(bump_vec[0], bump_vec[1], bump_vec[2]);
					//TGAColor result_color = normal_fragment_shader(n);
					//TGAColor result_color = texture.getColor(puv[0], puv[1]);
					//TGAColor result_color = texture.bilinear_interpolation(puv[0], puv[1]);
					//TGAColor result_color = bump_fragment_shader(n, camera.position, bump_vec.x, bump_vec.y, texture);
					vec3 color3 = texture.getColor(bump_vec.x, bump_vec.y);
					TGAColor result_color = phong_fragment_shader(n, camera.position, color3);
					//TGAColor result_color = phong_shading(n, color3);
					//TGAColor result_color = TGAColor(color3[0] * 255, color3[1] * 255, color3[2] * 255);
					image.set(x, y, result_color);
				}

			}
		}
	}
}

bool Light::inside(std::vector<vec3> vectors, double x, double y) {

	double direction[3] = { 0., 0. ,0. };
	for (int i = 0; i < 3; i++) {
		vec3 v0 = vectors[i];
		vec3 v1 = vectors[(i + 1) % 3];
		direction[i] = (y - v0.y) * (v1.x - v0.x) - (x - v0.x) * (v1.y - v0.y);
	}

	double i = std::abs(direction[0]) + std::abs(direction[1]) + std::abs(direction[2]);
	double j = std::abs(direction[0] + direction[1] + direction[2]);

	//cout << i << " " << j << endl;
	return i == j;
}

bool Light::z_buffer(vec3 normal, vec3 vertex, int x, int y) {
	float z = vertex.z - (normal.x * (x - vertex.x) + normal.y * (y - vertex.y)) / normal.z;

	if (depth[x][y][3] < z)
	{
		depth[x][y][0] = x;
		depth[x][y][1] = y;
		depth[x][y][2] = z;
		depth[x][y][3] = z;
		return true;
	}
	return false;
	
}

TGAColor Light::phong_shading(vec3 normal, vec3 color) {

	vec3 l2 = vec3(-20, 20, 0).normalize();

	normal = normal.normalize();
	double n_l = max(0., -1 * normal * direction);
	double n_h = max(0., normal * l2);

	vec3 ka = vec3(0.005, 0.005, 0.005);
	vec3 kd = color;
	vec3 ks = vec3(0.7937, 0.7937, 0.7937);

	double r1 = vec3(20, 20, 20).norm2();
	double r2 = vec3(-20, 20, 0).norm2();
	vec3 r = ka * 10.0;
	r = r + kd * 500.0 / r1 * n_l + ks * 500.0 / r1 * pow(n_l, 32);
	r = r + kd * 500.0 / r2 * n_h + ks * 500.0 / r2 * pow(n_h, 32);

	for (int i = 0;i < 3;i++) {
		if (r[i] > 1)
		{
			r[i] = 1;
		} else if (r[i] < 0)
		{
			r[i] = 0;
		}
	}
	r = r * 255.0;
	//std::cout << vec3(static_cast<unsigned int>(color[2]), static_cast<unsigned int>(color[1]), static_cast<unsigned int>(color[0])) << "==" << r << std::endl;
	return TGAColor(r[0], r[1], r[2]);
}

TGAColor Light::gouraud_shading(TGAColor color, vec3 normal, vec3 center) {
	return light_color;
}

TGAColor Light::normal_fragment_shader(vec3 normal) {
	normal = (normal.normalize() + vec3(1.0f, 1.0f, 1.0f)) / 2.f;
	return TGAColor(normal.x * 255, normal.y * 255, normal.z * 255);
}

TGAColor Light::phong_fragment_shader(vec3 normal, vec3 view_pos, vec3 color)
{
	normal = normal.normalize();
	vec3 ka = vec3(0.005, 0.005, 0.005);
	vec3 kd = color;
	vec3 ks = vec3(0.7937, 0.7937, 0.7937);

	std::vector<vec3> l1 = { vec3(20.f, 20.f, 20.f), vec3(500.f, 500.f, 500.f) };
	std::vector<vec3> l2 = { vec3(-20.f, 20.f, 0.f), vec3(500.f, 500.f, 500.f) };

	std::vector<std::vector<vec3>> lights = { l1, l2 };
	double amb_light_intensity = 10;
	//vec3 eye_pos = vec3(0, 0, 10);

	vec3 result_color = vec3( 0.f, 0.f, 0.f);

	result_color = result_color + ka * amb_light_intensity;

	for (auto& light : lights)
	{
		// TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
		// components are. Then, accumulate that result on the *result_color* object.
		vec3 power = light[1] / light[0].norm2();
		vec3 l = light[0].normalize();
		vec3 h = (2 * normal - l).normalize();
		double ld = (kd * power) * max(0., normal * l);
		double ls = (ks * power) * pow(max(0., normal * h), 32);
		result_color = result_color + ld + ls;
	}

	for (int i = 0;i < 3;i++) {
		if (result_color[i] > 1)
		{
			result_color[i] = 1;
		}
		else if (result_color[i] < 0)
		{
			result_color[i] = 0;
		}
	}
	result_color = result_color * 255.f;
	return TGAColor(result_color[0], result_color[1], result_color[2]);
}

TGAColor Light::bump_fragment_shader(vec3 normal, vec3 view_pos, double v, double u, Texture texture) {
	normal = normal.normalize();
	double kh = 0.2;
	double kn = 0.1;
	double x = normal.x;
	double y = normal.y;
	double z = normal.z;
	vec3 t = vec3(x * y / sqrt(x * x + z * z), sqrt(x * x + z * z), z * y / sqrt(x * x + z * z));
	vec3 b = cross(normal, t);
	mat<3, 3> TBN;
	TBN.set_col(0, t);
	TBN.set_col(1, b);
	TBN.set_col(2, normal);
	//TBN = TBN.transpose();
	double v1 = texture.getColor(u + 1.0 / texture.width, v).norm();
	double v2 = texture.getColor(u, v).norm();
	double v3 = texture.getColor(u, v + 1.0 / texture.height).norm();
	double dU = kh * kn * (v1 - v2);
	double dV = kh * kn * (v3 - v2);
	vec3 ln = vec3(-dU, dV, 1);
	//std::cout << ln << std::endl;
	//std::cout << "t：" << std::endl;
	//std::cout << t << std::endl;
	//std::cout << "矩阵：" << std::endl;
	//std::cout << TBN << std::endl;
	// Let n = normal = (x, y, z)
	// Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
	// Vector b = n cross product t
	// Matrix TBN = [t b n]
	// dU = kh * kn * (h(u+1/w,v)-h(u,v))
	// dV = kh * kn * (h(u,v+1/h)-h(u,v))
	// Vector ln = (-dU, -dV, 1)
	// Normal n = normalize(TBN * ln)
	vec3 r = TBN * ln;

	r = r.normalize() * 255.0;
	//std::cout << r << std::endl;
	return TGAColor(r[0], r[1], r[2]);
}

TGAColor Light::flat_shading(TGAColor color, vec3 normal) {
	//double r_2 = pow(center.x - position.x, 2) + pow(center.y - position.y, 2) + pow(center.z - position.z, 2);
	double n_l = normal * direction;
	//double n_l = normal.normalize() * direction.normalize();
	double a = max(0., n_l);
	//double a = static_cast<unsigned int>(light_color[3]) / (static_cast<unsigned int>(color[3]) + static_cast<unsigned int>(light_color[3]));
	//int r = static_cast<unsigned int>(color.bgra[2]) * (1 - a) + static_cast<unsigned int>(light_color.bgra[2]) * a;
	//int g = static_cast<unsigned int>(color.bgra[1]) * (1 - a) + static_cast<unsigned int>(light_color.bgra[1]) * a;
	//int b = static_cast<unsigned int>(color.bgra[0]) * (1 - a) + static_cast<unsigned int>(light_color.bgra[0]) * a;
	//return TGAColor(r, g, b, 255);
	return color * (1 - a) + light_color * a;
}

vec3 Light::get_interpolation(std::vector<vec3> vectors, int x, int y) {
	vec3 point;
	for (int i = 3; i--; point[i] = depth[x][y][i]);
	//a 点对面三角形面积
	double a = cross(vectors[2] - point, vectors[1] - point).norm();
	//b 点对面三角形面积
	double b = cross(vectors[0] - point, vectors[2] - point).norm();
	//c 点对面三角形面积
	double c = cross(vectors[1] - point, vectors[0] - point).norm();
	double alpha = a / (a + b + c);
	double beta = b / (a + b + c);
	double gamma = 1 - alpha - beta;
	vec3 l2 = (vec3(0, 0, 0) - vec3(-20, 20, 0)).normalize();
	//cout << alpha << " " << beta << " " << gamma << endl;
	vec3 normal = alpha * vectors[3] + beta * vectors[4] + gamma * vectors[5];
	return normal.normalize();
}

void Light::get_extremum(std::vector<vec3> vectors, int p, float* min, float* max) {
	*min = INT_MAX;
	*max = INT_MIN;
	for (int i = 0; i < vectors.size(); i++)
	{
		if (vectors[i][p] < *min) *min = vectors[i][p];
		if (vectors[i][p] > *max) *max = vectors[i][p];
	}
}

std::vector<vec3> Light::enlarge(std::vector<vec3> v, int num) {
	for (int i = 0; i < v.size(); i++) v[i] = v[i] * num;
	return v;
}
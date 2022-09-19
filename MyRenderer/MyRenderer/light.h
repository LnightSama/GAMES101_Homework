#pragma once
#include "geometry.h"
#include "tgaimage.h"
#include "camera.h"
#include "Texture.h"

class Light {

public:

	vec3 direction;
	const vec3 position;
	const double power;
	TGAColor light_color;
	std::vector<std::vector<vec4>> depth;

	Light(
		const vec3 position,
		const double power,
		TGAColor light_color,
		std::vector<std::vector<vec4>> depth
	) : position(position), power(power), light_color(light_color), depth(depth) {
		direction = (vec3(0, 0, 0) - position).normalize();
	}
	//面着色，根据每个面的法向量与光线的夹角的大于零的余弦值来求颜色
	TGAColor flat_shading(TGAColor color, vec3 normal);
	//先利用每个顶点的法线，给每个三角形的顶点着色，再用三个顶点的颜色以及重心给三角形内的每个点做插值获取颜色
	TGAColor gouraud_shading(TGAColor color, vec3 normal, vec3 center);
	//利用每个顶点的法线，和重心，给三角形内每个点做插值获取每个点的法线，再根据每个点的法线与光线的夹角的大于零的余弦值来求颜色
	TGAColor phong_shading(vec3 normal, vec3 color);
	TGAColor normal_fragment_shader(vec3 normal);
	TGAColor phong_fragment_shader(vec3 normal, vec3 view_pos, vec3 color);
	TGAColor bump_fragment_shader(vec3 normal, vec3 view_pos, double v, double u, Texture texture);
	//着色
	void tinting(std::vector<vec3> vectors, std::vector<vec2> colors, TGAImage& image, Camera camera, Texture texture); // 着色
	
	void tinting(std::vector<vec3> vectors, std::vector<vec3> normals, std::vector<vec2> uvs, TGAImage& image, Camera camera, Texture texture);
	//利用二维向量叉乘，判断点是否在三角形内部
	bool inside(std::vector<vec3> vectors, double x, double y); //判断像素点是否在三角形面里
	//深度缓存
	bool z_buffer(vec3 normal, vec3 vertex, int x, int y);

	vec3 get_interpolation(std::vector<vec3> vectors, int x, int y);

	std::vector<vec3> enlarge(std::vector<vec3> v, int num);

	static void get_extremum(std::vector<vec3> vectors, int p, float* min, float* max);
};
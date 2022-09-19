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
	//����ɫ������ÿ����ķ���������ߵļнǵĴ����������ֵ������ɫ
	TGAColor flat_shading(TGAColor color, vec3 normal);
	//������ÿ������ķ��ߣ���ÿ�������εĶ�����ɫ�����������������ɫ�Լ����ĸ��������ڵ�ÿ��������ֵ��ȡ��ɫ
	TGAColor gouraud_shading(TGAColor color, vec3 normal, vec3 center);
	//����ÿ������ķ��ߣ������ģ�����������ÿ��������ֵ��ȡÿ����ķ��ߣ��ٸ���ÿ����ķ�������ߵļнǵĴ����������ֵ������ɫ
	TGAColor phong_shading(vec3 normal, vec3 color);
	TGAColor normal_fragment_shader(vec3 normal);
	TGAColor phong_fragment_shader(vec3 normal, vec3 view_pos, vec3 color);
	TGAColor bump_fragment_shader(vec3 normal, vec3 view_pos, double v, double u, Texture texture);
	//��ɫ
	void tinting(std::vector<vec3> vectors, std::vector<vec2> colors, TGAImage& image, Camera camera, Texture texture); // ��ɫ
	
	void tinting(std::vector<vec3> vectors, std::vector<vec3> normals, std::vector<vec2> uvs, TGAImage& image, Camera camera, Texture texture);
	//���ö�ά������ˣ��жϵ��Ƿ����������ڲ�
	bool inside(std::vector<vec3> vectors, double x, double y); //�ж����ص��Ƿ�������������
	//��Ȼ���
	bool z_buffer(vec3 normal, vec3 vertex, int x, int y);

	vec3 get_interpolation(std::vector<vec3> vectors, int x, int y);

	std::vector<vec3> enlarge(std::vector<vec3> v, int num);

	static void get_extremum(std::vector<vec3> vectors, int p, float* min, float* max);
};
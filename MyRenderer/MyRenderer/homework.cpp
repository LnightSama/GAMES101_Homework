#include "homework.h"
#include "geometry.h"
#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "line.h"
#include "light.h"
#include "camera.h"
#include "model.h"
#include "Texture.h"

const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor black = TGAColor(0, 0, 0, 255);

void Homework::test_homework() {
	std::string obj_path = "obj/models/spot/";
	int width = 1024;
	int height = 1024;
	std::vector<std::vector<vec4>> depth(width, std::vector<vec4>(height, vec4(0, 0, 0, INT_MAX)));

	//图像
	TGAImage image(width, height, TGAImage::RGB);
	//连线
	Line line;
	//光照
	Light light(vec3(1, 1, 1), 0., white, depth);
	//相机
	Camera camera(vec3(1, 1, -1), image.width(), image.height(), 1000);
	Model* model = new Model(obj_path + "spot_triangulated_good.obj");
	Texture texture(obj_path + "spot_texture.tga");

	std::cout << camera.upward << std::endl;
	std::cout << camera.rightward << std::endl;
	std::cout << camera.forward << std::endl;
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<vec3> vectors = {
			model->vert(i, 0),
			model->vert(i, 1),
			model->vert(i, 2),
		};

		std::vector<vec3> normals = {
			model->normal(i, 0),
			model->normal(i, 1),
			model->normal(i, 2)
		};

		std::vector<vec2> uvs = {
			model->uv(i, 0),
			model->uv(i, 1),
			model->uv(i, 2)
		};

		light.tinting(vectors, normals, uvs, image, camera, texture);

	}
	image.flip_antiClockwise90();
	image.write_tga_file("output/lesson05.tga");
}

void Homework::zeroth_homework() {
	vec3 v = vec3(2.f, 1.f, 1);
	float sin45 = std::sin(45.0 / 180.0 * acos(-1));
	float cos45 = std::cos(45.0 / 180.0 * acos(-1));
	mat<3, 3> rotation45;
	rotation45.set_col(0, vec3(cos45, -sin45, 0.0));
	rotation45.set_col(1, vec3(sin45,  cos45, 0.0));
	rotation45.set_col(2, vec3(0.0  ,  0.0  , 1.0));
	rotation45 = rotation45.transpose();
	
	std::cout << rotation45 << std::endl;
	v = rotation45 * v;
	std::cout << v << std::endl;

	float tx = 1;
	float ty = 2;
	mat<3, 3> translation_1_2;
	translation_1_2.set_col(0, vec3(1.0, 0.0, tx));
	translation_1_2.set_col(1, vec3(0.0, 1.0, ty));
	translation_1_2.set_col(2, vec3(0.0, 0.0, 1.0));
	translation_1_2 = translation_1_2.transpose();

	v = translation_1_2 * v;
	std::cout << v  << std::endl;

	float sx = 2;
	float sy = 2;
	mat<3, 3> scale2;
	scale2.set_col(0, vec3(sx , 0.0, 0.0));
	scale2.set_col(1, vec3(0.0, sy , 0.0));
	scale2.set_col(2, vec3(0.0, 0.0, 1.0));
	scale2 = scale2.transpose();
}

void Homework::first_homework() {
	TGAImage image(700, 700, TGAImage::RGB);
	
	Line line;

	std::vector<vec3> points = {
		vec3(2, 0, 2),
		vec3(0, 2, -2),
		vec3(-2, 0, -2)
	};
	float angel = 0;
	vec3 eye_pos = vec3(0, 1, 0);
	vec3 normal = cross(points[0] - points[1], points[2] - points[1]).normalize();
	vec3 k = vec3(0, 0, 1);
	float sin_angel = std::sin(angel / 180.0 * acos(-1));
	float cos_angel = std::cos(angel / 180.0 * acos(-1));
	{
		//围绕x轴旋转
		mat<4, 4> roll;
		roll.set_col(0, vec4(1.0, 0.0, 0.0, 0.0));
		roll.set_col(1, vec4(0.0, cos_angel, -sin_angel, 0.0));
		roll.set_col(2, vec4(0.0, sin_angel, cos_angel, 0.0));
		roll.set_col(3, vec4(0.0, 0.0, 0.0, 1.0));
		roll = roll.transpose();
		//围绕y轴旋转
		mat<4, 4> pitch;
		pitch.set_col(0, vec4(cos_angel, 0.0, sin_angel, 0.0));
		pitch.set_col(1, vec4(0.0, 1.0, 0.0, 0.0));
		pitch.set_col(2, vec4(-sin_angel, 0.0, cos_angel, 0.0));
		pitch.set_col(3, vec4(0.0, 0.0, 0.0, 1.0));
		pitch = pitch.transpose();
		//围绕z轴旋转
		mat<4, 4> yaw;
		yaw.set_col(0, vec4(cos_angel, -sin_angel, 0.0, 0.0));
		yaw.set_col(1, vec4(sin_angel, cos_angel, 0.0, 0.0));
		yaw.set_col(2, vec4(0.0, 0.0, 1.0, 0.0));
		yaw.set_col(3, vec4(0.0, 0.0, 0.0, 1.0));
		yaw = yaw.transpose();
	}
	//罗德里格旋转公式
	{
		//normal = normal + (1.0 - cos_angel) * cross(k, cross(k, normal)) + cross(k, normal) * sin_angel;
		points[0] = points[0] + (1.0 - cos_angel) * cross(k, cross(k, points[0])) + cross(k, points[0]) * sin_angel;
		points[1] = points[1] + (1.0 - cos_angel) * cross(k, cross(k, points[1])) + cross(k, points[1]) * sin_angel;
		points[2] = points[2] + (1.0 - cos_angel) * cross(k, cross(k, points[2])) + cross(k, points[2]) * sin_angel;
	}
	//std::cout << points[0] << std::endl;
	//std::cout << points[1] << std::endl;
	//std::cout << points[2] << std::endl;
	Camera camera(eye_pos, 700, 700, 5);
	float t, b, l, r, f, n;
	Light::get_extremum(points, 0, &f, &n);
	Light::get_extremum(points, 1, &l, &r);
	Light::get_extremum(points, 2, &b, &t);
	//std::cout << t << " " << b << " " << l << " " << r << " " << f << " " << n << std::endl;
	mat<4, 4> translate_to_origin;
	translate_to_origin.set_col(0, vec4(1.0, 0.0, 0.0, -(n + f) / 2));
	translate_to_origin.set_col(1, vec4(0.0, 1.0, 0.0, -(r + l) / 2));
	translate_to_origin.set_col(2, vec4(0.0, 0.0, 1.0, -(t + b) / 2));
	translate_to_origin.set_col(3, vec4(0.0, 0.0, 0.0, 1.0));
	translate_to_origin = translate_to_origin.transpose();
	

	mat<4, 4> scale_normalize;
	scale_normalize.set_col(0, vec4((n - f) ? 2 / (n - f) : 1, 0.0, 0.0, 0.0));
	scale_normalize.set_col(1, vec4(0.0, (r - l) ? 2 / (r - l) : 1, 0.0, 0.0));
	scale_normalize.set_col(2, vec4(0.0, 0.0, (t - b) ? 2 / (t - b) : 1, 0.0));
	scale_normalize.set_col(3, vec4(0.0, 0.0, 0.0, 1.0));
	scale_normalize = scale_normalize.transpose();

	std::vector<vec4> ps = {
		vec4(points[0][0], points[0][1], points[0][2], 1),
		vec4(points[1][0], points[1][1], points[1][2], 1),
		vec4(points[2][0], points[2][1], points[2][2], 1)
	};
	
	for (int i = 0;i < ps.size();i++) {
		std::cout << translate_to_origin * ps[i] << std::endl;
		ps[i] = scale_normalize * (translate_to_origin * ps[i]);
		
	}
	std::cout << std::endl;
	
	std::cout << ps[0] << std::endl;
	std::cout << ps[1] << std::endl;
	std::cout << ps[2] << std::endl;
	//for (int i = 0;i < ps.size();i++) {
	//	points[i] = camera.get_projection(ps[i]);
	//}
	std::cout << camera.forward << std::endl;
	std::cout << camera.rightward << std::endl;
	std::cout << camera.upward << std::endl;
	line.aline(points[0][0], points[0][1], points[1][0], points[1][1], image, TGAColor(255, 255, 255, 255));
	line.aline(points[0][0], points[0][1], points[2][0], points[2][1], image, TGAColor(255, 255, 255, 255));
	line.aline(points[2][0], points[2][1], points[1][0], points[1][1], image, TGAColor(255, 255, 255, 255));
	image.write_tga_file("output/lesson00.tga");
}

void Homework::second_homework() {
	int width = 700;
	int height = 700;
	TGAImage image(width, height, TGAImage::RGB);
	std::vector<std::vector<vec4>> depth(width, std::vector<vec4>(height, vec4(0, 0, 0, INT_MIN)));
	Line line;
	Light light(vec3(0, 0, 5), 0., TGAColor(255, 255, 255, 255), depth);
	
	vec3 eye_pos = { 0,0,5 };
	std::vector<vec3> pos = {
		vec3(2, 0, -2),
		vec3(0, 2, -2),
		vec3(-2, 0, -2),
		vec3(3.5, -1, -5),
		vec3(2.5, 1.5, -5),
		vec3(-1, 0.5, -5)
	};

	std::vector<vec3> ind = {
		vec3(0, 1, 2),
		vec3(3, 4, 5)
	};

	std::vector<vec3> cols = {
		vec3(217.0, 238.0, 185.0),
		vec3(185.0, 217.0, 238.0)
	};

	
	vec3 normal = vec3(0, 0, 1);
	for (int j = 0;j < 2;j++) {

		std::vector<vec3> vectors = {
			pos[ind[j][0]],
			pos[ind[j][1]],
			pos[ind[j][2]],
			vec3(0,0,1),
			vec3(0,0,1),
			vec3(0,0,1)
		};

		{
			std::vector<int> area = { 0, image.height(), 0, image.width() };

			for (int i = 0;i < 3;i++) {

				vectors[i] = vectors[i] * 90;
				vectors[i] = vectors[i] + vec3(350, 350, 350);

				if (area[0] < vectors[i].y && vectors[i].y < image.height()) area[0] = vectors[i].y;
				if (area[1] > vectors[i].y && vectors[i].y > 0) area[1] = vectors[i].y;
				if (area[2] < vectors[i].x && vectors[i].x < image.width()) area[2] = vectors[i].x;
				if (area[3] > vectors[i].x && vectors[i].x > 0) area[3] = vectors[i].x;
			}

			for (int y = area[1]; y <= area[0]; y++) {
				for (int x = area[3]; x <= area[2]; x++) {
					//判断该像素点是否处于面被投影在镜头里的三角形内
					if (light.inside(vectors, x + 0.5, y + 0.5))
					{
						//深度缓存，显示深度值高的
						if (light.z_buffer(normal, vectors[0], x, y))
						{
							//通过三个顶点向量的插值，获取该点的颜色
							TGAColor result_color = TGAColor(cols[j][0], cols[j][1], cols[j][2], 255);
							vec3 p = vec3(x, y, depth[x][y][2]);
							image.set(p[0], p[1], result_color);
						}

					}

				}
			}
		}
		//for (int i = 0;i < 3;i++) {
			//line.aline(
			//	pos[ind[j][i]][0], pos[ind[j][i]][1],
			//	pos[ind[j][(i + 1) % 3]][0], pos[ind[j][(i + 1) % 3]][1],
			//	image,
			//	TGAColor(cols[ind[j][i]][0], cols[ind[j][i]][1], cols[ind[j][i]][2], 255)
			//);
		//}
	}



	image.write_tga_file("output/lesson00.tga");
}

void Homework::third_homework() {
	std::string obj_path = "obj/models/spot/";
	int width = 1024;
	int height = 1024;
	std::vector<std::vector<vec4>> depth(width, std::vector<vec4>(height, vec4(0, 0, 0, INT_MIN)));

	//图像
	TGAImage image(width, height, TGAImage::RGB);
	//连线
	Line line;
	//光照
	Light light(vec3(0, 0, 1), 0., white, depth);
	//相机
	Camera camera(vec3(0, -2, 3), image.width(), image.height(), 1000);
	Model* model = new Model(obj_path + "spot_triangulated_good.obj");
	Texture texture(obj_path + "spot_texture1.tga");
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<vec3> vectors = {
			model->vert(i, 0),
			model->vert(i, 1),
			model->vert(i, 2),
		};

		std::vector<vec3> normals = {
			model->normal(i, 0),
			model->normal(i, 1),
			model->normal(i, 2)
		};

		std::vector<vec3> uvs = {
			model->normal(model->uv(i, 0)),
			model->normal(model->uv(i, 1)),
			model->normal(model->uv(i, 2))
		};

		//light.tinting(vectors, normals, uvs, image, camera, texture);

	}
	image.write_tga_file("output/lesson04.tga");
}

void Homework::fourth_homework() {

}
void Homework::fifth_homework() {

}
void Homework::sixth_homework() {

}
void Homework::seventh_homework() {

}
void Homework::eighth_homework() {

}
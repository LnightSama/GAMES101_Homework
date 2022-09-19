//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
	Intersection inter = Scene::intersect(ray);
	if (!inter.happened)
		return {};
	//一、交点是光源：
	if (inter.m->hasEmission()) 
		return inter.m->getEmission();

	Vector3f n = inter.normal;
	Vector3f x = inter.coords;
	Vector3f wi = ray.direction;
	Vector3f p = ray.origin;

	Intersection light_inter;
	float pdf_light = 0.0f;
	sampleLight(light_inter, pdf_light);
	Vector3f light_point_v3 = light_inter.coords - x;
	Vector3f point_light_dir = light_point_v3.normalized();
	Ray light_to_object_ray(inter.coords, point_light_dir);
	Intersection light_to_anything_ray = Scene::intersect(light_to_object_ray);
	auto f_r = inter.m->eval(ray.direction, point_light_dir, inter.normal);
	Vector3f L_Dir;
	if (light_to_anything_ray.distance - light_point_v3.norm() > -0.005)
	{
		L_Dir = light_inter.emit * f_r * dotProduct(point_light_dir, inter.normal) * dotProduct(-point_light_dir, inter.normal) / dotProduct(light_point_v3, light_point_v3) /
			pdf_light;
	}

	if (get_random_float() > RussianRoulette)     //打到物体后对半圆随机采样使用RR算法
		return L_Dir;

	Vector3f L_indir;
	Vector3f wo = inter.m->sample(wi, inter.normal).normalized();
	Ray object_to_object_ray(inter.coords, wo);
	Intersection inter1 = Scene::intersect(object_to_object_ray);
	if (inter1.happened && !inter1.m->hasEmission())
	{
		L_indir =
			castRay(object_to_object_ray, depth + 1) *
			inter.m->eval(wi, wo, inter.normal) *
			dotProduct(wo, inter.normal) /
			inter.m->pdf(wi, wo, inter.normal) /
			RussianRoulette;
	}

	return L_Dir + L_indir;

}
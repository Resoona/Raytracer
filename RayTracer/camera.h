#pragma once

#include "ray.h"

vec3 random_in_unit_disk()
{
	vec3 p;
	do
	{
		auto r1 = rand();
		auto r2 = rand();
		vec3 rr = vec3(static_cast<float>(r1) / RAND_MAX, static_cast<float>(r2) / RAND_MAX, 0);
		p = 2.0*rr - vec3(1, 1, 0);		
		
	} while (dot(p, p) >= 1.0);


	return p;
}

class camera
{
public:
	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	float lens_radius;
	ray get_ray(float s, float t)
	{
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
		return ray(origin+offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
	}
	camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist);
	~camera();
};


camera::camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist)
{
	lens_radius = aperture / 2;
	float theta = vfov * 3.14159 / 180;
	float half_height = tan(theta / 2);
	float half_width = aspect * half_height;
	origin = lookfrom;
	w = unit_vector(lookfrom - lookat);
	u = unit_vector(cross(vup, w));
	v = cross(w, u);
	//lower_left_corner = vec3(-half_width, -half_height, -1.0);
	lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v - w * focus_dist;
	horizontal = 2 * half_width*focus_dist*u;
	vertical = 2 * half_height*focus_dist*v;
	
}


camera::~camera()
{
}

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "sphere.h"
#include "hitablelist.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include <random>


vec3 color(const ray& r, hitable *world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		else {
			return vec3(0, 0, 0);
		}
	}
	else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5*(unit_direction.y() + 1.0);
		return (1.0 - t)*vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

float get_rand()
{
	return static_cast<float>(rand()) / RAND_MAX;
}

hitable *random_scene()
{
	int n = 500;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
	int i = 1;
	for (int a = -6; a < 6; a++)
	{
		for (int b = -6; b < 6; b++)
		{
			float choose_mat = get_rand();
			vec3 center(a + 0.9* get_rand(), 0.2, b + 0.9*get_rand());
			if ((center - vec3(4.0, 0.2, 0)).length() > 0.9)
			{
				
				if (choose_mat < 0.8) //diffuse
				{
					list[i++] = new sphere(center, 0.2,
						new lambertian(vec3(get_rand()*get_rand(), get_rand()*get_rand(), get_rand()*get_rand())));
				}
				else if (choose_mat < 0.95) //metal
				{
					list[i++] = new sphere(center, 0.2,
						new metal(vec3(0.5*(1 + get_rand()), 0.5*(1 + get_rand()), 0.5*(1 + get_rand())), 0.5 * get_rand()));
				}
				else //Glass
				{
					list[i++] = new sphere(center, 0.2,
						new dielectric(1.5));
				}
			}
		}
	}

	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new hitable_list(list, i);
}

int main()
{
	int nx = 800;
	int ny = 600;
	int ns = 40;
	std::ofstream myfile;
	myfile.open("color.PPM");
	myfile << "P3\n" << nx << " " << ny << "\n255\n";
	hitable *list[5];
	list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
	list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
	list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0));
	list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
	list[4] = new sphere(vec3(-1, 0, -1), -0.49, new dielectric(1.5));
	hitable *world = new hitable_list(list, 5);

	world = random_scene();

	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	float dist_to_focus = (lookfrom - lookat).length();
	float aperture = 0.1;

	camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus);
	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i<nx; i++)
		{
			vec3 col(0, 0, 0);
			for (int s=0; s < ns; s++)
			{
				float u = float(i + (float)rand() / (RAND_MAX)) / float(nx);
				float v = float(j + (float)rand() / (RAND_MAX )) / float(ny);
				ray r = cam.get_ray(u, v);
				vec3 p = r.pointing_at_parameter(2.0);
				col += color(r, world, 0);
			}

			col /= float(ns);
			col = vec3(sqrtf(col[0]), sqrtf(col[1]), sqrtf(col[2]));
			int ir = int(255.99*col[0]);
			int ig = int(255.99*col[1]);
			int ib = int(255.99*col[2]);
			myfile << ir << " " << ig << " " << ib << "\n";
		}
	}
	

}

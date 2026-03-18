#include "library/library.hpp"

const uint32_t ImageWidth = 960;
const uint32_t ImageHeight = 540;

/**
 * Creates the 3D scene to be rendered.
 */
Scene make_scene()
{
	Scene scene;

	scene.insert_sphere({0.0f, 1.0f, 0.0f}, 1.0f, 0);
	scene.insert_plane({0.0f, 1.0f, 0.0f}, 0.0f, 1);

	return scene;
}

const Scene scene = make_scene();

Color escape(Vec3 direction)
{
	return direction * direction;
}

Color bsdf_smooth_conductor(Vec3 outgoing, Vec3 normal, Vec3 &incoming)
{
	incoming = random_on_sphere();
	make_same_side(outgoing, normal, incoming);
}

Color bsdf(uint32_t material, Vec3 outgoing, Vec3 normal, Vec3 &incoming)
{
	incoming = random_on_sphere();
	make_same_side(outgoing, normal, incoming);

	switch(material)
	{
		case 0: return Color(1.0f, 0.0f, 0.0f);
		case 1: return Color(0.0f, 0.0f, 1.0f);
		default: break;
	}

	return Color();
}

Color evaluate(Ray ray, size_t depth)
{
	if (depth == 0) return escape(ray.direction);

	float distance;
	Vec3 normal;
	uint32_t material;

	if (not scene.intersect(ray, distance, normal, material)) return escape(ray.direction);

	Vec3 outgoing = -ray.direction;
	Vec3 incoming;

	Color scatter = bsdf(material, outgoing, normal, incoming);
	Ray new_ray = bounce(ray, distance, incoming);

	Color radiance = evaluate(new_ray, depth - 1);
	float lambertian = abs_dot(normal, incoming);

	return scatter * radiance * lambertian;
}

Color render_sample(float u, float v)
{
	Ray ray;

	ray.origin = Vec3(0.0f, 1.0f, -5.0f);
	ray.direction = normalize(Vec3(u, v, 1.0f));

	return evaluate(ray, 4);
}

/**
 * Calculates the color value to output for a pixel.
 */
Color render_pixel(uint32_t x, uint32_t y)
{
	Color result;

	float u = (static_cast<float>(x) - ImageWidth / 2.0f) / ImageWidth;
	float v = (static_cast<float>(y) - ImageHeight / 2.0f) / ImageWidth;

	for (size_t i = 0; i < 8; i++)
		result = result + render_sample(u, v);
	return result / 8;
}

int main()
{
	Image image(ImageWidth, ImageHeight);

	for (uint32_t y = 0; y < ImageHeight; ++y)
	{
		for (uint32_t x = 0; x < ImageWidth; ++x)
		{
			Color color = render_pixel(x, y);
			image.set_pixel(x, y, color);
		}
	}

	image.write_file("output.png");
	return 0;
}


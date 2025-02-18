#include "library/library.hpp"

const uint32_t ImageWidth = 960;
const uint32_t ImageHeight = 540;

/**
 * Creates the 3D scene to be rendered.
 */
Scene make_scene()
{
	Scene scene;

	scene.insert_sphere({0.0f, 1.0f, 0.0f}, 1.0f);
	scene.insert_plane({0.0f, 1.0f, 0.0f}, 0.0f);

	return scene;
}

const Scene Scene = make_scene();

Color bsdf(Vec3 outgoing, Vec3 normal, Vec3& incident)
{
	incident = random_on_sphere();
	make_same_side(outgoing, normal, incident);
	return Color(1.0f);
}

/**
 * Determines the amount of light contributed by rays that have escaped the scene.
 * Note that there are many names for this concept, e.g. environment light, HDRI, sky box.
 */
Color escape(Vec3 direction)
{
	return direction * direction;
}

/**
 * Evaluates one sample of the rendering equation.
 */
Color evaluate(const Ray& ray, uint32_t depth)
{
	if (depth == 0) return escape(ray.direction);

	float distance;
	Vec3 normal;

	if (not Scene.intersect(ray, distance, normal)) return escape(ray.direction);

	Vec3 outgoing = -ray.direction;
	Vec3 incident;

	Color scatter = bsdf(outgoing, normal, incident);

	Ray new_ray = bounce(ray, distance, incident);
	float lambertian = abs_dot(normal, incident);
	return scatter * evaluate(new_ray, depth - 1) * lambertian;
}

/**
 * Calculates one Monte Carlo integration sample through the camera.
 */
Color render_sample(float u, float v)
{
	Ray ray;

	//The orientation of the camera determines the orientation of the initial rays
	ray.origin = Vec3(0.0f, 1.0f, -5.0f);
	ray.direction = normalize(Vec3(u, v, 1.0f));

	return evaluate(ray, MaxBounces);
}

/**
 * Calculates the color value to output for a pixel.
 */
Color render_pixel(uint32_t x, uint32_t y)
{
	Color result;

	float u = (static_cast<float>(x) - ImageWidth / 2.0f) / ImageWidth;
	float v = (static_cast<float>(y) - ImageHeight / 2.0f) / ImageWidth;

	result = Color(u, v, 1.0f);
	return result;
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


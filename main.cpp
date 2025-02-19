#include "library/library.hpp"

const uint32_t ImageWidth = 512 * 4;
const uint32_t ImageHeight = 512 * 4;
const uint32_t SamplesPerPixel = 64 * 16;
const uint32_t MaxBounces = 128;

/**
 * Creates the 3D scene to be rendered.
 */
Scene make_scene()
{
	Scene scene;

	//Cornell box interior scene
	scene.insert_plane({ 0.0f, 1.0f, 0.0f }, 0.0f, 0);
	scene.insert_plane({ 0.0f, 0.0f, -1.0f }, 5.0f, 3);
	scene.insert_plane({ 1.0f, 0.0f, 0.0f }, 5.0f, 1);
	scene.insert_plane({ 0.0f, -1.0f, 0.0f }, 10.0f, 0);
	scene.insert_plane({ -1.0f, 0.0f, 0.0f }, 5.0f, 2);

	scene.insert_sphere({ -2.5f, 2.0f, 1.5f }, 2.0f, 4);
	scene.insert_sphere({ 2.0f, 2.0f, -2.5f }, 2.0f, 5);
	scene.insert_box({ 0.0f, 8.75f, 0.0f }, { 6.0f, 0.1f, 6.0f }, 5);
	scene.insert_box({ 0.0f, 8.25f, 0.0f }, { 6.0f, 0.1f, 6.0f }, 5);
	scene.insert_box({ 0.0f, 7.75f, 0.0f }, { 6.0f, 0.1f, 6.0f }, 5);
	scene.insert_box({ 0.0f, 7.25f, 0.0f }, { 6.0f, 0.1f, 6.0f }, 5);
	scene.insert_box({ 0.0f, 6.75f, 0.0f }, { 6.0f, 0.1f, 6.0f }, 5);
	scene.insert_box({ 0.0f, 6.25f, 0.0f }, { 6.0f, 0.1f, 6.0f }, 5);
	scene.insert_box({ 0.0f, 5.75f, 0.0f }, { 6.0f, 0.1f, 6.0f }, 5);
	scene.insert_box({ 0.0f, 5.25f, 0.0f }, { 6.0f, 0.1f, 6.0f }, 5);
	scene.insert_box({ 0.0f, 4.75f, 0.0f }, { 6.0f, 0.1f, 6.0f }, 5);

	scene.insert_box({ -4.5f, 5.0f, -4.5f }, { 0.4f, 7.0f, 0.4f }, 10);
	scene.insert_box({ 4.5f, 5.0f, -4.5f }, { 0.4f, 7.0f, 0.4f }, 10);
	scene.insert_box({ -2.05f, 9.5f, 0.0f }, { 1.9f, 0.2f, 6.0f }, 11);
	scene.insert_box({ 0.0f, 9.5f, 0.0f }, { 1.9f, 0.2f, 6.0f }, 12);
	scene.insert_box({ 2.05f, 9.5f, 0.0f }, { 1.9f, 0.2f, 6.0f }, 13);

	return scene;
}

const Scene Scene = make_scene();

Color bsdf_lambertian_reflection(Vec3 outgoing, Vec3 normal, Vec3& incident)
{
	//Uniform sampling
	//	incident = random_on_sphere();
	//	float pdf = 1.0f / Pi / 2.0f;

	//Importance sampling based on cosine distribution
	incident = random_cosine_hemisphere(normal);
	float pdf = pdf_cosine_hemisphere(normal, incident);
	if (almost_zero(pdf)) return Color();

	make_same_side(outgoing, normal, incident);
	float evaluated = 1.0f / Pi;
	return Color(evaluated / pdf);
}

Color bsdf_specular_reflection(Vec3 outgoing, Vec3 normal, Vec3& incident)
{
	incident = reflect(outgoing, normal);
	float correction = abs_dot(incident, normal);
	if (almost_zero(correction)) return Color();
	return Color(1.0f / correction);
}

Color bsdf_specular_fresnel(Vec3 outgoing, Vec3 normal, Vec3& incident, float eta)
{
	float cos_o = dot(outgoing, normal);
	if (cos_o < 0.0f) eta = 1.0f / eta;

	float cos_i = fresnel_cos_i(eta, cos_o);
	float evaluated = fresnel_value(eta, cos_o, cos_i);

	//Uniform sampling
	//	if (random_float() < 0.5f)
	//	{
	//		//Specular reflection
	//		incident = normalize(reflect(outgoing, normal));
	//	}
	//	else
	//	{
	//		//Specular transmission
	//		evaluated = 1.0f - evaluated;
	//		incident = fresnel_refract(eta, cos_i, outgoing, normal);
	//	}
	//
	//	float correction = abs_dot(incident, normal);
	//	if (almost_zero(correction)) return Color();
	//	return Color(evaluated / correction / 0.5f);

	//Importance sampling
	if (random_float() < evaluated) incident = normalize(reflect(outgoing, normal));
	else incident = fresnel_refract(eta, cos_i, outgoing, normal);

	float correction = abs_dot(incident, normal);
	if (almost_zero(correction)) return Color();
	return Color(1.0f / correction);
}

Color bsdf(uint32_t material, Vec3 outgoing, Vec3 normal, Vec3& incident)
{
	//Cornell box interior scene
	switch (material)
	{
		case 0: return bsdf_lambertian_reflection(outgoing, normal, incident) * 0.8f;
		case 1: return bsdf_lambertian_reflection(outgoing, normal, incident) * Color(1.0f, 0.2f, 0.3f);
		case 2: return bsdf_lambertian_reflection(outgoing, normal, incident) * Color(0.3f, 0.2f, 1.0f);
		case 3: return bsdf_lambertian_reflection(outgoing, normal, incident) * Color(0.2f, 1.0f, 0.3f);
		case 4: return bsdf_specular_reflection(outgoing, normal, incident) * Color(0.2f, 0.3f, 0.9f);
		case 5: return bsdf_specular_fresnel(outgoing, normal, incident, 1.0f / 1.5f) * Color(0.8f, 0.8f, 0.85f);
		default: break;
	}

	return Color();
}

Color emit(uint32_t material)
{
	switch (material)
	{
		case 10: return Color(1.5f);
		case 11: return { 2.5f, 0.2f, 0.2f };
		case 12: return { 0.2f, 2.5f, 0.2f };
		case 13: return { 0.2f, 0.2f, 2.5f };
		default: break;
	}

	return Color();
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
	uint32_t material;

	if (not Scene.intersect(ray, distance, normal, material)) return escape(ray.direction);

	Vec3 outgoing = -ray.direction;
	Vec3 incident;

	Color scatter = bsdf(material, outgoing, normal, incident);
	Color emission = emit(material);

	Ray new_ray = bounce(ray, distance, incident);
	float lambertian = abs_dot(normal, incident);
	return scatter * evaluate(new_ray, depth - 1) * lambertian + emission;
}

/**
 * Evaluates one sample of the rendering equation.
 * This is implemented iteratively rather than recursively for better performance and readability (arguable).
 */
Color evaluate_iterative(Ray ray, uint32_t depth)
{
	Color energy(1.0f);
	Color result(0.0f);

	for (uint32_t i = 0; i < depth; ++i)
	{
		float distance;
		Vec3 normal;
		uint32_t material;

		if (not Scene.intersect(ray, distance, normal, material)) break;

		Vec3 outgoing = -ray.direction;
		Vec3 incident;

		Color scatter = bsdf(material, outgoing, normal, incident);
		Color emission = emit(material);

		ray = bounce(ray, distance, incident);
		float lambertian = abs_dot(normal, incident);

		result = result + emission * energy;
		energy = energy * scatter * lambertian;

		if (almost_black(energy)) break;
	}

	if (almost_black(energy)) return result;
	return result + energy * escape(ray.direction);
}

/**
 * Calculates one Monte Carlo integration sample through the camera.
 */
Color render_sample(float u, float v)
{
	Ray ray;

	//Cornell box interior scene
	ray.origin = Vec3(0.0f, 5.0f, -20.0f);
	ray.direction = normalize(Vec3(u, v, 1.5f));

	return evaluate_iterative(ray, MaxBounces);
}

/**
 * Calculates the color value to output for a pixel.
 */
Color render_pixel(uint32_t x, uint32_t y)
{
	Color result;
	uint32_t count = 0;

	for (uint32_t i = 0; i < SamplesPerPixel; ++i)
	{
		float u = (static_cast<float>(x) + random_float() - ImageWidth / 2.0f) / ImageWidth;
		float v = (static_cast<float>(y) + random_float() - ImageHeight / 2.0f) / ImageWidth;

		Color sample = render_sample(u, v);
		if (is_invalid(sample)) continue;
		result = result + sample;
		++count;
	}

	return result / static_cast<float>(count);
}

int main()
{
	Image image(ImageWidth, ImageHeight);

	parallel_for(0, ImageHeight, [&](uint32_t y)
	{
		for (uint32_t x = 0; x < ImageWidth; ++x)
		{
			Color color = render_pixel(x, y);
			image.set_pixel(x, y, color);
		}
	});

	image.write_file("output.png");
	return 0;
}


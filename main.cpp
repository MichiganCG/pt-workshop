#include "library/library.hpp"

const uint32_t ImageWidth = 512;
const uint32_t ImageHeight = 512;

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


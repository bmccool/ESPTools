#include "Color.h"

// helper function. Clamp float to 0,1, then map to 0-255
uint8_t Color::Color8(float color)
{
    if (color < 0) color = 0;
    if (color > 1) color = 1;
    auto c = 255*color + 0.5f;
    if (c > 255) c = 255;
    return static_cast<uint8_t>(c);
}

// helper function. clamp int to 0-255, then to clost
float Color::ColorF(int color)
{
	if (color < 0) color = 0;
	if (color > 255) color = 255;
	return color / 255.0f;
}

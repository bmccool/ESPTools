#include "Utils.h"

namespace Graphics{
// permute RGB in low bits of color to different order
// 0: RGB->RGB
// 1: RGB->RBG // swap last 2
// 2: RGB->GRB // swap first 2
// 3: RGB->GBR // cycle one to left
// 4: RGB->BRG // cycle one to right
// 5: RGB->BGR // swap 1 and 3
Color PermuteRGB(const Color & color, int permutation)
{
	switch (permutation%6)
	{
		case 1 : return Color(color.red, color.blue, color.green);
		case 2 : return Color(color.green, color.red, color.blue);
		case 3 : return Color(color.green, color.blue, color.red);
		case 4 : return Color(color.blue, color.red, color.green);
		case 5 : return Color(color.blue, color.green, color.red);
	}
	return color;
}


}
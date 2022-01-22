#ifndef GRAPHICS_UTILS_H
#define  GRAPHICS_UTILS_H

#include "Color.h"

namespace Graphics
{

// permute RGB in low bits of color to different order
// 0: RGB->RGB
// 1: RGB->RBG // swap last 2
// 2: RGB->GRB // swap first 2
// 3: RGB->GBR // cycle one to left
// 4: RGB->BRG // cycle one to right
// 5: RGB->BGR // swap 1 and 3
Color PermuteRGB(const Color & color, int permutation);

}

#endif //  GRAPHICS_UTILS_H

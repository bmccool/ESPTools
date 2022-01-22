#ifndef IMAGE_H
#define IMAGE_H

#include "../Standard.h"
#include "Color.h"

// Image on a color type
// is a 3D image, used as a 1D and 2D image as needed
class Image
{
public:
   Image(int width, int height, int depth = 1)
   {
    this->width  = width;
    this->height = height;
    this->depth  = depth;
    buffer.resize(width*height*depth);
   }

   int width{32}, height{32}, depth{1};

	// number of pixels
   size_t NumPixels() { return width*height*depth; }


   void SetPixel(const Color & color, int i, int j, int k = 0)
   {
    if (i < 0 || j < 0 || k < 0 ||
        width <= i || height <= j || depth <= k)
        return;
    auto index = i + width * j + width*height*k;
    buffer[index] = color;

   }

   Color GetPixel(int i, int j, int k = 0) const
   {
    if (i < 0 || j < 0 || k < 0 ||
        width <= i || height <= j || depth <= k)
        return Color(0,0,0);
    auto index = i + width * j + width*height*k;
    return buffer[index];
   }

   void Fill(const Color & c)
   {
        for (auto i = 0; i < width; ++i)
        for (auto j = 0; j < height; ++j)
        for (auto k = 0; k < depth; ++k)
            SetPixel(c,i,j,k);
   }


private:
   std::vector<Color> buffer;
};

using ImagePtr = std::shared_ptr<Image>;

#endif //  IMAGE_H
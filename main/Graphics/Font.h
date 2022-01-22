#ifndef FONT_H
#define FONT_H

#include <string>
#include <functional>

// Parameters: i, j -> 
// Text
// SetPixel -> Function that takes X, Y, and COLOR and returns void.  This draws the pixel
// fontIndex -> 
void DrawText(int i, int j, const std::string & text, const std::function<void(int x, int y, int color)> & setPixel, int fontIndex = -1);

#endif // FONT_H
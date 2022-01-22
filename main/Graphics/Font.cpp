#include <string>
#include <functional>
#include <cstdint>
#include "Font.h"
#include "Font5x7.h"
#include "Font9x16.h"

using namespace std;

// font data: 
//    data stored for each column at a time, low bits in bytes come first
//    bits ordered in column bottom of font to top
typedef struct FontDef
{
    uint8_t firstChar; // inclusive
    uint8_t lastChar;  // inclusive
    uint8_t width;
    uint8_t height;
    const uint8_t * data;
} FontDef;

static const FontDef fonts[] = 
{
    {0x20, 0x7F, 5,  7, Font5x7},
    {0x00, 0xFF, 9, 16, Font9x16}
};

void DrawText(int i, int j, const string & text, const function<void(int x, int y, int color)> & setPixel, int fontIndex)
{
    if (fontIndex < 0) 
        fontIndex = 0;
    auto font = fonts + fontIndex;
    auto num = -1;
    auto w = font->width;
    auto h = font->height;
    auto fst = font->firstChar;
    auto lst = font->lastChar;
    auto data = font->data;
    auto bytesPerHeight = (h+7)/8;
    auto bytesPerChar  = bytesPerHeight*w;

    for (auto c : text)
    {
        ++num; 
        auto ic = (int)c;
        if (ic < fst || lst < ic)
            continue; 
        ic -= fst; // zero index
        for (auto col = 0; col < w; ++col)
        {
            // each col uses bytesPerHeight
            auto ptr = data + ic*bytesPerChar + col*bytesPerHeight;
            auto b = *ptr++;
            for (auto row = 0; row < h; ++row)
            {
                setPixel(i + num*(w+1) + col, j + row, b&1);
                b >>= 1;
                if ((row&7) == 7)
                    b = *ptr++;
            }
        }       
    }
}


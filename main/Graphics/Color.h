#ifndef COLORAA_H
#define COLORAA_H

#include "../STandard.h"

// RGBA color type
class Color
{
public:
    Color(float r = 1, float g = 1, float b = 1, float a = 1) : red(r), green(g), blue(b), alpha(a) 
    {

    }
	
	// create color from components 0-255
	static Color FromRGB8(int red, int green, int blue, int alpha = 255)
	{
		return Color(ColorF(red), ColorF(green), ColorF(blue), ColorF(alpha));
	}

    
    float red;
    float green;
    float blue;
    float alpha;

    uint8_t red8() { return Color8(red);}
    uint8_t green8() {return Color8(green);}
    uint8_t blue8() {return Color8(blue);}
    uint8_t alpha8() {return Color8(alpha);}
    // A,R,G,B high bits to low bits, 8 bits each
    uint32_t argb() 
    {
        return (alpha8()<<24)|(red8()<<16)|(green8()<<8)|blue8();
    }
    
    // R,G,B high bits to low bits
    uint32_t rgb() 
    {
        return (red8()<<16)|(green8()<<8)|blue8();
    }

    // helper function. Clamp float to 0,1, then map to 0-255
    static uint8_t Color8(float color);
	
	// helper function. clamp int to 0-255, then to clost
	static float ColorF(int color);
	

};


#endif // COLORAA_H

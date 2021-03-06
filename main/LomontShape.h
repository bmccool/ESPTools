#ifndef LOMONT_SHAPE_H
#define LOMONT_SHAPE_H

#include <vector> // std::vector
#include <list> // std::list
#include <cmath>

class LomontShape{
    public:
        std::vector<float> points;
        std::vector<int> faces;
        int type;

        void SetPoints(std::list<float> NewPoints){
            for(float const &point: NewPoints){
                points.push_back(point);
            }
        }
        void SetFaces(std::list<int> NewFaces){
            for(int const &face: NewFaces){
                faces.push_back(face);
            }
        }

    LomontShape(int type): type(type){
		float s = 0.0; // side length
		switch (type)
		{
		case 0: // tetrahedron
			//SetPoints({ 1,1,1,-1,-1,1,1,-1,-1,-1,1,-1 });
			//SetFaces({ 3,0,2,1, 3,0,3,2, 3,1,2,3, 3,0,1,3 });
			
			SetPoints({ 0., 0., 0.612372, -0.288675, -0.5, -0.204124, -0.288675, 0.5, - 0.204124, 0.57735, 0., -0.204124 });
			SetFaces({ 3, 1, 2, 3, 3, 2, 1, 0, 3, 3, 0, 1, 3, 0, 3, 2 });			
			break;
		case 1 : // cube
			s = 0.5;
			//SetPoints({ 1,1,1,1,-1,1,-1,-1,1,-1,1,1,1,1,-1,1,-1,-1,-1,-1,-1,-1,1,-1 });
			//SetFaces({ 4, 0,1,2,3, 4, 4,7,6,5, 4, 0,4,5,1, 4, 1,5,6,2, 4, 3,2,6,7, 4, 0,3,7,4 });
			SetPoints({ -s, -s, -s, -s, -s, s, -s, s, -s, -s, s,  s, s, -s, -s, s, -s, s, s, s, -s, s, s, s});
			SetFaces({ 4, 7, 3, 1, 5, 4, 7, 5, 4, 6, 4, 7, 6, 2, 3, 4, 3, 2, 0, 1, 4, 0, 2, 6, 4, 4, 1, 0, 4, 5 });
			break;
		case 2 : // octahedron
			s = 1.0f/std::sqrt(2);
			//SetPoints({ 0,0,1,1,0,0,0,-1,0,-1,0,0,0,1,0,0,0,-1 });
			//SetFaces({ 3, 0,1,2, 3, 0,2,3, 3, 0,3,4, 3, 0,4,1, 3, 5,1,4, 3, 5,4,3, 3, 5,3,2, 3, 5,2,1 });
			SetPoints({ -s, 0, 0, 0, s, 0, 0, 0, -s, 0, 0, s, 0, -s, 0, s, 0, 0 });
			SetFaces({ 3, 3, 4, 5, 3, 3, 5, 1, 3, 3, 1, 0, 3, 3, 0, 4, 3, 4, 0, 2, 3, 4, 2, 5, 3, 2, 0, 1, 3, 5, 2, 1 });
			break;
		case 3: // dodecahedron
			SetPoints({ -1.37638f, 0.f, 0.262866f, 1.37638f, 0.f, -0.262866f, -0.425325f, -1.30902f, 0.262866f, -0.425325f, 1.30902f, 0.262866f, 1.11352f, -0.809017f, 0.262866f, 1.11352f, 0.809017f, 0.262866f, -0.262866f, -0.809017f, 1.11352f, - 0.262866f, 0.809017f, 1.11352f, -0.688191f, -0.5f, -1.11352f, -0.688191f, 0.5f, -1.11352f, 0.688191f, -0.5f, 1.11352f, 0.688191f, 0.5f, 1.11352f, 0.850651f, 0.f, -1.11352f, -1.11352f, -0.809017f, -0.262866f, -1.11352f, 0.809017f, -0.262866f, -0.850651f, 0.f, 1.11352f, 0.262866f, -0.809017f, - 1.11352f, 0.262866f, 0.809017f, -1.11352f, 0.425325f, -1.30902f, - 0.262866f, 0.425325f, 1.30902f, -0.262866f });
			SetFaces({ 5, 14, 9, 8, 13, 0, 5, 1, 5, 11, 10, 4, 5, 4, 10, 6, 2, 18, 5, 10, 11, 7, 15, 6, 5, 11, 5, 19, 3, 7, 5, 5, 1, 12, 17, 19, 5, 1, 4, 18, 16, 12, 5, 3, 19, 17, 9, 14, 5, 17, 12, 16, 8, 9, 5, 16, 18, 2, 13, 8, 5, 2, 6, 15, 0, 13, 5, 15, 7, 3, 14, 0 });
			break;
		case 4: // icosahedron
			SetPoints({0.f, 0.f, -0.951057f, 0.f, 0.f, 0.951057f, -0.850651f, 0.f, -0.425325f, 0.850651f, 0.f, 0.425325f, 0.688191f, -0.5f, -0.425325f, 0.688191f, 0.5f, - 0.425325f, -0.688191f, -0.5f, 0.425325f, -0.688191f, 0.5f, 0.425325f, - 0.262866f, -0.809017f, -0.425325f, -0.262866f, 0.809017f, -0.425325f, 0.262866f, -0.809017f, 0.425325f, 0.262866f, 0.809017f, 0.425325f			});
			SetFaces({ 3, 1, 11, 7, 3, 1, 7, 6, 3, 1, 6, 10, 3, 1, 10, 3, 3, 1, 3, 11, 3, 4, 8, 0, 3, 5, 4, 0, 3, 9, 5, 0, 3, 2, 9, 0, 3, 8, 2, 0, 3, 11, 9, 7, 3, 7, 2, 6, 3, 6, 8, 10, 3, 10, 4, 3, 3, 3, 5, 11, 3, 4, 10, 8, 3, 5, 3, 4, 3, 9, 11, 5, 3, 2, 7, 9, 3, 8, 6, 2 });
			break;
		case 5 : // spikey
			SetPoints({0.f, 0.f, -0.951057f, 0.f, 0.f, 0.951057f, -0.850651f, 0.f, -0.425325f, 0.850651f, 0.f, 0.425325f, -0.688191f, -0.5f, 0.425325f, -0.688191f, 0.5f, 0.425325f, 0.688191f, -0.5f, -0.425325f, 0.688191f, 0.5f, -0.425325f, - 0.262866f, -0.809017f, -0.425325f, -0.262866f, 0.809017f, -0.425325f, 0.262866f, -0.809017f, 0.425325f, 0.262866f, 0.809017f, 0.425325f, - 1.54435f, 0.f, 0.294944f, 1.54435f, 0.f, -0.294944f, -0.954458f, 0.f, 1.2494f, 0.954458f, 0.f, -1.2494f, -1.2494f, 0.907744f, -0.294944f, -1.2494f, - 0.907744f, -0.294944f, 1.2494f, 0.907744f, 0.294944f, 1.2494f, -0.907744f, 0.294944f, -0.294944f, 0.907744f, 1.2494f, -0.294944f, -0.907744f, 1.2494f, 0.294944f, 0.907744f, -1.2494f, 0.294944f, -0.907744f, -1.2494f, -0.772173f, 0.561016f, -1.2494f, -0.772173f, -0.561016f, -1.2494f, 0.772173f, 0.561016f, 1.2494f, 0.772173f, -0.561016f, 1.2494f, -0.477229f, 1.46876f, 0.294944f, - 0.477229f, -1.46876f, 0.294944f, 0.477229f, 1.46876f, -0.294944f, 0.477229f, -1.46876f, -0.294944f			});
			SetFaces({3, 20, 1, 11, 3, 20, 11, 5, 3, 20, 5, 1, 3, 14, 1, 5, 3, 14, 5, 4, 3, 14, 4, 1, 3, 21, 1, 4, 3, 21, 4, 10, 3, 21, 10, 1, 3, 27, 1, 10, 3, 27, 10, 3, 3, 27, 3, 1, 3, 26, 1, 3, 3, 26, 3, 11, 3, 26, 11, 1, 3, 23, 6, 8, 3, 23, 8, 0, 3, 23, 0, 6, 3, 15, 7, 6, 3, 15, 6, 0, 3, 15, 0, 7, 3, 22, 9, 7, 3, 22, 7, 0, 3, 22, 0, 9, 3, 24, 2, 9, 3, 24, 9, 0, 3, 24, 0, 2, 3, 25, 8, 2, 3, 25, 2, 0, 3, 25, 0, 8, 3, 28, 11, 9, 3, 28, 9, 5, 3, 28, 5, 11, 3, 12, 5, 2, 3, 12, 2, 4, 3, 12, 4, 5, 3, 29, 4, 8, 3, 29, 8, 10, 3, 29, 10, 4, 3, 19, 10, 6, 3, 19, 6, 3, 3, 19, 3, 10, 3, 18, 3, 7, 3, 18, 7, 11, 3, 18, 11, 3, 3, 31, 6, 10, 3, 31, 10, 8, 3, 31, 8, 6, 3, 13, 7, 3, 3, 13, 3, 6, 3, 13, 6, 7, 3, 30, 9, 11, 3, 30, 11, 7, 3, 30, 7, 9, 3, 16, 2, 5, 3, 16, 5, 9, 3, 16, 9, 2, 3, 17, 8, 4, 3, 17, 4, 2, 3, 17, 2, 8			});
			break;
		}

        /*
		// scale into unit radius sphere
		auto max = 0.0f;
		for (auto & v : points)
		{
			auto d = v.length();
			max = std::max(d, max);			
		}
		max = max * 0.6; // nicer scaling
		for (auto& v : points)
		{
			v.x /= max;
			v.y /= max;
			v.z /= max;
		}
        */
    }
}; // LomontShape

#endif //LOMONT_SHAPE_H
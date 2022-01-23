#include "3d_stuff.h"
#include "Graphics/Font.h"
#include <chrono> // function timing/metrics
#include <iostream> // cin, cout
#include <cstdlib> // Rand
#include <string>

#define PI 3.14159265

void cube_demo(void){
    clear_buffer(screen_buffer);
    draw_buffer(screen_buffer);

    float angle = 0;
    RotationMatrixZ rotate_z(angle);
    rotate_z.update();
    std::vector<std::vector<float>> orthogonal_projection_matrix = {
        {1, 0, 0},
        {0, 1, 0},
    };

    Point3 points[4];
    points[0].set(20, 20, 0);
    points[1].set(40, 20, 0);
    points[2].set(40, 40, 0);
    points[3].set(20, 40, 0);

    Point projected_point;
    Point3 rotated_point;
    Matrix<float> projected_matrix(2, 1);

    while(true){
        for(int i = 0; i < 4; i++){
            // Rotate the point
            rotated_point = rotate_point(rotate_z, points[i]); // TODO rotate should be a method of a rotation matrix

            // Project the point
            projected_matrix = m2x3by3x1(orthogonal_projection_matrix, rotated_point.to_matrix());
            projected_point.set(projected_matrix.elements[0][0], projected_matrix.elements[1][0]);
            shade_px(screen_buffer, SHADE_SOLID, projected_point.getx(), projected_point.gety());
        }
        draw_buffer(screen_buffer);
        angle = angle + 0.01;
        rotate_z.angle = angle;
        rotate_z.update();
    }
}

void demo_rotate_box_z(void){
    clear_buffer(screen_buffer);
    draw_buffer(screen_buffer);
    Sprite box;
    
    box.create_point( 20,  20, 0);
    box.create_point( 20, -20, 0);
    box.create_point(-20,  20, 0);
    box.create_point(-20, -20, 0);

    box.create_point(  4,  20, 0);
    box.create_point(  4, -20, 0);
    box.create_point( -4,  20, 0);
    box.create_point( -4, -20, 0);    

    box.create_point( 20,   0, 0);
    box.create_point(-20,   0, 0);
    box.create_point( 20,   4, 0);
    box.create_point(-20,   4, 0);
    box.create_point( 20,  -4, 0);
    box.create_point(-20,  -4, 0);
    
    //box.create_point( 10, 0, 0);

    std::vector<std::vector<float>> orthogonal_projection_matrix = {
        {1, 0, 0},
        {0, 1, 0},
    };    

    while(true){
        clear_buffer(screen_buffer);
        //draw_buffer(screen_buffer);

        box.render(orthogonal_projection_matrix, screen_buffer);
        draw_buffer(screen_buffer);
        
        box.angle_z = box.angle_z + (5 * PI / 180);
        box.angle_y = box.angle_y + (10 * PI / 180);
        box.angle_x = box.angle_x + (2 * PI / 180);
    }
}

void demo_rotate_cube(uint8_t* buffer){
    clear_buffer(buffer);
    draw_buffer(buffer);
    Sprite cube;
       
    // X Lines (4)
    cube.create_line(-20, -20, -20,  20, -20, -20);
    cube.create_line(-20,  20, -20,  20,  20, -20);
    cube.create_line(-20, -20,  20,  20, -20,  20);
    cube.create_line(-20,  20,  20,  20,  20,  20);

    // Y Lines (4)
    cube.create_line(-20, -20, -20, -20,  20, -20);
    cube.create_line( 20, -20, -20,  20,  20, -20);
    cube.create_line(-20, -20,  20, -20,  20,  20);
    cube.create_line( 20, -20,  20,  20,  20,  20);

    // Z Lines
    cube.create_line(-20, -20, -20, -20, -20,  20);
    cube.create_line(-20,  20, -20, -20,  20,  20);
    cube.create_line( 20, -20, -20,  20, -20,  20);
    cube.create_line( 20,  20, -20,  20,  20,  20);


    std::vector<std::vector<float>> orthogonal_projection_matrix = {
        {1, 0, 0},
        {0, 1, 0},
    };    

    while(true){
        clear_buffer(buffer);
        //draw_buffer(screen_buffer);

        cube.render(orthogonal_projection_matrix, buffer);
        draw_buffer(buffer);

        cube.angle_z = cube.angle_z + (5 * PI / 180);
        cube.angle_y = cube.angle_y + (10 * PI / 180);
        cube.angle_x = cube.angle_x + (2 * PI / 180);             
    }
}

void demo_lomont_shapes(uint8_t* buffer){
    clear_buffer(buffer);
    draw_buffer(buffer);
    Sprite shape = get_lomont_shape(5);
    
    std::vector<std::vector<float>> orthogonal_projection_matrix = {
        {1, 0, 0},
        {0, 1, 0},
    };    

    while(true){
        clear_buffer(buffer);
        shape.render(orthogonal_projection_matrix, buffer);
        draw_buffer(buffer);

        shape.angle_x = shape.angle_x + (2 * PI / 180);
        shape.angle_y = shape.angle_y + (10 * PI / 180);
        shape.angle_z = shape.angle_z + (5 * PI / 180);
    }
}

void demo_rotate_shaded_cubes(uint8_t* buffer){
    clear_buffer(buffer);
    draw_buffer(buffer);
    Cube cube20(20, Point3(32, 32, 0));
    Cube cube10(10, Point3(96, 32, 0));
    
    std::vector<std::vector<float>> orthogonal_projection_matrix = {
        {1, 0, 0},
        {0, 1, 0},
    };    

    //cube.origin = Point3(64, 64, 0);

    while(true){
        //std::cout << cube.angle_x << " " << cube.angle_y << " " << cube.angle_z << std::endl;
        clear_buffer(buffer);
        cube20.render(orthogonal_projection_matrix, buffer);
        cube10.render(orthogonal_projection_matrix, buffer);
        draw_buffer(buffer);

        cube20.angle_x = cube20.angle_x + (2 * PI / 180);
        cube20.angle_y = cube20.angle_y + (10 * PI / 180);
        cube20.angle_z = cube20.angle_z + (5 * PI / 180);
        cube10.angle_x = cube10.angle_x + (1 * PI / 180);
        cube10.angle_y = cube10.angle_y + (5 * PI / 180);
        cube10.angle_z = cube10.angle_z + (5 * PI / 180);        
    }
}

void demo_seizure_warning(uint8_t* buffer){
    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    float fps;
    std::cout << duration.count() << std::endl;

    while(true){

        fill_buffer(buffer);
        start = std::chrono::high_resolution_clock::now();
        draw_buffer(buffer);
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        fps = 1.0 / ((float)duration.count() / (1000000)); // Cast to seconds
        std::cout << "FPS: " << fps << std::endl;
        
        clear_buffer(buffer);
        start = std::chrono::high_resolution_clock::now();
        draw_buffer(buffer);
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        fps = 1.0 / ((float)duration.count() / (1000000)); // Cast to seconds
        std::cout << "FPS: " << fps << std::endl;
    }
}

void demo_text_hello_world(){
    std::string my_text = "Hello World!";
    DrawText(10, 20, my_text, &my_setPixel, 1);
    draw_buffer(screen_buffer);
}

void demo_poles(uint8_t* buffer, int num_poles = 1){
    //#define FRAME_Y_RESOLUTION 64
    //#define FRAME_X_RESOLUTION 128

    // Initialize pole speed to random unit vector
    Vec3 speed = Vec3::random(1);
    speed.to_unit();

    // Create pole, set speed
    Pole p1(64.8014, 33.0442, 1.50572);
    p1.speed = speed;

    System system;
    system.poles.push_back(p1);

    while(true){
        clear_buffer(buffer);
        shade_px(buffer, SHADE_SOLID, system.poles[0].location.x, system.poles[0].location.y);
        //std::string coordinate = "(" + std::to_string((int)location.x) + ", " + std::to_string((int)location.y) + ")";
        //DrawText(0, 0, coordinate, &my_setPixel, 1);

        //draw_rings(buffer, poles);
        //std::cout << system << std::endl;
        //vTaskDelay(100 / portTICK_PERIOD_MS);
        system.draw_rings(buffer);
        draw_buffer(buffer);
        for(auto &p : system.poles){
            p = p + p.speed; // TODO Speed needs to be a function of Poles.  In fact this whole update should be a function of poles...
            if(p.location.x < 0) {
                p.speed.x = p.speed.x * -1;
                p.location.x = 0;
            } else if(p.location.x >= FRAME_X_RESOLUTION){
                p.speed.x = p.speed.x * -1;
                p.location.x = FRAME_X_RESOLUTION - 1;
            }
            if(p.location.y < 0){
                p.speed.y = p.speed.y * -1;
                p.location.y = 0;
            } else if(p.location.y >= FRAME_Y_RESOLUTION){
                p.speed.y = p.speed.y * -1;
                p.location.y = FRAME_Y_RESOLUTION - 1;
            }
        }
    }
}
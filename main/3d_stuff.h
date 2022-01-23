#ifndef MY_3D_STUFF_H
#define MY_3D_STUFF_H

#include <vector> // std::vector
#include <math.h> // Pow
#include <cmath> // round()
#include <iostream> // std::cout and such
#include "esp_system.h" // esp_err_ti

#include "LomontShape.h"

// TODO fix this include
#define FRAME_Y_RESOLUTION 64
#define FRAME_X_RESOLUTION 128

// I2C DEFINITIONS
#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          1000000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

// SSD1306 DEFINITIONS

// Set the SSD1306 internal command registers as per
// https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf

// If I2C address is unspecified, use default
// (0x3C for 128x32 displays, 0x3D for all others).
// BAM: Using a 128x64 display, so 0x3D!
// EXCEPT!  3D gives nak, 3C gives ack, so, 3C must be correct???
#define SSD1306_I2C_ADDR 0x3C
#define FRAME_Y_RESOLUTION 64
#define FRAME_X_RESOLUTION 128


template <class T = uint8_t> // Default element type is uint8_t
class Matrix {
    private:
        unsigned int rows, columns;
        

    public:
        std::vector<std::vector<T>> elements;

        // Initialize with a vector of vectors
        Matrix(std::vector<std::vector<T>> elements): rows(elements.size()), columns(elements[0].size()), elements(elements){}

        Matrix(unsigned int rows, unsigned int columns): rows(rows), columns(columns), elements(std::vector<std::vector<T>> (rows, std::vector<T>(columns, 0))){}
        // Initialize (to empty) with size of (row, col)
        //Matrix(int rows, int columns): 
        unsigned int getcolumns(){
            return columns;
        }

        unsigned int getrows(){
            return rows;
        }

        void print(void){
            printf("Size %d x %d\n", getrows(), getcolumns());
            printf("===============================\n");
            for(int i = 0; i < getrows(); i++){
                for(int j = 0; j < getcolumns(); j++){
                    printf("%f ", elements[i][j]); //TODO this is gonna complain if our template type isn't float...
                }
                printf("\n");
            }
            printf("===============================\n");
        }
};


Matrix<float> m2x3by3x1(Matrix<float> m1, Matrix<float> m2){
    Matrix<float> retval(2, 1);
    if((m1.getcolumns() != 3) || (m2.getrows() != 3)){
        printf("Columns of first matrix must match rows of second!\n");
        return retval;
    }
    retval.elements[0][0] = ((m1.elements[0][0] * m2.elements[0][0]) +
                             (m1.elements[0][1] * m2.elements[0][0]) +
                             (m1.elements[0][2] * m2.elements[0][0]));
    retval.elements[1][0] = ((m1.elements[1][0] * m2.elements[1][0]) +
                             (m1.elements[1][1] * m2.elements[1][0]) +
                             (m1.elements[1][2] * m2.elements[1][0]));
    return retval;
}

class Point {
    private:
        int x, y; // TODO Should this be a standardtype?

    public:
        Point(int x0 = 0, int y0 = 0){
            set(x0, y0);
        }
        
        void print(void){
            printf("X: %d, Y: %d\n", getx(), gety());
        }        

        void set(int x0, int y0){
            x = x0;
            y = y0;
        }

        int getx(){
            return x;
        }

        int gety(){
            return y;
        }        
};

class Point3 { //TODO could this inherit from Point?
    public:
        float x, y, z;
        Point3(float x0 = 0, float y0 = 0, float z0 = 0){
            set(x0, y0, z0);
        }
        

        void set(float x0, float y0, float z0){
            x = x0;
            y = y0;
            z = z0;
        }

        float getx(){
            return x;
        }

        float gety(){
            return y;
        }

        float getz(){
            return z;
        }

        std::vector<std::vector<float>> to_matrix(){
            std::vector<std::vector<float>> retval = {{x},
                                                        {y},
                                                        {z}};
            return retval;
        }  

        void print(void){
            printf("X: %f, Y: %f Z: %f\n", getx(), gety(), getz());
        }

        Point3 operator + (Point3 p){
            return Point3(x + p.x, y + p.y, z + p.z);
        }

        Point3 operator - (Point3 p){
            return Point3(x - p.x, y - p.y, z - p.z);
        }        

        Point3 operator * (Point3 p){ // Cross Product
            return Point3(((y * p.z) - (z * p.y)), 
                          ((z * p.x) - (x * p.z)),
                          ((x * p.y) - (y * p.x)));
        }

        bool operator== (Point3 rhs) const{ // Equality Test
            if((x == rhs.x) && (y == rhs.y) && (z == rhs.z)){ 
                return true;
            }
            return false;
        }
        bool operator!= (Point3 rhs) const{ // Inequality Test
            if((x != rhs.x) && (y != rhs.y) && (z != rhs.z)){ 
                return true;
            }
            return false;
        }

        Point3 scale(float factor){
            return Point3(x * factor, y * factor, z * factor);
        }

        Point to_2d(void){
            return Point(x, y);
        }

        float length(void){
            return std::sqrt((x * x) + (y * y) + (z * z));
        }

        void round(){
            x = std::round(x);
            y = std::round(y);
            z = std::round(z);
        }

        friend std::ostream& operator<<(std::ostream& os, const Point3& p);
};

std::ostream& operator<<(std::ostream& os, const Point3& p)
{
    os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
    return os;
}

class Vec3 : public Point3{
    public:
        Vec3(float x0 = 0, float y0 = 0, float z0 = 0): Point3(x0, y0, z0){};
        Vec3(Point3 p1): Point3(p1.x, p1.y, p1.z){}; 
        
        static Vec3 random(int seed = 1){
            std::srand(seed);
            return Vec3(std::rand()%100, std::rand()%100, std::rand()%100);
        }

        float magnitude(){
            return length();
        }

        void to_unit(){
            float m = magnitude();
            x = x/m;
            y = y/m;
            z = z/m;
        }

        void scale(float factor){
            x *= factor;
            y *= factor;
            z *= factor;
        }

        Vec3 operator - (){
            return Vec3(-x, -y, -z);
        }           

}; // Vec3

class RotationMatrix3 : public Matrix<float>{
    public:
        float angle;
        RotationMatrix3(float angle = 0): Matrix<float>(3, 3), angle(angle){}

        virtual void update(void){
            std::cout << "Update the rotation matrix based on the angle." << std::endl;
        }
}; // RotationMatrix3

class RotationMatrixZ : public RotationMatrix3{
    public:
        RotationMatrixZ(float angle) : RotationMatrix3(angle){update();};
        void update(void){
            elements[0][0] =  cos(angle);
            elements[0][1] = -sin(angle);
            elements[0][2] =  0;

            elements[1][0] =  sin(angle);
            elements[1][1] =  cos(angle);
            elements[1][2] =  0;

            elements[2][0] =  0;
            elements[2][1] =  0;
            elements[2][2] =  1;
        }
};

class RotationMatrixY : public RotationMatrix3{
    public:
        RotationMatrixY(float angle) : RotationMatrix3(angle){update();};    
        void update(void){
            elements[0][0] =  cos(angle);
            elements[0][1] =  0;
            elements[0][2] =  sin(angle);

            elements[1][0] =  0;
            elements[1][1] =  1;
            elements[1][2] =  0;

            elements[2][0] = -sin(angle);
            elements[2][1] =  0;
            elements[2][2] =  cos(angle);
        }
};

class RotationMatrixX : public RotationMatrix3{
    public:
        RotationMatrixX(float angle) : RotationMatrix3(angle){update();};    
        void update(void){
            elements[0][0] =  1;
            elements[0][1] =  0;
            elements[0][2] =  0;

            elements[1][0] =  0;
            elements[1][1] =  cos(angle);
            elements[1][2] = -sin(angle);

            elements[2][0] =  0;
            elements[2][1] =  sin(angle);
            elements[2][2] =  cos(angle);
        }
};

class RotationMatrix2{
    // Rotation Matrix will always be 2x2
    // TODO get a good templateable base matrix, PLEASE
    private:
        float angle_x;

    public:
        std::vector<std::vector<float>> elements;
        RotationMatrix2(float x = 0): angle_x(x){update();}

        void update(void){
            elements[0][0] =  cos(angle_x);
            elements[0][1] = -sin(angle_x);
            elements[1][0] =  sin(angle_x);
            elements[1][1] =  cos(angle_x);
        }
}; // RotationMatrix2

#define MAX_SHADES 17
#define SHADE_SOLID 16
#define SHADE_EMPTY 0
static const uint8_t shade8x8[17*8]= // 17 shade patterns 8x8 pixels
    {
      0,  0,  0,  0,  0,  0,  0,  0, // 0
     17,  0,  0,  0, 17,  0,  0,  0,
     17,  0, 68,  0, 17,  0, 68,  0,
     68, 17, 68,  0, 68, 17, 68,  0,
     85,  0,170,  0, 85,  0,170,  0,
     68,170,  0,170, 68,170,  0,170,
     68,170, 17,170, 68,170, 17,170,
     85,136, 85,170, 85,136, 85,170,
     85,170, 85,170, 85,170, 85,170,
     85,238, 85,170,119,170, 85,170,
    221,170,119,170,221,170,119,170,
    221,170,255,170,221,170,255,170,
     85,255,170,255, 85,255,170,255,
    221,119,221,255,221,119,221,255,
    119,255,221,255,119,255,221,255,
    119,255,255,255,119,255,255,255,
    255,255,255,255,255,255,255,255  // 16
    };

// 8 pages, 8 bytes tall, 128 px wide
static uint8_t screen_buffer[(128 * 8)];
#define I2C_BUFFER_SIZE ((128 * 8) + 1)
static uint8_t i2c_buffer[I2C_BUFFER_SIZE];

static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = 11;
    conf.scl_io_num = 10;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void oled_init(void){
    uint8_t data[3];
    // Display Off
    data[0] = 0x00;
    data[1] = 0xAE;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Osc Frequency D5h, 80h
    // Set Display Clock Divide Ratio/Oscillator Frequency
    // Lower 4 bits define divide ration D of the display clocks (DCLK) - default is 0000, but it's + 1 (cant have 0) so 1
    // Upper 4 bits set oscillator frequency fosc.  Default is 1000b 
    data[0] = 0x00;
    data[1] = 0xD5;
    data[2] = 0x80;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set MUX Ratio A8h, 3Fh 
    // BAM TODO: Something to do with frame refresh?
    data[0] = 0x00;
    data[1] = 0xA8;
    data[2] = 0x3F;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Display Offset D3h, 00h 
    // BAM TODO: Set veritical shift, what?  00 is set after reset, so this is just ensuring default value?
    data[0] = 0x00;
    data[1] = 0xD3;
    data[2] = 0x00;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Display Start Line 40h
    data[0] = 0x00;
    data[1] = 0x40;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Enable charge pump regulator 8Dh, 14h
    // 14 enables charge pump during displayu on
    data[0] = 0x00;
    data[1] = 0x8D;
    data[2] = 0x14;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Memory Mode
    // 00 HORIZONTAL ADDRESSING MODE - Page addressing mode is 02 and default, but this follows adafruit
    data[0] = 0x00;
    data[1] = 0x20; 
    data[2] = 0x00;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));    

    // Set Segment re-map A0h/A1h
    // A0 means column address 0 is mapped to SEG0 BAM TODO what???
    data[0] = 0x00;
    data[1] = 0xA1;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set COM Output Scan Direction C0h/C8h 
    // BAM: C0 is normal (default) COM0 -> COM[N-1], C8 is remapped, COM[N-1] -> COM0.  N is multiplex ratio
    data[0] = 0x00;
    data[1] = 0xC8;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set COM Pins hardware configuration DAh, 02
    data[0] = 0x00;
    data[1] = 0xDA; 
    data[2] = 0x12;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Contrast Control 81h, 7Fh
    // Set [1-256] contrast steps.  Default is 7F
    data[0] = 0x00;
    data[1] = 0x81;
    data[2] = 0xCF;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Pre-charge Period D9 F1 
    data[0] = 0x00;
    data[1] = 0xD9;
    data[2] = 0xF1;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));    

    // Set VCOMH Deselect Level DB 40
    data[0] = 0x00;
    data[1] = 0xDB;
    data[2] = 0x40;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));        

    // Entire Display On (resume) A4h
    data[0] = 0x00;
    data[1] = 0xA4;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Normal Display A6h
    // BAM: As opposed to INVERSE display
    data[0] = 0x00;
    data[1] = 0xA6;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Deactivate Scroll
    data[0] = 0x00;
    data[1] = 0x2E;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));    

    // Display On AFh
    data[0] = 0x00;
    data[1] = 0xAF;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Page Address 22 00 FF
    data[0] = 0x00;
    data[1] = 0x22;
    data[2] = 0x00;
    data[3] = 0x07;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 4, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Column Address 21 0 7 TODO 3rd arg is set to WIDTH - 1, that should be 128-1=127 => 0x7F
    data[0] = 0x00;
    data[1] = 0x21;
    data[2] = 0x00;
    data[3] = 0x7F;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 4, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));    

}

uint16_t coord_to_frame(uint8_t x, uint8_t y){
    // The OLED screen is divieded into 8 pages stacked vertically,
    // each is 8 px tall and 128 px wide.  They are ordered top to bottom,
    // left to right within a page, then top to bottom by page
    // Coordinates are given (x increasing to right, y increasing to bottom)
    // Return a pixel index that corresponds to the frame ordering of the pixel indicated
    // by the coordinates.
    uint16_t page = y / 8;
    uint16_t page_row = y % 8;
    uint16_t column = x;
    return ((page * (128 * 8)) + (column * 8) + (page_row));
}

void write_px_to_buffer(uint16_t px, uint8_t* buffer){
    uint16_t byte_index = px / 8;
    uint8_t bit = px % 8;
    buffer[byte_index] = buffer[byte_index] | (0x01 << bit);
}

bool coord_is_drawable(int x, int y){
    if((x > FRAME_X_RESOLUTION - 1) ||
       (x < 0) ||
       (y > FRAME_Y_RESOLUTION - 1) ||
       (y < 0)){
           return false;
       }
    return true;
}

void shade_px(uint8_t* buffer, uint8_t shade, int x, int y){
    if (shade >= MAX_SHADES) shade = MAX_SHADES - 1; // TODO this is clumsy, fix MAX SHADES so we don't need the -1
    uint8_t shade_x = x % 8;
    uint8_t shade_y = y % 8;
    bool value = ((shade8x8[((shade * 8) + shade_x)]) >> shade_y) & 0x01;

    // Write the pixel to the buffer if we should, else leave it blank
    if(coord_is_drawable(x, y)){
        if (value) write_px_to_buffer(coord_to_frame(x, y), buffer);
    }
}

void shade_demo(uint8_t* buffer){
    // Shade left to right
    uint8_t shade;
    for (int x = 0; x < 128; x++){
        for (int y = 0; y < (8 * 8); y++){
            shade = (x * (MAX_SHADES - 1)) / 128; // 128 is max x value
            //printf("Shading (%d, %d) with %d value...\n", x, y, shade);
            shade_px(buffer, shade, x, y);
        }
    }
}

void clear_buffer(uint8_t* buffer){
    for(int i = 0; i < (128 * 8); i++){
        buffer[i] = 0;
    }
}

void fill_buffer(uint8_t* buffer){
    for(int i = 0; i < (128 * 8); i++){
        buffer[i] = 255;
    }
}

void draw_buffer(uint8_t* buffer){
    i2c_buffer[0] = 0x40;
    for(int i = 1; i < I2C_BUFFER_SIZE; i++){
        i2c_buffer[i] = buffer[i-1];
    }    
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, i2c_buffer, I2C_BUFFER_SIZE, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
}

void dump_buffer(uint8_t* buffer){
    for(int i = 0; i < (128 * 8); i++){
        printf("%d, ", buffer[i]);
        if ((i % 127) == 0) printf("\n===================\n");
    }
}

void oled_spiral(int size){

    // 8 pages, 8 bytes tall, 96 px wide
    static uint8_t screen_buffer[(96 * 8)];
    int x = 0;
    int y = 0;
    int last_x = -1;
    int last_y = 0;
    int filled = 0;
    int left = 0;
    int right = 95;
    int bottom = 63;
    int top = 0;

    while(filled < size){

        filled++;
        printf("print to buffer (%d, %d)\n", x, y);
        write_px_to_buffer(coord_to_frame(x, y), screen_buffer);

        // Get next pointer location
        if(last_x < x){
            last_x = x;
            x++;
        }
        else if (last_x > x){
            last_x = x;
            x--;
        }
        else if (last_y < y){
            last_y = y;
            y++;
        }
        else if (last_y > y){
            last_y = y;
            y--;
        }

        // Check we're not out of bounds
        if (x > right){ // Go down
            x--;
            y--;
        }
        if (x < left){ // Go up
            x++;
            y++;
        }
        if (y > top){
            y--;
            x++;
        }
        if (y < bottom){
            y++;
            x--;
        }
                    
    }

    uint8_t data[2];
    for(int i = 0; i < (96 * 8); i++){
        data[0] = 0x40;
        data[1] = screen_buffer[i];
        ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
    }
    
}

void draw_shaded_line_to_buffer(Point p0, Point p1, uint8_t* buffer, uint8_t weight){
    float slope;
    if(std::abs(p1.gety() - p0.gety()) < std::abs(p1.getx() - p0.getx())){ // Line is longer in horizontal direction
        float y = p0.gety();
        slope = ((float)(p1.gety() - p0.gety()) / (p1.getx() - p0.getx()));
        if(p0.getx() < p1.getx()){
            for(int x = p0.getx(); x < p1.getx(); x++){
                shade_px(buffer, weight, x, (int)y);
                y += slope;
            }
        } else {
            for(int x = p0.getx(); x > p1.getx(); x--){
                shade_px(buffer, weight, x, (int)y);
                y -= slope;            
            }
        }
    }
    else { // Line is 45 degrees or longer in vertical direction
        float x = p0.getx();
        slope = ((float)(p1.getx() - p0.getx()) / (p1.gety() - p0.gety()));        
        if(p0.gety() < p1.gety()){
            for(int y = p0.gety(); y < p1.gety(); y++){
                shade_px(buffer, weight, (int)x, y);
                x += slope;
            }
        } else {
            for(int y = p0.gety(); y > p1.gety(); y--){
                shade_px(buffer, weight, (int)x, y);
                x -= slope;
            }
        }
    }
}

void draw_solid_line_to_buffer(Point p0, Point p1, uint8_t* buffer){
    draw_shaded_line_to_buffer(p0, p1, buffer, SHADE_SOLID);
}

void buffer_wrapper(uint8_t* buffer, int index, int value){
    //printf("buffer[%d] = %d\n", index, value);
    //if(index == 64 || index == 0) printf("==========================");
    buffer[index] = value;
}

void draw_line_to_fill_buffers(Point p0, Point p1, uint8_t* lbuf, uint8_t* rbuf){\
    /*
    ASSUMPTION: Points P1, P2, P3 are given in clockwise order
    If line Y is increasing, we are on the left buffer (clockwise)
    If line Y is decreasing, we are on the right buffer (clockwise)
    if line Y is neither increasing nor decreasing
        min x of line to left buffer, max x of line to right buffer    
    */

   // TODO 
   // We have an issue drawing
   // Point(66, 0) -> Point(0,63)
   // The very first x coordinate (which should be 66, or is it reversed?) is not transferred
   // to the lbuffer
   // Q: is it reversed?
   // x range 66
   // y range 63, y is increasing, x is decreasing, line is longer in horizontal
    float slope;
    if((p0.gety() == p1.gety()) && (p0.gety() < FRAME_Y_RESOLUTION)) { // Simple case, min_x to lbuf and max_x to rbuf
        if(p0.getx() > p1.getx()) {
            buffer_wrapper(rbuf, p0.gety(), p0.getx());
            buffer_wrapper(lbuf, p0.gety(), p1.getx());
        } else {
            buffer_wrapper(rbuf, p0.gety(), p1.getx());
            buffer_wrapper(lbuf, p0.gety(), p0.getx());
        }
        return; // Early return, don't need to do the rest of this
    }

    // Normal case - Which buffer does this line go to?
    uint8_t* buffer;
    if(p0.gety() < p1.gety()) { // Y is increasing
        buffer = lbuf;
    } else { // Y is decreasing
        buffer = rbuf;
    }



    // TODO here begins duplicated code from draw_line
    if(std::abs(p1.gety() - p0.gety()) < std::abs(p1.getx() - p0.getx())){ // Line is longer in horizontal direction Multiple xs for same ys
        float y = p0.gety();
        slope = ((float)(p1.gety() - p0.gety()) / (p1.getx() - p0.getx()));


        // If lbuf and x increasing, reverse x direction
        // If rbuf and x decreasing, reverse x direction
        // This is because multiple xs can share the same ys, we want the widest possible x range for each y

        if(p0.getx() < p1.getx()){ // x increasing
            if(buffer == lbuf){
                y = p1.gety();
                for(int x = p1.getx(); x > p0.getx(); x--){
                    if(coord_is_drawable((int)x, (int)y)) buffer_wrapper(buffer, (uint8_t)y, x);
                    y -= slope;
                }
            } else {
                for(int x = p0.getx(); x < p1.getx(); x++){
                    if(coord_is_drawable((int)x, (int)y)) buffer_wrapper(buffer, (uint8_t)y, x);
                    y += slope;
                }
            }
        } else {                   // x decreasing
            if(buffer == rbuf){
                y = p1.gety();
                for(int x = p1.getx(); x < p0.getx(); x++){
                    if(coord_is_drawable((int)x, (int)y)) buffer_wrapper(buffer, (uint8_t)y, x);
                    y += slope;            
                }
            } else {
                for(int x = p0.getx(); x > p1.getx(); x--){
                    if(coord_is_drawable((int)x, (int)y)) buffer_wrapper(buffer, (uint8_t)y, x);
                    y -= slope;            
                }
            }
        }
    }
    else { // Line is 45 degrees or longer in vertical direction
        float x = p0.getx();
        slope = ((float)(p1.getx() - p0.getx()) / (p1.gety() - p0.gety()));        
        if(p0.gety() < p1.gety()){
            for(int y = p0.gety(); y < p1.gety(); y++){
                //printf("(%f, %d)\n", x, y);
                if(coord_is_drawable((int)x, (int)y)) buffer_wrapper(buffer, (uint8_t)y, x);
                x += slope;
            }
        } else {
            for(int y = p0.gety(); y > p1.gety(); y--){
                if(coord_is_drawable((int)x, (int)y)) buffer_wrapper(buffer, (uint8_t)y, x);
                x -= slope;
            }
        }
    }

    // As a final measure "dot" the endpoints.  This takes care of any sloppyness from float->int stuff
    if(coord_is_drawable(p0.getx(), p0.gety())) buffer_wrapper(buffer, p0.gety(),p0.getx());
    if(coord_is_drawable(p1.getx(), p1.gety())) buffer_wrapper(buffer, p1.gety(),p1.getx());

    
}

class Line {
    public:
        Point3 p1, p2;
        Line(Point3 p1, Point3 p2): p1(p1), p2(p2){}

        Line operator + (Point3 p){
            return Line(p1 + p, p2 + p);
        }

        Line project(Matrix<float> projection_matrix){
            // Project from 3D -> 2D using provided projection matrix
            Matrix<float> projected_matrix(2, 1);
            Point3 projected_p1, projected_p2;
            projected_matrix = m2x3by3x1(projection_matrix, p1.to_matrix());
            projected_p1.set(projected_matrix.elements[0][0], projected_matrix.elements[1][0], 0);
            projected_matrix = m2x3by3x1(projection_matrix, p2.to_matrix());
            projected_p2.set(projected_matrix.elements[0][0], projected_matrix.elements[1][0], 0);
            return Line(projected_p1, projected_p2);
        }

        void draw_to(uint8_t* buffer){
            draw_shaded_line_to_buffer(p1.to_2d(), p2.to_2d(), buffer, SHADE_SOLID); //TODO draw_shaded_line should be able to just take a line.
        }


}; // Line

void print_buffers(uint8_t* lbuffer, uint8_t* rbuffer){
    printf("Y   LBUFFER RBUFFER\n");
    for(int i = 0; i < FRAME_Y_RESOLUTION; i++){
        printf("%d:  %d,  %d\n", i, lbuffer[i], rbuffer[i]);
    }
}

void fill_quad(Point p1, Point p2, Point p3, Point p4, uint8_t* buffer, uint8_t shade){
    // TODO COPIED FROM fill_tri
    //printf("(%d, %d)\n", p1.getx(), p1.gety());
    //printf("(%d, %d)\n", p2.getx(), p2.gety());
    //printf("(%d, %d)\n", p3.getx(), p3.gety());
    //draw_line(p1, p2);
    //draw_line(p2, p3);
    //draw_line(p3, p1);

    /*
    Idea is to use two buffers, left and right, to hold the x coordinates
    of the left and right side of a horizontal line between them, at each
    vertical position that should be rendered.  Then just draw the lines.
    \<---------->\
     \<---------->\
      \<---------->|
       \<--------->|
        \<-------->|
    ASSUMPTION: Points P1, P2, P3 are given in clockwise order
    If line Y is increasing, we are on the left buffer (clockwise)
    If line Y is decreasing, we are on the right buffer (clockwise)
    if line Y is neither increasing nor decreasing
        min x of line to left buffer, max x of line to right buffer
    */
    uint8_t buf_left[FRAME_Y_RESOLUTION] = { }, buf_right[FRAME_Y_RESOLUTION] = { };
    draw_line_to_fill_buffers(p1, p2, buf_left, buf_right);
    draw_line_to_fill_buffers(p2, p3, buf_left, buf_right);
    draw_line_to_fill_buffers(p3, p4, buf_left, buf_right);
    draw_line_to_fill_buffers(p4, p1, buf_left, buf_right);
    for(int i = 0; i < (FRAME_Y_RESOLUTION - 1); i++){
        if(buf_left[i] != buf_right[i]){
            draw_shaded_line_to_buffer(Point(buf_left[i], i), Point(buf_right[i], i), buffer, shade);
        }
    }
}

class Quad {
    // A quad is defined as four points.  It represents a surface created by
    // joining those points with lines in order: p1 -> p2 -> p3 -> p4 -> p1.
    // The "outward" (drawn) side of a quad is determined by right hand rule
    // from the points in order.
    public:
        Point3 p1, p2, p3, p4;
        uint8_t shade;

        Quad(Point3 p1, Point3 p2, Point3 p3, Point3 p4, uint8_t shade=SHADE_SOLID): p1(p1), p2(p2), p3(p3), p4(p4), shade(shade){}

        bool is_forward_facing(void){
            // TODO There is currently no care to ensure all points are on a plane
            // So we will say the whole quad is forward facing if ANY of the four 
            // cross products have negative Z (Face the camera)
            if(((p2 - p1) * (p3 - p2)).getz() > 0) return true;
            if(((p3 - p2) * (p4 - p3)).getz() > 0) return true;
            if(((p4 - p3) * (p1 - p4)).getz() > 0) return true;
            if(((p1 - p4) * (p2 - p1)).getz() > 0) return true;
            return false;
        }

        Quad operator + (Point3 p){
            return Quad(p1 + p, p2 + p, p3 + p, p4 + p, shade);
        }

        Quad project(Matrix<float> projection_matrix){
            // Project from 3D -> 2D using provided projection matrix
            Matrix<float> projected_matrix(2, 1);
            Point3 projected_p1, projected_p2, projected_p3, projected_p4;

            projected_matrix = m2x3by3x1(projection_matrix, p1.to_matrix());
            projected_p1.set(projected_matrix.elements[0][0], projected_matrix.elements[1][0], 0);

            projected_matrix = m2x3by3x1(projection_matrix, p2.to_matrix());
            projected_p2.set(projected_matrix.elements[0][0], projected_matrix.elements[1][0], 0);

            projected_matrix = m2x3by3x1(projection_matrix, p3.to_matrix());
            projected_p3.set(projected_matrix.elements[0][0], projected_matrix.elements[1][0], 0);

            projected_matrix = m2x3by3x1(projection_matrix, p4.to_matrix());
            projected_p4.set(projected_matrix.elements[0][0], projected_matrix.elements[1][0], 0);                        

            return Quad(projected_p1, projected_p2, projected_p3, projected_p4, shade);
        }   

        void draw_to(uint8_t* buffer){
            fill_quad(p1.to_2d(), p2.to_2d(), p3.to_2d(), p4.to_2d(), buffer, shade); // TODO can I make the "to_2d() less clunky?"
            //draw_shaded_line_to_buffer(p1.to_2d(), p2.to_2d(), SHADE_SOLID); //TODO draw_shaded_line should be able to just take a line.
        }

}; // Quad

void fill_tri(Point p1, Point p2, Point p3, uint8_t* buffer, uint8_t shade){
    //printf("(%d, %d)\n", p1.getx(), p1.gety());
    //printf("(%d, %d)\n", p2.getx(), p2.gety());
    //printf("(%d, %d)\n", p3.getx(), p3.gety());
    //draw_line(p1, p2);
    //draw_line(p2, p3);
    //draw_line(p3, p1);

    /*
    Idea is to use two buffers, left and right, to hold the x coordinates
    of the left and right side of a horizontal line between them, at each
    vertical position that should be rendered.  Then just draw the lines.
    \<---------->\
     \<---------->\
      \<---------->|
       \<--------->|
        \<-------->|
    ASSUMPTION: Points P1, P2, P3 are given in clockwise order
    If line Y is increasing, we are on the left buffer (clockwise)
    If line Y is decreasing, we are on the right buffer (clockwise)
    if line Y is neither increasing nor decreasing
        min x of line to left buffer, max x of line to right buffer
    */
    uint8_t buf_left[FRAME_Y_RESOLUTION] = { }, buf_right[FRAME_Y_RESOLUTION] = { }; 
    draw_line_to_fill_buffers(p1, p2, buf_left, buf_right);
    draw_line_to_fill_buffers(p2, p3, buf_left, buf_right);
    draw_line_to_fill_buffers(p3, p1, buf_left, buf_right);
    for(int i = 0; i < FRAME_Y_RESOLUTION; i++){
        if(buf_left[i] != buf_right[i]){
            draw_shaded_line_to_buffer(Point(buf_left[i], i), Point(buf_right[i], i), buffer, shade);
        }
    }
}

class Tri {
    public:
        Point3 p1, p2, p3;
        uint8_t shade;

        Tri(Point3 p1, Point3 p2, Point3 p3, uint8_t shade=SHADE_SOLID): p1(p1), p2(p2), p3(p3), shade(shade){}

        bool is_forward_facing(void){
            // TODO There is currently no care to ensure all points are on a plane
            // So we will say the whole quad is forward facing if ANY of the three 
            // cross products have negative Z (Face the camera)
            if(((p2 - p1) * (p3 - p2)).getz() > 0) return true;
            if(((p3 - p2) * (p1 - p3)).getz() > 0) return true;
            if(((p1 - p3) * (p2 - p1)).getz() > 0) return true;
            return false;
        }

        Tri operator + (Point3 p){
            return Tri(p1 + p, p2 + p, p3 + p, shade);
        }

        void print(void){
            printf("Triangle has points: \n");
            p1.print();
            p2.print();
            p3.print();
        }

        Tri project(Matrix<float> projection_matrix){
            // Project from 3D -> 2D using provided projection matrix
            Matrix<float> projected_matrix(2, 1);
            Point3 projected_p1, projected_p2, projected_p3;

            projected_matrix = m2x3by3x1(projection_matrix, p1.to_matrix());
            projected_p1.set(projected_matrix.elements[0][0], projected_matrix.elements[1][0], 0);

            projected_matrix = m2x3by3x1(projection_matrix, p2.to_matrix());
            projected_p2.set(projected_matrix.elements[0][0], projected_matrix.elements[1][0], 0);

            projected_matrix = m2x3by3x1(projection_matrix, p3.to_matrix());
            projected_p3.set(projected_matrix.elements[0][0], projected_matrix.elements[1][0], 0);                   

            return Tri(projected_p1, projected_p2, projected_p3, shade);
        }   

        void draw_to(uint8_t* buffer){
            fill_tri(p1.to_2d(), p2.to_2d(), p3.to_2d(), buffer, shade); // TODO can I make the "to_2d() less clunky?"
            //draw_shaded_line_to_buffer(p1.to_2d(), p2.to_2d(), SHADE_SOLID); //TODO draw_shaded_line should be able to just take a line.
        }

        Tri operator * (float factor){ // Scale
            return Tri(p1.scale(factor), p2.scale(factor), p3.scale(factor), shade);
        }        

}; // Tri

Matrix<> matrix_multiply(Matrix<> m1, Matrix<> m2){
    // todo dynamic size of retval
    Matrix<> retval(2, 1);

    if(m1.elements[0].size() != m2.elements.size()) {
        printf("Columns of first matrix must match rows of second!\n");
        return retval;
    }

    int sum = 0;
    for(int i = 0; i < m1.getrows(); i++){
        for(int j = 0; j < m2.getcolumns(); j++){
            sum = 0;
            for(int k = 0; k < m1.getcolumns(); k++){
                sum += m1.elements[i][k] * m2.elements[k][j];
            }
            retval.elements[i][j] = sum;
        }
    }
    return retval;
}

Matrix<float> m3x3by3x1(Matrix<float> m1, Matrix<float> m2){
    Matrix<float> retval(3, 1);
    if((m1.getcolumns() != 3) || (m2.getrows() != 3)){
        printf("Columns of first matrix must match rows of second!\n");
        return retval;
    }
    retval.elements[0][0] = ((m1.elements[0][0] * m2.elements[0][0]) +
                             (m1.elements[0][1] * m2.elements[1][0]) +
                             (m1.elements[0][2] * m2.elements[2][0]));
    retval.elements[1][0] = ((m1.elements[1][0] * m2.elements[0][0]) +
                             (m1.elements[1][1] * m2.elements[1][0]) +
                             (m1.elements[1][2] * m2.elements[2][0]));
    retval.elements[2][0] = ((m1.elements[2][0] * m2.elements[0][0]) +
                             (m1.elements[2][1] * m2.elements[1][0]) +
                             (m1.elements[2][2] * m2.elements[2][0]));                             
    return retval;
}

Point3 rotate_point(RotationMatrix3 m, Point3 p){
    Point3 rotated_point;
    Matrix<float> rotated_point_matrix(3, 1);
    rotated_point_matrix = m3x3by3x1(m, p.to_matrix());
    rotated_point.set(rotated_point_matrix.elements[0][0],
                      rotated_point_matrix.elements[1][0],
                      rotated_point_matrix.elements[2][0]);
    return rotated_point;
}

class Sprite{
    // I'm calling Sprite a collection of things in 3D that should move/rotate/draw together
    // Points are RELATIVE TO ORIGIN
    // RENDER will cast them back to absolute coordinates
    public:
        Point3 origin;
        float angle_x;
        float angle_y;
        float angle_z;
        std::vector<Point3> points;
        std::vector<Line> lines;
        std::vector<Quad> quads;
        std::vector<Tri> tris;
        Sprite(Point3 origin = Point3(64, 32, 0)): origin(origin), angle_x(0), angle_y(0), angle_z(0){}
        Sprite(float x, float y, float z): Sprite(Point3(x, y, z)){}
        
        
        void add_point(Point3 p){
            points.push_back(p);
        }
        void create_point(float x, float y, float z){
            points.emplace_back(x, y, z);
        }
        void add_line(Line l){
            lines.push_back(l);
        }
        void create_line(Point3 p1, Point3 p2){
            lines.emplace_back(p1, p2);
        }
        void create_line(float x1, float y1, float z1, float x2, float y2, float z2){
            lines.emplace_back(Point3(x1, y1, z1), Point3(x2, y2, z2));
        }
        void create_quad(Point3 p1, Point3 p2, Point3 p3, Point3 p4, uint8_t shade){
            quads.emplace_back(p1, p2, p3, p4, shade);
        }
        void create_tri(Point3 p1, Point3 p2, Point3 p3, uint8_t shade){
            tris.emplace_back(p1, p2, p3, shade);
        }        
        // TODO who should hold the rotation matrix?
        // Should they be calculated at time rotate?
        // probably...
        void rotate_z(float new_z_angle){
            angle_z = new_z_angle;
            printf("angle_z: %f\n", angle_z);
                    
            // Create rotation matrix Z
            RotationMatrixZ rotation_matrix_z(angle_z); // TODO What's convention for casing for class vs instance?
            rotation_matrix_z.update();
            rotation_matrix_z.angle = angle_z;
            rotation_matrix_z.update(); //TODO Two updates?  :(

            printf("Rotate each part of the sprite\n");
            // Rotate each part of the sprite
            print();
            for(int i = 0; i < points.size(); i++){
                points[i] = rotate_point(rotation_matrix_z, points[i]); // TODO rotate should be a method of a rotation matrix
            }
            print();
        }

        void print(void){
            for(int i = 0; i < points.size(); i++){
                points[i].print();
            }
        }

        Line rotate_line(RotationMatrix3 m, Line l){
            Line rotated_line(rotate_point(m, l.p1), rotate_point(m, l.p2));
            return rotated_line;
        }

        Quad rotate_quad(RotationMatrix3 m, Quad q){
            Quad rotated_quad(rotate_point(m, q.p1),
                              rotate_point(m, q.p2),
                              rotate_point(m, q.p3),
                              rotate_point(m, q.p4), q.shade);
            return rotated_quad;
        }

        Tri rotate_tri(RotationMatrix3 m, Tri t){
            Tri rotated_tri(rotate_point(m, t.p1),
                            rotate_point(m, t.p2),
                            rotate_point(m, t.p3), t.shade);
            return rotated_tri;
        }        
        

        void render(Matrix<float> projection_matrix, uint8_t* buffer){ // TODO should the projection matrix be held by the sprite?  Probably not...
            // For each thing to render, we need to rotate, then cast from relative (to origin) -> absolute position,
            // then project the point from 3D -> 2D using the projection matrix given
            // Then, just draw each point to the buffer

            Point3 rotated_point;
            Point3 absolute_point;
            Point projected_point;
            Matrix<float> projected_matrix(2, 1);
            RotationMatrixZ rotation_matrix_z(angle_z); // TODO What's convention for casing for class vs instance?
            RotationMatrixY rotation_matrix_y(angle_y);
            RotationMatrixX rotation_matrix_x(angle_x);            
            for(int i = 0; i < points.size(); i++){
                // Rotate the point
                rotated_point = rotate_point(rotation_matrix_z, points[i]);
                rotated_point = rotate_point(rotation_matrix_y, rotated_point);
                rotated_point = rotate_point(rotation_matrix_x, rotated_point);

                // Cast to absolute coordinates
                absolute_point.set(rotated_point.getx() + origin.getx(),
                                   rotated_point.gety() + origin.gety(),
                                   rotated_point.getz() + origin.getz());

                // Project from 3D -> 2D
                projected_matrix = m2x3by3x1(projection_matrix, absolute_point.to_matrix());
                projected_point.set(projected_matrix.elements[0][0], projected_matrix.elements[1][0]);

                // Draw to buffer
                shade_px(buffer, SHADE_SOLID, projected_point.getx(), projected_point.gety());                                  
            }
            for(int i = 0; i < lines.size(); i++){
                // Rotate the line
                Line rotated_line = rotate_line(rotation_matrix_z, lines[i]);
                rotated_line = rotate_line(rotation_matrix_y, rotated_line);
                rotated_line = rotate_line(rotation_matrix_x, rotated_line);

                // Cast to absolute coordinates
                Line absolute_line = rotated_line + origin;

                // Project from 3D -> 2D
                Line projected_line = absolute_line.project(projection_matrix);

                // Draw to buffer
                projected_line.draw_to(buffer);
            }
            for(int i = 0; i < quads.size(); i++){
                // Rotate the quad
                Quad rotated_quad = rotate_quad(rotation_matrix_z, quads[i]);
                rotated_quad = rotate_quad(rotation_matrix_y, rotated_quad);
                rotated_quad = rotate_quad(rotation_matrix_x, rotated_quad);

                // Check outward size is towards POV, else skip drawing
                if(rotated_quad.is_forward_facing()){

                    // Cast to absolute coordinates
                    Quad absolute_quad = rotated_quad + origin;

                    // Project from 3D -> 2D
                    Quad projected_quad = absolute_quad.project(projection_matrix);

                    // Draw to buffer
                    projected_quad.draw_to(buffer);
                }
            }
            for(int i = 0; i < tris.size(); i++){
                // Rotate the tri
                Tri rotated_tri = rotate_tri(rotation_matrix_z, tris[i]);
                rotated_tri = rotate_tri(rotation_matrix_y, rotated_tri);
                rotated_tri = rotate_tri(rotation_matrix_x, rotated_tri);

                // Check outward size is towards POV, else skip drawing
                if(rotated_tri.is_forward_facing()){

                    // Cast to absolute coordinates
                    Tri absolute_tri = rotated_tri + origin;

                    // Project from 3D -> 2D
                    Tri projected_tri = absolute_tri.project(projection_matrix);

                    // Draw to buffer
                    projected_tri.draw_to(buffer);
                }
            }                   
        }

        // TODO Why doesn't this work?
        void scale(float factor){
            for(auto p : points){
                p = p.scale(factor);
            }
            //for(auto l : lines){
            //    l = l * factor;
            //}
            for(auto t : tris){
                t = t * factor;
            }
        }

}; // Sprite

class Cube : public Sprite{
    public:
        int radius;
        Cube(int radius, Point3 origin = Point3(64, 32, 0)): Sprite(origin), radius(radius){
            Point3 p1(-radius, -radius, -radius);
            Point3 p2(-radius, -radius,  radius);
            Point3 p3(-radius,  radius, -radius);
            Point3 p4(-radius,  radius,  radius);
            Point3 p5( radius, -radius, -radius);
            Point3 p6( radius, -radius,  radius);
            Point3 p7( radius,  radius, -radius);
            Point3 p8( radius,  radius,  radius);

            create_quad(p4, p8, p6, p2, SHADE_SOLID); // Front
            create_quad(p5, p7, p3, p1, 2); // Back
            create_quad(p3, p7, p8, p4, 4); // Top
            create_quad(p2, p6, p5, p1, 6); // Bottom
            create_quad(p1, p3, p4, p2, 8); // Left
            create_quad(p5, p6, p8, p7, 10); // Right
        }
}; // Cube

Sprite get_lomont_shape(int choice){
    Sprite sprite;
    LomontShape lomontshape(choice);
    for(int i = 0; i < lomontshape.points.size() - 2; i+=3){
        sprite.create_point(lomontshape.points[i], lomontshape.points[i+1], lomontshape.points[i+2]);
    }

    // scale into unit radius sphere
    auto max = 0.0f;
    for (auto & v : sprite.points)
    {
        auto d = v.length();
        max = std::max(d, max);			
    }
        printf("Max is: %f\n", max);
    max = max * 0.6; // nicer scaling
    for (auto& v : sprite.points)
    {
        v.x /= max;
        v.y /= max;
        v.z /= max;
    } 

    // TODO Scale this elsewhere
    float factor = 20;
    for (auto& v : sprite.points)
    {
        v.x *= factor;
        v.y *= factor;
        v.z *= factor;
    }     

    //for(int i = 1; i < lomontshape.faces.size() - 2; i+=4){
    for(int i = 0; i < lomontshape.faces.size();){
        if(lomontshape.faces[i] == 3){
            sprite.create_tri(sprite.points[lomontshape.faces[i+1]],
                              sprite.points[lomontshape.faces[i+2]],
                              sprite.points[lomontshape.faces[i+3]], i % MAX_SHADES);
            i += 4;
        }
        else if(lomontshape.faces[i] == 4){
            sprite.create_quad(sprite.points[lomontshape.faces[i+1]],
                               sprite.points[lomontshape.faces[i+2]],
                               sprite.points[lomontshape.faces[i+3]],
                               sprite.points[lomontshape.faces[i+4]], i % MAX_SHADES);
            i += 5;
        }
        else if(lomontshape.faces[i] == 5){
            sprite.create_quad(sprite.points[lomontshape.faces[i+1]],
                               sprite.points[lomontshape.faces[i+2]],
                               sprite.points[lomontshape.faces[i+3]],
                               sprite.points[lomontshape.faces[i+4]], i % MAX_SHADES);
            sprite.create_tri(sprite.points[lomontshape.faces[i+4]],
                              sprite.points[lomontshape.faces[i+5]],
                              sprite.points[lomontshape.faces[i+1]], i % MAX_SHADES);                                
            i += 6;
        }
        else {
            printf("Crap, can't make a face with %d points!!!\n", lomontshape.faces[i]);
            i += (lomontshape.faces[i] + 1);
        }
    }    

    return sprite;
}

void my_setPixel(int x, int y, int color){
    // Color is 1 if on, 0 if off
    uint8_t shade;
    if(color == 1){ shade = SHADE_SOLID; }
    else{ shade = SHADE_EMPTY; }
    shade_px(screen_buffer, shade, x, y);
}

class Pole{
    public:
        // A pole has a location, given by (x, y, z) (inherited)
        // A pole has a speed, given by <dx, dy, dz>
        Point3 location;        
        Vec3 speed;
        
        // A pole also has a mass, given by m
        float m;

        Pole(float x0 = 0, float y0 = 0, float z0 = 0): location(Point3(x0, y0, z0)), speed(Vec3(0, 0, 0)), m(1){};

        Pole operator + (Point3 p){
            Pole ret_val(location.x + p.x, location.y + p.y, location.z + p.z);
            ret_val.speed = speed;
            ret_val.m = m;
            return ret_val;
        }

        Pole operator - (Point3 p){
            Pole ret_val(location.x - p.x, location.y - p.y, location.z - p.z);
            ret_val.speed = speed;
            ret_val.m = m;            
            return ret_val;
        }

        Vec3 force_at(Point3 p1){
            // Calculate the force from this pole at point p1
            // F = GMm/R2
            // G = gravitational Constant Eh, get rid of it
            // M is mass of the pole
            // m is mass of the point?
            // R is the distance from pole to point
            // float F = 1/(distance * distance)
            // Magnitude is F, direction is unit vector in direction p1 -> this pole
            Vec3 f = (location - p1);
            float distance = f.magnitude();
            float magnitude = 1 / (distance * distance);
            f.to_unit();
            f.scale(magnitude);
            //pull.print();
            return f;
        }

        void print(){
            printf("Pole: (%d, %d, %d)<%.1f, %.1f, %.1f>\n", (int)location.x, (int)location.y, (int)location.z, speed.x, speed.y, speed.z);
        }

}; // Pole

class System{
    public:
        std::vector<Pole> poles;
        //std::vector<Hole> holes;

        System(){};



        Vec3 force_vector(Point3 p1){
            // Get the force vector of the system at point p
            Vec3 force(0, 0, 0);
            for(auto &p : poles){
                force = force + p.force_at(p1);
            }
            return force;
        }

        Point3 walk_perp_z(Vec3 v, Point3 p){
            // Return the next drawable point from p that is perpendicular to v wrt to z
            int x = round(p.x); //uint8 because these should map to drawable coordinates (don't care about decimals)
            int y = round(p.y);
            //uint8_t z = p.z;
            
            Vec3 perp(v * Vec3(0, 0, 1));
            //std::cout << "Perpendicular Vector (wrt force vector: " << v << " and zhat): " << perp << std::endl;
            perp.to_unit();
            //std::cout << "Perpendicular unit vector: " << perp << std::endl;
            Point3 ret_val = p + perp;
            while(true){
                if(((int)round(ret_val.x) != x) || ((int)round(ret_val.y) != y)){
                    ret_val.round();
                    return ret_val;
                }
                ret_val = ret_val + perp;
            }
        }

        Point3 discrete_step(Vec3 direction, Point3 p){
            // Move by at most 1 integer (<1>, <1>, <1>) in direction
            direction.to_unit();
            //std::cout << "Perpendicular unit vector: " << perp << std::endl;
            Point3 ret_val = p + direction;
            while(true){
                if(((int)ret_val.x != (int)p.x) || ((int)ret_val.y != (int)p.y)){
                    ret_val.round();
                    return ret_val;
                }
                ret_val = ret_val + direction;
            }
        }

        Point3 hug_target_magnitude(float targetmag, Point3 p){
            Vec3 fv = force_vector(p);
            float mag = fv.magnitude(); // Get the magnitude of force at this point
            float newmag;
            Point3 newpoint;
            while(true){
                // If the magnitude is less than the target magnitude, move one px in the direction of the force_vector(p)
                if(mag < targetmag){
                    newpoint = discrete_step(fv, p);
                    newmag = force_vector(newpoint).magnitude();
                    if(std::abs(newmag - targetmag) < std::abs(mag - targetmag)) { return newpoint; }
                    else {return p; }
                }
                // If the magnitude is greater than the target magnitude, move on px opposite the direciton of the force_vector(p)
                else if(mag > targetmag) {
                    newpoint = discrete_step(-fv, p);
                    newmag = force_vector(newpoint).magnitude();
                    if(std::abs(newmag - targetmag) < std::abs(mag - targetmag)) { return newpoint; }
                    else {return p; }
                }
                // If mag is not greater or less than the target, it's probaly equal, just call it good and return
                return p;
            }
        }

        //Point3 get_next_ring_point(Point3 p_last, float targetmag){
        //    Point3 p = p_last;
        //    Point3 p_walked;
        //    Point3 p_checked;
        //    std::cout << "Feeding Point to walk_perp(): " << p_last;
        //    p_walked = walk_perp_z(force_vector(p_last), p_last);
        //    std::cout << "Feeding Point to hug_target(): " << p_walked;
        //    p_checked = hug_target_magnitude(targetmag, p_walked);
        //    std::cout << "Checking Point: " << p_checked;
        //    if(check_point != p1){ p2 = check_point; }
        //    p1 = p2;
        //}

        void draw_rings(uint8_t* buffer){
            float start = 0.1;
            float spacing = 0.3;
            int rings = 5;
            printf("Starting to draw rings!\n");
            for(auto &p : poles){
                for(int ring = 0; ring < rings; ring++){
                    std::cout << "Drawing ring:  " << ring << std::endl;
                    Point3 p1(p.location.x, p.location.y, p.location.z);
                    float targetmag = start * pow(spacing, ring);
                    float mag = targetmag + 1;
                    // Walk NORTH until the magnitude crosses from > to <
                    while(mag > targetmag){
                        p1.y += 1;
                        mag = force_vector(p1).magnitude();
                    }
                    p1 = hug_target_magnitude(targetmag, p1);
                    //std::cout << "Drawing point: " << p1 << " Mag(F) == " << force_vector(p1).magnitude() << " | " << targetmag << std::endl;
                    shade_px(buffer, SHADE_SOLID, p1.x, p1.y);
                    
                    // Walk p1 perpendicular to its force_vector until p1 is reached again
                    //     to define the force countour at this magnitude
                    // The direction perpendicular to the force_vector is given by f cross zhat
                    //p2  = (p1 + (f cross zhat)unit)
                    Point3 stop_point((uint8_t)p1.x, (uint8_t)p1.y, (uint8_t)p1.z);
                    Point3 p2 = p1;
                    Point3 check_point;
                    p2 = walk_perp_z(force_vector(p2), p2);
                    check_point = hug_target_magnitude(targetmag, p2);
                    if(check_point != stop_point){ p2 = check_point; }
                    p1 = p2;
                    //std::cout << "Drawing point: " << p2 << " Mag(F) == " << force_vector(p2).magnitude() << " | " << targetmag << std::endl;
                    shade_px(buffer, SHADE_SOLID, p2.x, p2.y);
                    while(!((int)stop_point.x == (int)p2.x &&
                            (int)stop_point.y == (int)p2.y &&
                            (int)stop_point.z == (int)p2.z)){
                        //std::cout << "Feeding Point to walk_perp(): " << p2;   
                        p2 = walk_perp_z(force_vector(p2), p2);
                        //std::cout << "Feeding Point to hug_target(): " << p2;
                        check_point = hug_target_magnitude(targetmag, p2);
                        //std::cout << "Checking Point: " << check_point;
                        if(check_point == p1){ 
                            //std::cout << " Rejecting, equal to p1! <- " << p1;
                        }else{
                            p2 = check_point;
                        }
                        p1 = p2;
                        shade_px(buffer, SHADE_SOLID, p2.x, p2.y);
                        //std::cout << "Hit Point " << p2 << " with pole at " << poles[0].location << " Mag(F) == " << force_vector(p2).magnitude() << " | " << targetmag << std::endl;
                        
                    }
                    std::cout << "Finished ring: " << ring << std::endl;
                }
            }
            std::cout << "Drawing rings!" << std::endl;
        }

}; // Poles

//void draw_rings(uint8_t* buffer, std::vector<Pole> poles){
    //pole.print();
    //int rings = 5;
    //int spacing = 5; 

    // Walk north force from all poles is <spacing>
    // F = GMm/R2
    // G = gravitational Constant Eh, get rid of it
    // M is mass of the pole
    // m is mass of the point?
    // R is the distance from pole to point
    // float F = 1/(distance * distance)

    //for(auto &p : poles){
    //    for(int ring = 0; ring < rings; ring++){

    //    }
    //}
//}

#endif // MY_3D_STUFF_H

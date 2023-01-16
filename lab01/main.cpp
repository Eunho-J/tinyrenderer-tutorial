#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

inline void swap(int* a, int* b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

inline int abs(int a)
{
    return a >= 0 ? a : -a;
}

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
    bool steep = false;
    int dx, dy, derror, error_sum, x, y;
    if (x0 > x1) 
    {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }
    if (abs(x1 - x0) < abs(y1 - y0))
    {
        swap(&x0, &y0);
        swap(&x1, &y1);
        steep = true;
    }
    dx = x1 - x0;
    dy = y1 - y0;
    derror = abs(dy * 2);
    error_sum = 0;
    for (x = x0, y = y0; x <= x1; x++)
    {
        if (steep)
            image.set(y, x, color);
        else   
            image.set(x, y, color);
        error_sum += derror;
        if (error_sum > dx)
        {
            y += y1 > y0 ? 1 : -1;
            error_sum -= dx * 2;
        }
    }
}

int main(int argc, char const *argv[])
{
    TGAImage image(100, 100, TGAImage::RGB);

    line(5,5,89,52,image,white);
    line(90,3,5,5,image,red);
    line(10,10,10,90,image,blue);
    line(50,5,50,95,image,green);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}

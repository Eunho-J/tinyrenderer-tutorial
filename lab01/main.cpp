#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
    bool steep = false;
    
    
}

int main(int argc, char const *argv[])
{
    TGAImage image(100, 100, TGAImage::RGB);

    line(5,5,89,52,image,white);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}

#include <vector> 
#include <iostream> 
#include <limits>
#include "geometry.h"
#include "tgaimage.h" 
#include "model.h"
#include <Eigen/Core>
 
const int width  = 200; 
const int height = 200; 
 
Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    Vec3f s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}
 
void triangle(Vec3f *pts, float* zbuffer, TGAImage &image, TGAColor color) { 
    Vec2f bboxmin(image.get_width()-1,  image.get_height()-1); 
    Vec2f bboxmax(0, 0); 
    Vec2f clamp(image.get_width()-1, image.get_height()-1); 
    /* 이미지 전체의 점에 대해서 각 삼각형에 대해 검사를 하면 너무 많은 연산을 해야한다. 
       따라서 삼각형을 둘러싼 최소 크기의 Bounding Box를 구하여,
       해당 Box 안의 점에 대해서만 연산을 수행한다. */
    for (int i=0; i<3; i++) { 
        bboxmin.x = std::max(0.f, std::min(bboxmin.x, pts[i].x));
        bboxmin.y = std::max(0.f, std::min(bboxmin.y, pts[i].y));
        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    } 
    Vec3f P; 
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) { 
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) { 
            Vec3f bc_screen  = barycentric(pts, P); 
            //i, j, k중 하나라도 음수이면 삼각형 밖의 점이다.
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            image.set(P.x, P.y, color); 
        } 
    } 
} 

int main(int argc, char** argv) { 
    Model* model;
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    Vec3f light_dir(0,0,-1);
    float* zbuffer = new float[width*height];
    for (int i = width*height; i >= 0; i--) zbuffer[i] = -std::numeric_limits<float>::max();

    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec3f screen_coords[3];
        Vec3f world_coords[3];
        for (int j=0; j<3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec3f((v.x+1.)*width/2., (v.y+1.)*height/2., v.z);
            world_coords[j]  = v;
        }
        Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
        n.normalize();
        float intensity = n*light_dir;
        Vec3f pts[3] = {screen_coords[0], screen_coords[1], screen_coords[2]};
        if (intensity>0) {
            triangle(pts, zbuffer, image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "sphere.hpp"
#include "hitable_list.hpp"
#include "camera.hpp"
#include "metal.hpp"
#include "lambertian.hpp"
#include "dielectric.hpp"
#include "random.hpp"
#include "moving_sphere.hpp"
#include "bvh_node.hpp"
#include "constant_texture.hpp"
#include "checker_texture.hpp"
#include "noise_texture.hpp"
#include "image_texture.hpp"
#include "xy_rect.hpp"
#include "xz_rect.hpp"
#include "yz_rect.hpp"
#include "flip_normals.hpp"
#include "diffuse_light.hpp"
#include <unistd.h>
#include <SFML/Graphics.hpp>
#include <float.h>
#include <functional>
#include <fstream>
#include <omp.h>
using namespace std;

const int nx = 1024;
const int ny = 512;
int ns = 30;
int frames = 1;

const int max_depth = 16;

int bounces[max_depth+2] = {0};

int blackRays = 0;

const int TILE_SIZE = 4;

float tiles[nx/TILE_SIZE][ny/TILE_SIZE] = {0};

vec3 lookfrom(278, 278, -800);
vec3 lookat(278, 278, 0);
float dist_to_focus = 10.0;
float aperture = 0.0;
float vfov = 40.0;

camera cam(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny), aperture, dist_to_focus,0.0,1.0);

bvh_node * random_scene() {

    lookfrom = vec3(13,2,3);
    lookat = vec3(0,0,0);
    dist_to_focus = 10.0;
    aperture = 0.0;

    cam = camera(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus,0.0,1.0);

    int n = 500;
    hitable ** list = new hitable*[n+1];

    texture * checker = new checker_texture( new constant_texture(vec3(0.2,0.3,0.1)), new constant_texture(vec3(0.9,0.9,0.9)));

    list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian( checker ));
    int i = 1;

    for(int a = -11; a < 11; a++){
        for(int b = -11;b < 11; b++){

            vec3 center(a+0.9*random_double(),0.2,b+0.9*random_double());
            if((center-vec3(4,0.2,0)).length() > 0.9){
                float choose_mat = random_double();
                if (choose_mat < 0.8){
                    list[i++] = new moving_sphere(center, center, 0.0, 1.0, 0.2, new lambertian(new constant_texture(vec3(random_double() * random_double(), random_double()*random_double(), random_double()*random_double()))));
                } else if(choose_mat < 0.95){
                    list[i++] = new sphere(center, 0.2,
                                           new metal(vec3(0.5*(1 + random_double()),0.5*(1+random_double()),0.5*(1+random_double())),0.5*random_double()));
                } else {
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0,1,0),1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4,1,0), 1.0, new lambertian(new constant_texture(vec3(0.4,0.2,0.1))));
    list[i++] = new sphere(vec3(4,1,0),1.0,new metal(vec3(0.7,0.6,0.5),0.0));

    return new bvh_node(list,i,0.0,1.0);

}

bvh_node * two_spheres() {

    lookfrom = vec3(13,2,3);
    lookat = vec3(0,0,0);
    dist_to_focus = 10.0;
    aperture = 0.0;

    cam = camera(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus,0.0,1.0);

    texture * checker = new checker_texture( new constant_texture(vec3(0.2,0.3,0.1)), new constant_texture(vec3(0.9,0.9,0.9)));
    int n = 2;
    hitable **list = new hitable*[n];
    list[0] = new sphere(vec3(0,-10,0),10, new lambertian(checker));
    list[1] = new sphere(vec3(0,10,0),10, new lambertian(checker));

    return new bvh_node(list,2,0.0,1.0);
}

bvh_node * noisy_spheres() {
    
    lookfrom = vec3(13,2,3);
    lookat = vec3(0,0,0);
    dist_to_focus = 10.0;
    aperture = 0.0;

    cam = camera(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus,0.0,1.0);

    texture *pertext = new noise_texture(5);
    hitable **list = new hitable*[2];
    list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0,2,0),2,new lambertian(pertext));

    return new bvh_node(list,2,0.0,1.0);
}

bvh_node * image_on_sphere() {

    lookfrom = vec3(13,2,3);
    lookat = vec3(0,0,0);
    dist_to_focus = 10.0;
    aperture = 0.0;

    cam = camera(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus,0.0,1.0);

    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("spamton.jpg", &nx, &ny, &nn, 0);
    material *mat = new lambertian(new image_texture(tex_data,nx, ny));
    hitable **list = new hitable*[1];
    list[0] = new sphere(vec3(0,0,0), 3, mat);

    return new bvh_node(list,1,0.0,1.0);
}

bvh_node * rectangle_light() {

    lookfrom = vec3(30,7,20);
    lookat = vec3(0,0,0);
    dist_to_focus = 10.0;
    aperture = 0.0;

    cam = camera(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus,0.0,1.0);

    texture *pertext = new noise_texture(4);

    hitable **list = new hitable*[3];
    list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian( pertext ));
    list[1] = new sphere(vec3(0,2,0), 2, new lambertian( pertext ));
    list[2] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light( new constant_texture( vec3(4,4,4))));

    return new bvh_node(list,3,0.0,1.0);
}

bvh_node * sphere_light() {

    lookfrom = vec3(30,7,3);
    lookat = vec3(0,0,0);
    dist_to_focus = 10.0;
    aperture = 0.0;

    cam = camera(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus,0.0,1.0);

    texture *pertext = new noise_texture(4);

    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("spamton.jpg", &nx, &ny, &nn, 0);
    material *mat = new diffuse_light(new image_texture(tex_data,nx, ny));

    hitable **list = new hitable*[2];
    list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian( pertext ));
    list[1] = new sphere(vec3(0,4,0), 2, mat);

    return new bvh_node(list,2,0.0,1.0);
}

bvh_node *cornell_box()
{
    lookfrom = vec3(278, 278, -800);
    lookat = vec3(278, 278, 0);
    dist_to_focus = 10.0;
    aperture = 0.0;
    vfov = 40.0;

    cam = camera(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny), aperture, dist_to_focus,0.0,1.0);

    hitable **list = new hitable*[6];
    int i=0;
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );

    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
    list[i++] = new flip_normals( new xz_rect(0,555,0,555,555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals( new xy_rect(0, 555, 0, 555, 555, white));

    return new bvh_node(list,i,0.0,1.0);
}

vec3 random_in_unit_sphere(){
    vec3 p;
    do {
        p = 2.0*vec3(random_double(),random_double(),random_double()) - vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}

vec3 colorLight(const ray& r, hitable *world, int depth, int x, int y) {
    hit_record rec;

    if(world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;

        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

        if(depth < max_depth && rec.mat_ptr->scatter(r,rec,attenuation,scattered)){
            return emitted + attenuation * colorLight(scattered, world, depth+1, x, y);
        }
        else{
            bounces[depth+1]++;
            tiles[x/TILE_SIZE][y/TILE_SIZE] += (float)(depth+1)/(TILE_SIZE*TILE_SIZE*ns);
            return emitted;
        }
    } else {
        bounces[depth]++;
        tiles[x/TILE_SIZE][y/TILE_SIZE] += (float)depth/(TILE_SIZE*TILE_SIZE*ns);
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

vec3 colorDark(const ray& r, hitable *world, int depth, int x, int y) {
    hit_record rec;

    if(world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;

        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

        if(depth < max_depth && rec.mat_ptr->scatter(r,rec,attenuation,scattered)){
            return emitted + attenuation * colorDark(scattered, world, depth+1, x, y);
        }
        else{
            bounces[depth+1]++;
            tiles[x/TILE_SIZE][y/TILE_SIZE] += (float)(depth+1)/(TILE_SIZE*TILE_SIZE*ns);
            return emitted;
        }
    } else {
        bounces[depth]++;
        tiles[x/TILE_SIZE][y/TILE_SIZE] += (float)depth/(TILE_SIZE*TILE_SIZE*ns);
        return vec3(0,0,0);
    }
}

bvh_node * world;

sf::Image image;

void calculateDark(int start_y, int end_y, int start_x, int end_x)
{
    for(int j = start_y; j <= end_y; j++){
        for(int i = start_x; i <= end_x; i++){
            vec3 col(0,0,0);
            for(int s=0;s<ns;s++){
                float u = float(i+random_double()) / float(nx);
                float v = float(j+random_double()) / float(ny);
                ray r = cam.get_ray(u,v);
                vec3 currentCol = colorDark(r,world,0, i, j);
                if(currentCol == vec3(0,0,0))
                    blackRays++;
                col += currentCol;
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]),sqrt(col[1]),sqrt(col[2]) );
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);

            image.setPixel(i,ny-j-1,sf::Color(ir,ig,ib));
        }
    }
}

void calculateLight(int start_y, int end_y, int start_x, int end_x)
{
    for(int j = start_y; j <= end_y; j++){
        for(int i = start_x; i <= end_x; i++){
            vec3 col(0,0,0);
            for(int s=0;s<ns;s++){
                float u = float(i+random_double()) / float(nx);
                float v = float(j+random_double()) / float(ny);
                ray r = cam.get_ray(u,v);
                vec3 currentCol = colorLight(r,world,0,i,j);
                if(currentCol == vec3(0,0,0))
                    blackRays++;
                col += currentCol;
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]),sqrt(col[1]),sqrt(col[2]) );
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);

            image.setPixel(i,ny-j-1,sf::Color(ir,ig,ib));
        }
    }
}

void masterThread(){

    char mode;
    std::cout << "¿Qué modo?" << std::endl;
    std::cout << "N: Normal" << std::endl;
    //std::cout << "B: ..." << std::endl;
    std::cout << "Introduce un modo: ";
    std::cin >> mode;

    char scene;
    std::cout << "¿Qué escena?" << std::endl;
    std::cout << "c: Cornell Box" << std::endl;
    std::cout << "w: Ray Tracing In One Weekend" << std::endl;
    std::cout << "2: Two Spheres" << std::endl;
    std::cout << "n: Noisy Spheres" << std::endl;
    std::cout << "i: Image on Sphere" << std::endl;
    std::cout << "r: Rectangular Light Source" << std::endl;
    std::cout << "s: Sphere Light Source" << std::endl;
    std::cout << "Introduce una escena: ";
    std::cin >> scene;

    std::cout << std::endl << "¿Cuántos rayos por píxel?" << std::endl;
    std::cout << "ns = ";
    std::cin >> ns;

    if(ns < 0){
        std::cerr << "ERROR: debe ser mayor o igual que 0" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << std::endl << "¿Cuántos frames?" << std::endl;
    std::cout << "frames = ";
    std::cin >> frames;
    if(frames < 1){
        std::cerr << "ERROR: debe ser mayor que 0" << std::endl;
        exit(EXIT_FAILURE);
    }

    time_t tt;
    struct tm* ti;
    time(&tt);
    ti = localtime(&tt);

    std::string fileName = std::string("files/")+scene+mode+std::string("_")+scene+std::string("_")+std::string(asctime(ti))+std::string(".txt");

    fileName.replace(fileName.find('\n'),1,"");

    std::fstream file(fileName,std::ios_base::out);

    file << "Escena: " << scene << std::endl;
    file << "Modo: " << mode << std::endl;
    file << "Rayos por píxel: " << ns << std::endl;

    //std::vector<sf::Thread*> threads;
    sf::Clock clock;

    for(int i=1;i<=frames;i++){

        lookfrom.e[0]+=1;

        cam.origin = lookfrom;

        switch(mode){
        case 'B':
            /*
            threads.push_back(new sf::Thread(std::bind(throwThreadsBigShot,0,ny/2,0,nx/2)));
            threads.push_back(new sf::Thread(std::bind(throwThreadsBigShot,0,ny/2,nx/2,nx)));
            threads.push_back(new sf::Thread(std::bind(throwThreadsBigShot,ny/2,ny,0,nx/2)));
            threads.push_back(new sf::Thread(std::bind(throwThreadsBigShot,ny/2,ny,nx/2,nx)));
            */
            //calculate(0,ny,0,nx);
            break;
        case 'N':
            /*
            threads.push_back(new sf::Thread(std::bind(throwThreads,0,ny/2,0,nx/2)));
            threads.push_back(new sf::Thread(std::bind(throwThreads,0,ny/2,nx/2,nx)));
            threads.push_back(new sf::Thread(std::bind(throwThreads,ny/2,ny,0,nx/2)));
            threads.push_back(new sf::Thread(std::bind(throwThreads,ny/2,ny,nx/2,nx)));
            */
            switch(scene){
            case 'c':
                
                world = cornell_box();

                clock.restart();
                calculateDark(0,ny-1,0,nx-1);

                break;
            case 'w':

                world = random_scene();

                clock.restart();
                calculateLight(0,ny-1,0,nx-1);
                break;
            case '2':

                world = two_spheres();

                clock.restart();
                calculateLight(0,ny-1,0,nx-1);
                break;
            case 'n':

                world = noisy_spheres();

                clock.restart();
                calculateLight(0,ny-1,0,nx-1);
                break;
            case 'i':

                world = image_on_sphere();

                clock.restart();
                calculateLight(0,ny-1,0,nx-1);
                break;
            case 'r':

                world = rectangle_light();

                clock.restart();
                calculateDark(0,ny-1,0,nx-1);
                break;
            case 's':

                world = sphere_light();

                clock.restart();
                calculateDark(0,ny-1,0,nx-1);
                break;
            }
            break;
        default:
            exit(EXIT_FAILURE);
            break;
        }

        /*clock.restart();

        for(sf::Thread *t : threads){
            t->launch();
        }

        for(sf::Thread *t : threads){
            t->wait();
        }
        */

        file << "-----------------------------" << std::endl << "Frame " << i << std::endl << std::endl;

        file << "Tiempo: " << clock.getElapsedTime().asSeconds() << " segundos" << std::endl << std::endl;

        if(i==frames) {
            image.saveToFile(std::string("images/")+mode+std::string("_")+scene+std::string("_")+std::string(asctime(ti))+std::string(".png"));
            
            sf::Image tileMap;
            tileMap.create(nx, ny);
            for(int i=0;i<nx;i++){
                for(int j=0;j<ny;j++){

                    float grayLevel = 255.99-(255.99*(tiles[i/TILE_SIZE][j/TILE_SIZE]/(max_depth+1)));

                    tileMap.setPixel(i,ny-j-1,sf::Color(grayLevel,grayLevel,grayLevel));
                }
            }
            tileMap.saveToFile(std::string("tilemaps/")+mode+std::string("_")+scene+std::string("_")+std::string(asctime(ti))+std::string(".png"));
        }

        int totalRays = 0;
        int totalBounces = 0;

        for(int i=0;i<=max_depth+1;i++){
            file << i << ";" << bounces[i] << std::endl;
            
            totalRays += bounces[i];
            totalBounces += i*bounces[i];
        }

        file << "Rayos totales: " << totalRays << std::endl;

        file << "Rayos que superaron el límite: " << bounces[max_depth+1] << " (" << 100*(double)bounces[max_depth+1]/totalRays << "%)" << std::endl;

        file << "Rayos que devolvieron un color negro puro: " << blackRays << " (" << 100*(double)blackRays/totalRays << "%)" << std::endl;

        file << "Media de rebotes: " << (double)totalBounces/totalRays << std::endl;

        blackRays = 0;
        memset(bounces,0,sizeof(int)*(max_depth+2));
        memset(tiles,0,TILE_SIZE*sizeof(float)*TILE_SIZE*sizeof(float));
    }
    /*
    for(sf::Thread *t : threads){
        delete t;
    }
    */
}

int main() {

    //sf::RenderWindow window(sf::VideoMode(nx, ny), "BigShot");
    //window.setFramerateLimit(60);

    image.create(nx, ny);

    sf::Thread master(masterThread);

    master.launch();

    master.wait();
    /*
    while(window.isOpen()){

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite;
        sprite.setTexture(texture);

        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
                exit(0);
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
    */
}
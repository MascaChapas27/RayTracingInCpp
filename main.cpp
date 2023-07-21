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
#include <SFML/Graphics.hpp>
#include <float.h>
#include <functional>

bvh_node * random_scene() {
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
    texture * checker = new checker_texture( new constant_texture(vec3(0.2,0.3,0.1)), new constant_texture(vec3(0.9,0.9,0.9)));
    int n = 2;
    hitable **list = new hitable*[n];
    list[0] = new sphere(vec3(0,-10,0),10, new lambertian(checker));
    list[1] = new sphere(vec3(0,10,0),10, new lambertian(checker));

    return new bvh_node(list,2,0.0,1.0);
}

bvh_node * noisy_spheres() {
    texture *pertext = new noise_texture(5);
    hitable **list = new hitable*[2];
    list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0,2,0),2,new lambertian(pertext));

    return new bvh_node(list,2,0.0,1.0);
}

vec3 random_in_unit_sphere(){
    vec3 p;
    do {
        p = 2.0*vec3(random_double(),random_double(),random_double()) - vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}

int max_depth = 50;

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if(world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;

        if(depth < max_depth && rec.mat_ptr->scatter(r,rec,attenuation,scattered)){
            return attenuation * color(scattered, world, depth+1);
        }
        else{
            return vec3(0,0,0);
        }
    } else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

int nx = 500;
int ny = 250;
int ns = 100;

bvh_node * world = noisy_spheres();

sf::Image image;

vec3 lookfrom(13,2,3);
vec3 lookat(0,0,0);
float dist_to_focus = 10.0;
float aperture = 0.0;

camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus,0.0,1.0);

void calculate(int start_y, int end_y, int start_x, int end_x)
{
    for(int j = start_y; j <= end_y; j++){
        for(int i = start_x; i <= end_x; i++){
            vec3 col(0,0,0);
            for(int s=0;s<ns;s++){
                float u = float(i+random_double()) / float(nx);
                float v = float(j+random_double()) / float(ny);
                ray r = cam.get_ray(u,v);
                col += color(r,world,0);
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

void throwThreads(int s_y, int e_y, int s_x, int e_x)
{
    sf::Thread t1(std::bind(calculate,s_y,(s_y+e_y)/2,s_x,(s_x+e_x)/2));
    sf::Thread t2(std::bind(calculate,s_y,(s_y+e_y)/2,(s_x+e_x)/2+1,e_x-1));
    sf::Thread t3(std::bind(calculate,(s_y+e_y)/2+1,e_y-1,s_x,(s_x+e_x)/2));
    sf::Thread t4(std::bind(calculate,(s_y+e_y)/2+1,e_y-1,(s_x+e_x)/2+1,e_x-1));

    t1.launch();
    t2.launch();
    t3.launch();
    t4.launch();
}

int main() {
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(nx, ny), "Ray Tracing");
    window.setFramerateLimit(10);

    image.create(nx, ny);

    sf::Thread t1(std::bind(throwThreads,0,ny/2,0,nx/2));
    sf::Thread t2(std::bind(throwThreads,0,ny/2,nx/2,nx));
    sf::Thread t3(std::bind(throwThreads,ny/2,ny,0,nx/2));
    sf::Thread t4(std::bind(throwThreads,ny/2,ny,nx/2,nx));

    t1.launch();
    t2.launch();
    t3.launch();
    t4.launch();

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
}

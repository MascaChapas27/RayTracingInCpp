#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "ray.hpp"
#include "random.hpp"

vec3 random_in_unit_disk() {
    vec3 p;
    do {
        p = 2.0*vec3(random_double(), random_double(),0)-vec3(1,1,0);
    } while(dot(p,p) >= 1.0);
    return p;
}

class camera {
    public:
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect_ratio, float aperture, float focus_dist, float t0, float t1) { // vfov is top to bottom in degrees
            time0 = t0;
            time1 = t1;
            float theta = vfov*3.1415926/180;
            float h = tan(theta/2);
            float viewport_height = 2.0*h;
            float viewport_width = aspect_ratio * viewport_height;

            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;

            lens_radius = aperture / 2;
        }

        ray get_ray(float s, float t) {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u*rd.x() + v*rd.y();
            float time = time0 + random_double()*(time1-time0);
            return ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset,time);
        }

        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        float time0, time1;
        float lens_radius;
};

#endif // __CAMERA_HPP__

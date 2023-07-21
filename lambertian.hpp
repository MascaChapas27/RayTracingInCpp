#ifndef __LAMBERTIAN_HPP__
#define __LAMBERTIAN_HPP__

#include "material.hpp"
#include "vec3.hpp"
#include "texture.hpp"

extern vec3 random_in_unit_sphere();

class lambertian : public material {
    public:
        lambertian(texture * a) : albedo(a) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
            vec3 target = rec.p + rec.normal + random_in_unit_sphere();
            scattered = ray(rec.p, target-rec.p,r_in.time());
            attenuation = albedo->value(0,0,rec.p);
            return true;
        }

        texture * albedo;
};

#endif // __LAMBERTIAN_HPP__

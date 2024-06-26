#ifndef __DIFFUSE_LIGHT_HPP__
#define __DIFFUSE_LIGHT_HPP__

#include "material.hpp"
#include "texture.hpp"

class diffuse_light : public material
{
    public:
        diffuse_light(texture *a) : emit(a) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {return false; }
        virtual vec3 emitted(float u, float v, const vec3& p) const { return emit->value(u, v, p); }

        texture *emit;
};

#endif

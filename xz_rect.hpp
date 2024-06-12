#ifndef __XZ_RECT_HPP__
#define __XZ_RECT_HPP__

#include "hitable.hpp"
#include "aabb.hpp"
#include "material.hpp"

class xz_rect : public hitable
{
    public:
        xz_rect() {}
        xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, material *mat) : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

        virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& box) const
        {
            box = aabb(vec3(x0, k-0.0001, z0), vec3(x1, k+0.0001, z1));
            return true;
        }

        float x0, x1, z0, z1, k;
        material *mp;
};

bool xz_rect::hit(const ray& r, float t0, float t1, hit_record& rec) const
{
    float t = (k-r.origin().y()) / r.direction().y();
    if(t < t0 || t > t1) return false;

    float x = r.origin().x() + t*r.direction().x();
    float z = r.origin().z() + t*r.direction().z();

    if(x < x0 || x > x1 || z < z0 || z > z1) return false;

    rec.u = (x-x0)/(x1-x0);
    rec.v = (z-z0)/(z1-z0);
    rec.t = t;
    rec.mat_ptr = mp;
    rec.p = r.point_at_parameter(t);
    rec.normal = vec3(0, 1, 0);
    return true;
}

#endif

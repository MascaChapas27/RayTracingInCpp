#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
    vec3 uv = unit_vector(v);
    float dt = dot(uv,n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
    if(discriminant > 0) {
        refracted = ni_over_nt*(uv-n*dt) - n*sqrt(discriminant);
        return true;
    }
    return false;
}
class material {
    public:
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
        virtual vec3 emitted(float u, float v, const vec3& p) const { return vec3(0,0,0); }
};

#endif // __MATERIAL_HPP__

#ifndef __VEC3_HPP__
#define __VEC3_HPP__

#include <cmath>
#include <stdlib.h>
#include <iostream>

class vec3 {

    public:
        vec3() {}
        vec3(float e0, float e1, float e2) {
            e[0] = e0;
            e[1] = e1;
            e[2] = e2;
        }
        float x() const {return e[0];}
        float y() const {return e[1];}
        float z() const {return e[2];}
        float r() const {return e[0];}
        float g() const {return e[1];}
        float b() const {return e[2];}

        const vec3& operator+() const {return *this;}
        vec3 operator-() const {return vec3(-e[0],-e[1],-e[2]);}
        float operator[](int i) const {return e[i];}
        float& operator[](int i) {return e[i];}

        vec3& operator+=(const vec3 &v2);
        vec3& operator-=(const vec3 &v2);
        vec3& operator*=(const vec3 &v2);
        vec3& operator/=(const vec3 &v2);
        vec3& operator*=(const float t);
        vec3& operator/=(const float t);
        bool operator==(const vec3 &vec2);

        float length() const{
            return sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);
        }
        float squared_length() const{
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }
        void make_unit_vector();

        float e[3];
};

std::istream& operator>>(std::istream &is, vec3 &t){
    is >> t.e[0] >> t.e[1] >> t.e[2];
    return is;
}

std::ostream& operator<<(std::ostream &os, const vec3 &t){
    os <<  t.e[0] << " " << t.e[1] << " " << t.e[2];
    return os;
}

void vec3::make_unit_vector() {
    float k = 1.0 / sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);
    e[0]*=k;
    e[1]*=k;
    e[2]*=k;
}

vec3 operator+(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

vec3 operator-(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

vec3 operator*(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

vec3 operator/(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

vec3 operator*(float t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

vec3 operator/(const vec3 &v, float t) {
    return vec3(v.e[0]/t, v.e[1]/t, v.e[2]/t);
}

vec3 operator*(const vec3 &v, float t) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

float dot(const vec3 &v1, const vec3 &v2){
    return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

vec3 cross(const vec3 &v1, const vec3 &v2){
    return vec3( (v1.e[1]*v2.e[2] - v1.e[2]*v2.e[1]),
                 (-(v1.e[0]*v2.e[2] - v1.e[2]*v2.e[0])),
                 (v1.e[0]*v2.e[1] - v1.e[1]*v2.e[0]));
}

vec3& vec3::operator+=(const vec3 &v){
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
}

vec3& vec3::operator*=(const vec3 &v){
    e[0] *= v.e[0];
    e[1] *= v.e[1];
    e[2] *= v.e[2];
    return *this;
}

vec3& vec3::operator/=(const vec3 &v){
    e[0] /= v.e[0];
    e[1] /= v.e[1];
    e[2] /= v.e[2];
    return *this;
}

vec3& vec3::operator-=(const vec3 &v){
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    e[2] -= v.e[2];
    return *this;
}

vec3& vec3::operator*=(const float t){
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
}

vec3& vec3::operator/=(const float t){
    float k = 1.0 / t;

    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
    return *this;
}

bool vec3::operator==(const vec3 &vec2){
    return vec2.r() == this->r() && vec2.g() == this->g() && vec2.b() == this->b();
}

vec3 unit_vector(vec3 v){
    return v / v.length();
}


#endif // __VEC3__

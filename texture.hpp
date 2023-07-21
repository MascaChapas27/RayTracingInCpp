#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

class texture {
    public:
        virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

#endif // __TEXTURE_HPP__


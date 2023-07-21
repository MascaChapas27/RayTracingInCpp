#ifndef __CONSTANT_TEXTURE_HPP__
#define __CONSTANT_TEXTURE_HPP__

#include "texture.hpp"

class constant_texture : public texture {
    public:
        constant_texture() {}
        constant_texture(vec3 c) : color(c) {}
        virtual vec3 value(float u, float v, const vec3& p) const {
            return color;
        }

        vec3 color;
};

#endif // __CONSTANT_TEXTURE_HPP__

#ifndef __RANDOM_HPP__
#define __RANDOM_HPP__

#include <cstdlib>

inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

#endif // __RANDOM_HPP__

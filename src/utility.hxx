#ifndef _UTILITY_HXX_
#define _UTILITY_HXX_

#include <random>

#include <qstring.h>

template<typename T> T generate_random_number(T min, T max) {
    std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<T> dist(min, max);
    return dist(gen);
}

QString generate_name();

#endif // _UTILITY_HXX_

/**
 * @file Float3.hpp
 * @author Kay Scheerer, Fabian Hauck, Timo Schrader
 * @brief Contains float3 struct definitions provided by GPU Lab Exercise 6
 * @version 1
 * @date 2021-12-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __N_BODY_SIMULATION_FLOAT3_HPP__
#define __N_BODY_SIMULATION_FLOAT3_HPP__

#include <algorithm>
#include <cmath>
#include <xmmintrin.h>

#ifdef ENABLE_SIMD
#define EPSILON 0.00001f
struct float3 {
    union {
        __m128 sseValues;
        struct {
            float x;
            float y;
            float z;
            float w;
        };
    };
    float3() = default;
    float3(float f) {
        this->x = this->y = this->z = f;
        this->w = EPSILON;
    }
    float3(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = EPSILON;
    }
    float3(const __m128 &vals) {
        this->sseValues = vals;
    }
    float3 operator+(const float3 &f) const {
        return float3(_mm_add_ps(this->sseValues, f.sseValues));
    }
    float3 operator-(const float3 &f) const {
        return float3(_mm_sub_ps(this->sseValues, f.sseValues));
    }
    float3 operator*(const float3 &f) const {
        return float3(_mm_mul_ps(this->sseValues, f.sseValues));
    }
    float3 operator*(const float &f) const {
        __m128 multiplicand = {f, f, f, EPSILON};
        return float3(_mm_mul_ps(this->sseValues, multiplicand));
    }
    float3 operator/(const float3 &f) const {
        return float3(_mm_div_ps(this->sseValues, f.sseValues));
    }
    float3 operator/(const float &f) const {
        __m128 denominator = {f, f, f, EPSILON};
        return float3(_mm_div_ps(this->sseValues, denominator));
    }
    float3 operator+=(const float3 &f) {
        this->sseValues = _mm_add_ps(this->sseValues, f.sseValues);
        return *this;
    }
};

#else

/**
 * Source: GPU Lab Exercise 06
 */
struct float3 {
    float x{}, y{}, z{};
    float3() = default;
    float3(float f) : x(f), y(f), z(f) {}
    float3(float x, float y, float z) : x(x), y(y), z(z) {}
    float3 operator+(const float3 &f) const {
        return float3(x + f.x, y + f.y, z + f.z);
    }
    float3 operator-(const float3 &f) const {
        return float3(x - f.x, y - f.y, z - f.z);
    }
    float3 operator*(const float3 &f) const {
        return float3(x * f.x, y * f.y, z * f.z);
    }
    float3 operator*(const float &f) const {
        return float3(x * f, y * f, z * f);
    }
    float3 operator/(const float3 &f) const {
        return float3(x / f.x, y / f.y, z / f.z);
    }
    float3 operator/(const float &f) const {
        return float3(x / f, y / f, z / f);
    }
    float3 operator+=(const float3 &f) {
        x += f.x;
        y += f.y;
        z += f.z;
        return *this;
    }
};

#endif

inline float dot(float3 f1, float3 f2) {
    return f1.x * f2.x + f1.y * f2.y + f1.z * f2.z;
}

inline float3 _min(float3 f1, float3 f2) {
    return float3((std::min) (f1.x, f2.x), (std::min) (f1.y, f2.y), (std::min) (f1.z, f2.z));
}
inline float3 _max(float3 f1, float3 f2) {
    return float3((std::max) (f1.x, f2.x), (std::max) (f1.y, f2.y), (std::max) (f1.z, f2.z));
}
inline float3 normalize(float3 f) {
    return f / std::sqrt(dot(f, f));
}
inline float distance(const float3 &f1, const float3 &f2) {
    return (float) std::sqrt(
            std::pow(f1.x - f2.x, 2) + std::pow(f1.y - f2.y, 2) + std::pow(f1.z - f2.z, 2));
}

#endif
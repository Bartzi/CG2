/*
    Copyright (C) 2007-2008 by Jan Eric Kyprianidis <www.kyprianidis.com>
    All rights reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef CGMATH_INCLUDED_TYPES_HPP
#define CGMATH_INCLUDED_TYPES_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <limits>
#include <iostream>

#ifdef M_PI
#undef M_PI
#endif

/// Global namespace of cgmath 
namespace cgmath {

    static const double PI    = 3.14159265358979323846;
    static const double PI_2  = 1.57079632679489661923;
    static const double PI_4  = 0.785398163397448309616;
    static const float EPSILON  = 1e-5f;

    inline double to_rad(double deg) { return deg * PI / 180.0; }
    inline double to_deg(double rad) { return rad * 180.0 / PI; }
    inline float to_rad(float deg) { return static_cast<float>(deg * PI / 180.0 ); }
    inline float to_deg(float rad) { return static_cast<float>(rad * 180.0 / PI); }

    template <typename T> inline T abs_value(T v) {
        return (v < 0)? -v : v;
    }

    template<typename T> inline T sign(T v) { 
        return v<0? (T)-1 : (T)1; 
    }

    inline bool is_valid(double v) {
        #ifdef _MSC_VER
            return _finite(v) != 0;
        #else
            return std::isnan(v) != 0;
        #endif
    }

    template <typename T> T min_value(T a, T b) {
        return (a < b)?  a :  b;
    }

    template <typename T> T max_value(T a, T b) {
        return (a > b)?  a :  b;
    }

    template <typename T> T clamp_value(T x, T a, T b) {
        return (x < a)?  a : ((x > b)? b : x);
    }

    inline bool almost_equal(float a, float b, float epsilon=EPSILON) {
        return abs(a - b) <= epsilon;
    }

    inline bool almost_equal(double a, double b, double epsilon=EPSILON) {
        return abs(a - b) <= epsilon;
    }

    template <typename T> inline T det2x2(T a, T b, T c, T d) {
        return (a * d - b * c);
    }

    template <typename T> inline T det3x3(T a1, T a2, T a3, T b1, T b2, T b3, T c1, T c2, T c3) {
        return (a1 * det2x2(b2, b3, c2, c3) - b1 * det2x2(a2, a3, c2, c3) + c1 * det2x2(a2, a3, b2, b3));
    }

}

#endif

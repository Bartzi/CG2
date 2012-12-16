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
#ifndef CGMATH_INCLUDED_VEC3_HPP
#define CGMATH_INCLUDED_VEC3_HPP

#include "types.hpp"

namespace cgmath {

    /// 3-dimensional vector template (T = float|double)
    template <typename T = double> class vec3 {
    public:
        enum { dim = 3 };
        typedef T value_type;

        vec3() { }

        vec3(T vx, T vy, T vz)
            : x(vx), y(vy), z(vz) { }

        template <typename U> vec3(const vec3<U>& src)
            : x(static_cast<T>(src.x)), y(static_cast<T>(src.y)), z(static_cast<T>(src.z)) { }

        template <typename U> explicit vec3(const U *src)
            : x(static_cast<T>(src[0])), y(static_cast<T>(src[1])), z(static_cast<T>(src[2])) { }

        T& operator[](int index) {
            return (&x)[index];
        }

        const T& operator[](int index) const {
            return (&x)[index];
        }

        template <typename U> void get(U *dst) const {
            dst[0] = static_cast<U>(x);
            dst[1] = static_cast<U>(y);
            dst[2] = static_cast<U>(z);
        }

        bool operator==(const vec3& v) const {
            return (x == v.x) && (y == v.y) && (z == v.z);
        }

        bool operator!=(const vec3& v) const {
            return !this->operator==(v);
        }

        bool is_valid() const {
            return cgmath::is_valid(x) && cgmath::is_valid(y) && cgmath::is_valid(z);
        }

        const vec3& operator+=(const vec3& v) {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        vec3 operator+(const vec3& v) const {
            return vec3(x + v.x, y + v.y, z + v.z);
        }

        const vec3& operator-=(const vec3& v) {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        vec3 operator-(const vec3& v) const {
            return vec3(x - v.x, y - v.y, z - v.z);
        }

        vec3 operator-() const {
            return vec3(-x, -y, -z);
        }

        const vec3& operator*=(T k) {
            x *= k;
            y *= k;
            z *= k;
            return *this;
        }

        const vec3& operator/=(T d) {
            return this->operator*=(1 / d);
        }

        vec3 operator/(T d) const {
            return operator*(*this, 1 / d);
        }

        T length2() const {
            return (x * x + y * y + z * z);
        }

        T length() const {
            return sqrt(length2());
        }

        T normalize() {
            T len = length();
            if (len > 0) this->operator/=(len);
            return len;
        }

        vec3 normalized() const {
            return this->operator/(length());
        }

        T x;
        T y;
        T z;
    };

    template <typename T> vec3<T> operator*(const vec3<T> v, T k) {
        return vec3<T>(v.x * k, v.y * k, v.z * k);
    }

    template <typename T> vec3<T> operator*(T k, const vec3<T>& v) {
        return vec3<T>(v.x * k, v.y * k, v.z * k);
    }

    template <typename T> T dot_prod(const vec3<T>& a, const vec3<T>& b){
        return (a.x * b.x + a.y * b.y + a.z * b.z);
    }

    template <typename T> vec3<T> cross_prod(const vec3<T>& a, const vec3<T>& b) {
        return vec3<T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    template <typename T> vec3<T> lerp(const vec3<T>& a, const vec3<T>& b, T t) {
        T u = 1 - t;
        return vec3<T>(u * a.x + t * b.x, u * a.y + t * b.y, u * a.z + t * b.z);
    }

    template <typename T> T dist2(const vec3<T>& a, const vec3<T>& b) {
        return vec3<T>(b - a).length2();
    }

    template <typename T> T dist(const vec3<T>& a, const vec3<T>& b) {
        return vec3<T>(b - a).length();
    }

    template <typename T> T angle_between(const vec3<T>& a, const vec3<T>& b) {
        return to_deg(atan2(cross_prod(a,b).length(), dot_prod(a,b)));
    }

    template <typename T> bool almost_equal(const vec3<T>& a, const vec3<T>& b, T epsilon=EPSILON) {
        return (
            almost_equal<T>(a.x, b.x, epsilon) &&
            almost_equal<T>(a.y, b.y, epsilon) &&
            almost_equal<T>(a.z, b.z, epsilon)
        );
    }

    template<typename T> std::ostream& operator<<(std::ostream& os, const vec3<T>& v) {
        return (os << v.x << " " << v.y << " " << v.z);
    }

    template<typename T> std::istream& operator>>(std::istream& is, vec3<T>& v) {
        return is >> v.x >> v.y >> v.z;
    }
}

#endif

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
#ifndef CGMATH_INCLUDED_MAT44_HPP
#define CGMATH_INCLUDED_MAT44_HPP

#include "vec3.hpp"

namespace cgmath {

    /// 4 x 4 matrix class (T=float|double)
    template <typename T> class Mat44 {
    public:
        typedef T value_type;

        Mat44() {}

        Mat44( T a00, T a01, T a02, T a03, 
               T a10, T a11, T a12, T a13,
               T a20, T a21, T a22, T a23,
               T a30, T a31, T a32, T a33) {
               m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
               m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
               m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
               m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33;
        }

        template<typename U> Mat44( const Mat44<U>& src ) {
            for (int i = 0; i < 4; ++i) 
                for (int j = 0; j < 4; ++j) m[i][j] = static_cast<T>(src.m[i*4+j]);
        }

        template<typename U> Mat44( const U *src, bool row_major=true ) {
            if (row_major) {
                for (int i = 0; i < 4; ++i) 
                    for (int j = 0; j < 4; ++j) m[i][j] = static_cast<T>(src[i*4+j]);
            } else {
                for (int i = 0; i < 4; ++i) 
                    for (int j = 0; j < 4; ++j) m[i][j] = static_cast<T>(src[i+j*4]);
            }
        }

        Mat44( const Mat44& A, const Mat44& B ) {
            for (int i = 0; i < 4; ++i) {
                m[i][0] =  A.m[i][0] * B.m[0][0] + A.m[i][1] * B.m[1][0] + A.m[i][2] * B.m[2][0] + A.m[i][3] * B.m[3][0];
                m[i][1] =  A.m[i][0] * B.m[0][1] + A.m[i][1] * B.m[1][1] + A.m[i][2] * B.m[2][1] + A.m[i][3] * B.m[3][1];
                m[i][2] =  A.m[i][0] * B.m[0][2] + A.m[i][1] * B.m[1][2] + A.m[i][2] * B.m[2][2] + A.m[i][3] * B.m[3][2];
                m[i][3] =  A.m[i][0] * B.m[0][3] + A.m[i][1] * B.m[1][3] + A.m[i][2] * B.m[2][3] + A.m[i][3] * B.m[3][3];
            }
        }

        bool operator==( const Mat44& rhs ) const {
            for (int i = 0; i < 4; ++i) 
                for (int j = 0; j < 4; ++j) if (m[i][j] != rhs.m[i][j]) return false;
            return true;
        }

        bool operator!=( const Mat44& rhs ) const {
            return !this->operator==(rhs);
        }

        T* operator[]( int row ) {
            return m[row];
        }

        const T* operator[](int row) const {
            return m[row];
        }

        template <typename U> void set(const U *src, bool row_major= true) {
            if (row_major) {
                for (int i = 0; i < 4; ++i) 
                    for (int j = 0; j < 4; ++j) m[i][j] = static_cast<float>(src[i*4+j]);
            } else {
                for (int i = 0; i < 4; ++i) 
                    for (int j = 0; j < 4; ++j) m[i][j] = static_cast<float>(src[i+j*4]);
            }
        }

        template <typename U>  void get(U *dst, bool row_major=true) const {
            if (row_major) {
                for (int i = 0; i < 4; ++i) 
                    for (int j = 0; j < 4; ++j) dst[i*4+j] = static_cast<float>(m[i][j]);
            } else {
                for (int i = 0; i < 4; ++i) 
                    for (int j = 0; j < 4; ++j) dst[i+j*4] = static_cast<float>(m[i][j]);
            }
        }

        Mat44 operator*( const Mat44& rhs ) const {
            return Mat44(*this, rhs);
        }

        const Mat44& operator*=(const Mat44& rhs) {
            return (*this = Mat44(*this, rhs));
        }

        Mat44& zero() {
            for (int i = 0; i < 4; ++i) 
                for (int j = 0; j < 4; ++j) m[i][j] = 0;
            return *this;
        }

        Mat44& identity() {
            for (int i = 0; i < 4; ++i) 
                for (int j = 0; j < 4; ++j) m[i][j] = (i == j)? 1 : 0;
            return *this;
        }

        Mat44& scale(T sx, T sy, T sz) {
            for (int i = 0; i < 4; ++i) {
                m[i][0] *= sx;
                m[i][1] *= sy;
                m[i][2] *= sz;
            }
            return *this;
        }

        Mat44& translate(T tx, T ty, T tz) {
            for (int i = 0; i < 3; ++i) {
                m[i][3] += tx * m[i][0] + ty * m[i][1] + tz * m[i][2];
            }
            return *this;
        }

        template <typename U> vec3<U> transform( const vec3<U>& v ) {
            U x = static_cast<U>(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z);
            U y = static_cast<U>(m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z);
            U z = static_cast<U>(m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
            U w = static_cast<U>(m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z);
            return vec3<U>(x / w, y / w, z / w);
        }

        template <typename U> U norm2() const {
            U n2 = 0;
            for (int i = 0; i < 4; ++i) 
                for (int j = 0; j < 4; ++j) n2 += m[i][j] * m[i][j];
            return n2;
        }

        template <typename U> U norm() const {
            return sqrt(norm2());
        }

        Mat44<T> transpose() {
            return Mat44<T>( m[0][0], m[1][0], m[2][0], m[3][0],
                             m[0][1], m[1][1], m[2][1], m[3][1],
                             m[0][2], m[1][2], m[2][2], m[3][2],
                             m[0][3], m[1][3], m[2][3], m[3][3] );
        }

        Mat44<T> invert_gauss_jordan() {
            Mat44<T> inv;
            inv.identity();

            for (int j = 0; j < 4; ++j) {
                int pv_idx = -1;
                double pv_val = 0;
                for (int i = j; i < 4; ++i) {
                    const double tmp = fabs(m[i][j]);
                    if (pv_val < tmp ) {
                        pv_val = tmp;
                        pv_idx = i;
                    }
                }
                if (pv_idx < 0) {
                    return inv;
                }

                if (pv_idx != j) {
                    for (int k = 0; k < 4; ++k) {
                        std::swap(m[j][k], m[pv_idx][k]);
                        std::swap(inv[j][k], inv[pv_idx][k]);
                    }
                }

                {
                    const double tmp = 1.0 / m[j][j];
                    for (int k = 0; k < 4; ++k) {
                        m[j][k] *= tmp;
                        inv[j][k] *= tmp;
                    }
                }

                for (int i = 0; i < 4; ++i) {
                    if (i != j) {
                        const double m_ij = m[i][j];
                        for (int k = 0; k < 4; ++k) {
                            m[i][k] -= m[j][k] * m_ij;
                            inv[i][k] -= inv[j][k] * m_ij;
                        }
                    }
                }
            }

            return inv;
        }

    public:
        T m[4][4];
    };




    /*
    template <typename T> std::ostream& operator<<( std::ostream& os, const mat44<T>& m ) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                os << m[i][j];
                if  ((i < 4) && (j < 4)) os << " ";
            }
        }
        return os;
    }

    template <typename T> std::istream& operator>>( std::istream& is, mat44<T>& m ) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                is >> m[i][j];
            }
        }
        return is;
    }
    */
} 

#endif

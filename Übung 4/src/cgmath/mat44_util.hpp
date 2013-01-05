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
#pragma once

namespace cgmath {

#if 0
    double mat44::det() const {
        return (m[0][0] * det3x3(m[1][1], m[1][2], m[1][3],  m[2][1], m[2][2], m[2][3],  m[3][1], m[3][2], m[3][3])
              - m[0][1] * det3x3(m[1][0], m[1][2], m[1][3],  m[2][0], m[2][2], m[2][3],  m[3][0], m[3][2], m[3][3])
              + m[0][2] * det3x3(m[1][0], m[1][1], m[1][3],  m[2][0], m[2][1], m[2][3],  m[3][0], m[3][1], m[3][3])
              - m[0][3] * det3x3(m[1][0], m[1][1], m[1][2],  m[2][0], m[2][1], m[2][2],  m[3][0], m[3][1], m[3][2]));
    }


    mat44& mat44::adjoint() {
        mat44 M(*this);

        m[0][0] =  det3x3(M[1][1], M[1][2], M[1][3],  M[2][1], M[2][2], M[2][3],  M[3][1], M[3][2], M[3][3]);
        m[0][1] = -det3x3(M[1][0], M[1][2], M[1][3],  M[2][0], M[2][2], M[2][3],  M[3][0], M[3][2], M[3][3]);
        m[0][2] =  det3x3(M[1][0], M[1][1], M[1][3],  M[2][0], M[2][1], M[2][3],  M[3][0], M[3][1], M[3][3]);
        m[0][3] = -det3x3(M[1][0], M[1][1], M[1][2],  M[2][0], M[2][1], M[2][2],  M[3][0], M[3][1], M[3][2]);

        m[1][0] = -det3x3(M[0][1], M[0][2], M[0][3],  M[2][1], M[2][2], M[2][3],  M[3][1], M[3][2], M[3][3]);
        m[1][1] =  det3x3(M[0][0], M[0][2], M[0][3],  M[2][0], M[2][2], M[2][3],  M[3][0], M[3][2], M[3][3]);
        m[1][2] = -det3x3(M[0][0], M[0][1], M[0][3],  M[2][0], M[2][1], M[2][3],  M[3][0], M[3][1], M[3][3]);
        m[1][3] =  det3x3(M[0][0], M[0][1], M[0][2],  M[2][0], M[2][1], M[2][2],  M[3][0], M[3][1], M[3][2]);

        m[2][0] =  det3x3(M[0][1], M[0][2], M[0][3],  M[1][1], M[1][2], M[1][3],  M[3][1], M[3][2], M[3][3]);
        m[2][1] = -det3x3(M[0][0], M[0][2], M[0][3],  M[1][0], M[1][2], M[1][3],  M[3][0], M[3][2], M[3][3]);
        m[2][2] =  det3x3(M[0][0], M[0][1], M[0][3],  M[1][0], M[1][1], M[1][3],  M[3][0], M[3][1], M[3][3]);
        m[2][3] = -det3x3(M[0][0], M[0][1], M[0][2],  M[1][0], M[1][1], M[1][2],  M[3][0], M[3][1], M[3][2]);

        m[3][0] = -det3x3(M[0][1], M[0][2], M[0][3],  M[1][1], M[1][2], M[1][3],  M[2][1], M[2][2], M[2][3]);
        m[3][1] =  det3x3(M[0][0], M[0][2], M[0][3],  M[1][0], M[1][2], M[1][3],  M[2][0], M[2][2], M[2][3]);
        m[3][2] = -det3x3(M[0][0], M[0][1], M[0][3],  M[1][0], M[1][1], M[1][3],  M[2][0], M[2][1], M[2][3]);
        m[3][3] =  det3x3(M[0][0], M[0][1], M[0][2],  M[1][0], M[1][1], M[1][2],  M[2][0], M[2][1], M[2][2]);

        return *this;
    }


    bool mat44::invert_gauss_jordan() {
        mat44 inv;
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
                return false;
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

        *this = inv;
        return true;
    }


    bool mat44::invert_direct() {
        double r0[4];
        for (int j = 0; j < 4; ++j) {
            r0[j] = m[0][j];
        }
        this->adjoint();

        double determinat = 0;
        for (int j = 0; j < 4; ++j) {
            determinat += r0[j] * m[0][j];
        }
        if (determinat == 0) {
            return false;
        }

        double s = 1 / determinat;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] *= s;    
            }
        }
        return true;
    }


/*
    Wu, Kevin, Fast Matrix Inversion, Graphics Gems II, p. 342-350

    Computes the inverse of a 3D affine matrix; i.e. a matrix with a dimen-
    sionality of 4 where the right column has the entries (0, 0, 0, 1).

    This procedure treats the 4 by 4 matrix as a block matrix and
    calculates the inverse of one submatrix for a significant perform-
    ance improvement over a general procedure that can invert any non-
    singular matrix:
              --        --          --          --
              |          | -1       |    -1      |
              | A      0 |          |   A      0 |
        -1    |          |          |            |
       M   =  |          |     =    |     -1     |
              | C      1 |          | -C A     1 |
              |          |          |            |
              --        --          --          --

      where     M is a 4 by 4 matrix,
                A is the 3 by 3 upper left submatrix of M,
                C is the 1 by 3 lower left submatrix of M.

    Returned value:
      true   if input matrix is nonsingular
      false  otherwise
*/
    bool mat44::invert_affine() {
        mat44 in(*this);
        double det_1;
        double pos, neg, temp;

        // Calculate the determinant of submatrix A and determine if the
        // the matrix is singular as limited by the double precision
        // floating-point data representation.
        pos = neg = 0.0;
        for (int i = 0; i < 3; ++i) {
            temp =  in[i][0] * in[(i+1)%3][1] * in[(i+2)%3][2];
            if (temp >= 0.0) 
                pos += temp; 
            else 
                neg += temp;

            temp =  -in[i][0] * in[(i+2)%3][1] * in[(i+1)%3][2];
            if (temp >= 0.0) 
                pos += temp; 
            else 
                neg += temp;
        }

        det_1 = pos + neg;

        // Is the submatrix A singular?
        if ((det_1 == 0.0) || (fabs(det_1 / (pos - neg)) < 1e-15)) {
            return false;
        }
        else {

            // Calculate inverse(A) = adj(A) / det(A) 
            det_1 = 1.0 / det_1;
            m[0][0] =   ( in[1][1] * in[2][2] - in[2][1] * in[1][2] ) * det_1;
            m[0][1] = - ( in[0][1] * in[2][2] - in[2][1] * in[0][2] ) * det_1;
            m[0][2] =   ( in[0][1] * in[1][2] - in[1][1] * in[0][2] ) * det_1;
            m[1][0] = - ( in[1][0] * in[2][2] - in[2][0] * in[1][2] ) * det_1;
            m[1][1] =   ( in[0][0] * in[2][2] - in[2][0] * in[0][2] ) * det_1;
            m[1][2] = - ( in[0][0] * in[1][2] - in[1][0] * in[0][2] ) * det_1;
            m[2][0] =   ( in[1][0] * in[2][1] - in[2][0] * in[1][1] ) * det_1;
            m[2][1] = - ( in[0][0] * in[2][1] - in[2][0] * in[0][1] ) * det_1;
            m[2][2] =   ( in[0][0] * in[1][1] - in[1][0] * in[0][1] ) * det_1;

            // Calculate -C * inverse(A)
            m[0][3] = - ( in[0][3] * m[0][0] + in[1][3] * m[0][1] + in[2][3] * m[0][2] );
            m[1][3] = - ( in[0][3] * m[1][0] + in[1][3] * m[1][1] + in[2][3] * m[1][2] );
            m[2][3] = - ( in[0][3] * m[2][0] + in[1][3] * m[2][1] + in[2][3] * m[2][2] );

            // Fill in last column 
            m[3][0] = m[3][1] = m[3][2] = 0.0;
            m[3][3] = 1.0;

            return true;
        }
    }

    mat44& mat44::ortho_project( double left, double right, double bottom, double top, double z_near, double z_far ) {
        mat44 M;
        M[0][0] = 2.0 / (right - left);
        M[0][3] = (right + left) / (right - left);
        M[1][1] = 2.0 / (top - bottom);
        M[1][3] = (top + bottom) / (top - bottom);
        M[2][2] = -2.0 / (z_far - z_near);
        M[2][3] = (z_far + z_near) / (z_far - z_near);
        this->operator*=(M);
        return *this;
    }


    mat44& mat44::persp_project( double fov, double aspect_ratio, double z_near, double z_far ) {
        double w = z_near * tan(0.5 * radians(fov));
        double h = w / aspect_ratio;
        mat44 M;
        M[0][0] = 2.0 * z_near / w;
        M[1][1] = 2.0 * z_near / h;
        M[2][2] = -(z_far + z_near) / (z_far - z_near);
        M[2][3] = -2.0 * z_far * z_near / (z_far - z_near);
        M[3][2] = -1.0;
        M[3][3] = 0.0;
        this->operator*=(M);
        return *this;
    }

    
    mat44& mat44::frustum( double left, double right, double bottom, double top, double z_near, double z_far ) {
        mat44 M;
        M[0][0] = 2.0 * z_near / (right - left);
        M[0][2] = (right + left) / (right - left);
        M[1][1] = 2.0 * z_near / (top - bottom);
        M[1][2] = (top + bottom) / (top - bottom);
        M[2][2] = -(z_far + z_near) / (z_far - z_near);
        M[2][3] = -2.0 * z_far * z_near / (z_far - z_near);
        M[3][2] = -1.0;
        M[3][3] = 0.0;
        this->operator*=(M);
        return *this;
    }


    /* TODO
    mat44 look_at( const vec<3,double>& pos, const vec<3,double>& tgt, const vec<3,double>& up )  {
        vec<3,double> n = normalize(tgt - pos);
        vec<3,double> r = cross_prod(n, normalize(up));
        vec<3,double> u = cross_prod(r, n);
        return mat44(
            r.x, u.x, n.x, pos.x,
            r.y, u.y, n.y, pos.y,
            r.z, u.z, n.z, pos.z,
            0,   0,   0,   1
        );
    }
    */
    #endif

}

#pragma once

#include "csm_define.hpp"

CSM_PUBLIC_SCOPE
{
    template<typename T>
    union Mat4Base
    {
        struct
        {
            T x00,x01,x02,x03; // Col0
            T x10,x11,x12,x13; // Col1
            T x20,x21,x22,x23; // Col2
            T x30,x31,x32,x33; // Col3
        };

        T x[4][4];
        T raw[4*4];

        Mat4Base()
        {
            // Nothing...
        }
        Mat4Base(T x00_, T x01_, T x02_, T x03_, T x10_, T x11_, T x12_, T x13_, T x20_, T x21_, T x22_, T x23_, T x30_, T x31_, T x32_, T x33_)
        {
            x00 = x00_, x01 = x01_, x02 = x02_, x03 = x03_;
            x10 = x10_, x11 = x11_, x12 = x12_, x13 = x13_;
            x20 = x20_, x21 = x21_, x22 = x22_, x23 = x23_;
            x30 = x30_, x31 = x31_, x32 = x32_, x33 = x33_;
        }
        Mat4Base(const Mat4Base& m)
        {
            x00 = m.x00, x01 = m.x01, x02 = m.x02, x03 = m.x03;
            x10 = m.x10, x11 = m.x11, x12 = m.x12, x13 = m.x13;
            x20 = m.x20, x21 = m.x21, x22 = m.x22, x23 = m.x23;
            x30 = m.x30, x31 = m.x31, x32 = m.x32, x33 = m.x33;
        }
        explicit Mat4Base(T s)
        {
            x00 = s, x01 = s, x02 = s, x03 = s;
            x10 = s, x11 = s, x12 = s, x13 = s;
            x20 = s, x21 = s, x22 = s, x23 = s;
            x30 = s, x31 = s, x32 = s, x33 = s;
        }

        bool operator==(const Mat4Base& m) const
        {
            for(s32 c=0; c<4; ++c)
                for(s32 r=0; r<4; ++r)
                    if(x[c][r] != m.x[c][r])
                        return false;
            return true;
        }
        bool operator!=(const Mat4Base& m) const
        {
            return !(operator==(m));
        }

        Mat4Base<T>& operator+=(const Mat4Base& m)
        {
            for(s32 c=0; c<4; ++c)
                for(s32 r=0; r<4; ++r)
                    x[c][r] += m.x[c][r];
            return *this;
        }
        Mat4Base<T>& operator-=(const Mat4Base& m)
        {
            for(s32 c=0; c<4; ++c)
                for(s32 r=0; r<4; ++r)
                    x[c][r] -= m.x[c][r];
            return *this;
        }
        Mat4Base<T>& operator/=(const Mat4Base& m)
        {
            for(s32 c=0; c<4; ++c)
                for(s32 r=0; r<4; ++r)
                    x[c][r] /= m.x[c][r];
            return *this;
        }
        Mat4Base<T>& operator*=(const Mat4Base& m)
        {
            Mat4Base<T> result;
            for(s32 c=0; c<4; ++c)
                for(s32 r0=0; r0<4; ++r0)
                    for(s32 r1=0; r1<4; ++r1)
                        result[c][r0] += x[c][r1] * m.x[r1][r0];
            this = result;
            return *this;
        }

        Mat4Base<T>& operator+=(const T& s)
        {
            for(s32 c=0; c<4; ++c)
                for(s32 r=0; r<4; ++r)
                    x[c][r] += s;
            return *this;
        }
        Mat4Base<T>& operator-=(const T& s)
        {
            for(s32 c=0; c<4; ++c)
                for(s32 r=0; r<4; ++r)
                    x[c][r] -= s;
            return *this;
        }
        Mat4Base<T>& operator/=(const T& s)
        {
            for(s32 c=0; c<4; ++c)
                for(s32 r=0; r<4; ++r)
                    x[c][r] /= s;
            return *this;
        }
        Mat4Base<T>& operator*=(const T& s)
        {
            for(s32 c=0; c<4; ++c)
                for(s32 r=0; r<4; ++r)
                    x[c][r] *= s;
            return *this;
        }

        Mat4Base<T> operator+(const Mat4Base& m) const
        {
            Mat4Base<T> r(*this);
            return r += m;
        }
        Mat4Base<T> operator-(const Mat4Base& m) const
        {
            Mat4Base<T> r(*this);
            return r -= m;
        }
        Mat4Base<T> operator/(const Mat4Base& m) const
        {
            Mat4Base<T> r(*this);
            return r /= m;
        }
        Mat4Base<T> operator*(const Mat4Base& m) const
        {
            Mat4Base<T> r(*this);
            return r *= m;
        }

        Mat4Base<T> operator+(const T& s) const
        {
            Mat4Base<T> r(*this);
            return r += s;
        }
        Mat4Base<T> operator-(const T& s) const
        {
            Mat4Base<T> r(*this);
            return r -= s;
        }
        Mat4Base<T> operator/(const T& s) const
        {
            Mat4Base<T> r(*this);
            return r /= s;
        }
        Mat4Base<T> operator*(const T& s) const
        {
            Mat4Base<T> r(*this);
            return r *= s;
        }

        Vec4Base<T> operator*(const Vec4Base<T>& v) const
        {
            Vec4Base<T> result;
            for(s32 c=0; c<4; ++c)
                for(s32 r=0; r<4; ++r)
                    result[c] += x[c][r] * v.raw[r];
            return result;
        }

        void Identity()
        {
            *this = Mat4Base<T>(0);
            x00 = 1, x11 = 1, x22 = 1, x33 = 1;
        }
    };

    template<typename T>
    Mat4Base<T> Translate(const Mat4Base<T>& m, T x, T y, T z = 0.0f)
    {
        Mat4Base<T> result(m);
        result.x30 = m.x00 * x + m.x10 * y + m.x20 * z + m.x30;
        result.x31 = m.x01 * x + m.x11 * y + m.x21 * z + m.x31;
        result.x32 = m.x02 * x + m.x12 * y + m.x22 * z + m.x32;
        return result;
    }

    template<typename T>
    Mat4Base<T> Rotate(const Mat4Base<T>& m, f32 radians, T x, T y, T z)
    {
        const f32 a = radians;
        const f32 c = cosf(a);
        const f32 s = sinf(a);

        Vec3Base<T> axis(Normalize(Vec3Base<T>(x,y,z))); // Ensure that the rotation axis is unit length.
        Vec3Base<T> t((1.0f-c)*axis.x, (1.0f-c)*axis.y, (1.0f-c)*axis.z);

        Mat4Base<T> r;
        r.x00 = c + t.x * axis.x;
        r.x01 = t.x * axis.y + s * axis.z;
        r.x02 = t.x * axis.z - s * axis.y;
        r.x10 = t.y * axis.x - s * axis.z;
        r.x11 = c + t.y * axis.y;
        r.x12 = t.y * axis.z + s * axis.x;
        r.x20 = t.z * axis.x + s * axis.y;
        r.x21 = t.z * axis.y - s * axis.x;
        r.x22 = c + t.z * axis.z;

        Mat4Base<T> result(m);
        result.x00 = m.x00 * r.x00 + m.x10 * r.x01 + m.x20 * r.x02;
        result.x01 = m.x01 * r.x00 + m.x11 * r.x01 + m.x21 * r.x02;
        result.x02 = m.x02 * r.x00 + m.x12 * r.x01 + m.x22 * r.x02;
        result.x10 = m.x00 * r.x10 + m.x10 * r.x11 + m.x20 * r.x12;
        result.x11 = m.x01 * r.x10 + m.x11 * r.x11 + m.x21 * r.x12;
        result.x12 = m.x02 * r.x10 + m.x12 * r.x11 + m.x22 * r.x12;
        result.x20 = m.x00 * r.x20 + m.x10 * r.x21 + m.x20 * r.x22;
        result.x21 = m.x01 * r.x20 + m.x11 * r.x21 + m.x21 * r.x22;
        result.x22 = m.x02 * r.x20 + m.x12 * r.x21 + m.x22 * r.x22;
        return result;
    }

    template<typename T>
    Mat4Base<T> Scale(const Mat4Base<T>& m, T x, T y, T z = 1.0f)
    {
        Mat4Base<T> result(m);
        result.x00 *= x;
        result.x01 *= x;
        result.x02 *= x;
        result.x10 *= y;
        result.x11 *= y;
        result.x12 *= y;
        result.x20 *= z;
        result.x21 *= z;
        result.x22 *= z;
        return result;
    }

    template<typename T>
    Mat4Base<T> Orthographic(T l, T r, T b, T t, T n = 0.0f, T f = 1.0f)
    {
        Mat4Base<T> result;
        result.Identity();
        result.x[0][0] =  CS_CAST(T,2) / (r - l);
        result.x[1][1] =  CS_CAST(T,2) / (t - b);
        result.x[3][0] = -(r + l) / (r - l);
        result.x[3][1] = -(t + b) / (t - b);
        result.x[2][2] = -CS_CAST(T,1) / (f - n);
        result.x[3][2] = -n / (f - n);
        return result;
    }

    template<typename T>
    Mat4Base<T> Perspective(T fov, T aspect, T nearZ, T farZ)
    {
        const T tanHalfFov = tan(fov / CS_CAST(T,2));
        Mat4Base<T> result(0);
        result.x[0][0] =  CS_CAST(T,1) / (aspect * tanHalfFov);
        result.x[1][1] =  CS_CAST(T,1) / (tanHalfFov);
        result.x[2][3] = -CS_CAST(T,1);
        result.x[2][2] =  farZ / (nearZ - farZ);
        result.x[3][2] = -(farZ * nearZ) / (farZ - nearZ);
        return result;
    }

    typedef Mat4Base<f32> Mat4;
    typedef Mat4Base<s32> Mat4i;
}

#pragma once

#include "csm_define.hpp"

CSM_PUBLIC_SCOPE
{
    template<typename T>
    union Mat3Base
    {
        struct
        {
            T x00,x01,x02; // Col0
            T x10,x11,x12; // Col1
            T x20,x21,x22; // Col2
        };

        T x[3][3];
        T raw[3*3];

        Mat3Base()
        {
            // Nothing...
        }
        Mat3Base(T x00_, T x01_, T x02_, T x10_, T x11_, T x12_, T x20_, T x21_, T x22_)
        {
            x00 = x00_, x01 = x01_, x02 = x02_;
            x10 = x10_, x11 = x11_, x12 = x12_;
            x20 = x20_, x21 = x21_, x22 = x22_;
        }
        Mat3Base(const Mat3Base& m)
        {
            x00 = m.x00, x01 = m.x01, x02 = m.x02;
            x10 = m.x10, x11 = m.x11, x12 = m.x12;
            x20 = m.x20, x21 = m.x21, x22 = m.x22;
        }
        explicit Mat3Base(T s)
        {
            x00 = s, x01 = s, x02 = s;
            x10 = s, x11 = s, x12 = s;
            x20 = s, x21 = s, x22 = s;
        }

        bool operator==(const Mat3Base& m) const
        {
            for(s32 c=0; c<3; ++c)
                for(s32 r=0; r<3; ++r)
                    if(x[c][r] != m.x[c][r])
                        return false;
            return true;
        }
        bool operator!=(const Mat3Base& m) const
        {
            return !(operator==(m));
        }

        Mat3Base<T>& operator+=(const Mat3Base& m)
        {
            for(s32 c=0; c<3; ++c)
                for(s32 r=0; r<3; ++r)
                    x[c][r] += m.x[c][r];
            return *this;
        }
        Mat3Base<T>& operator-=(const Mat3Base& m)
        {
            for(s32 c=0; c<3; ++c)
                for(s32 r=0; r<3; ++r)
                    x[c][r] -= m.x[c][r];
            return *this;
        }
        Mat3Base<T>& operator/=(const Mat3Base& m)
        {
            for(s32 c=0; c<3; ++c)
                for(s32 r=0; r<3; ++r)
                    x[c][r] /= m.x[c][r];
            return *this;
        }
        Mat3Base<T>& operator*=(const Mat3Base& m)
        {
            Mat3Base<T> result;
            for(s32 c=0; c<3; ++c)
                for(s32 r0=0; r0<3; ++r0)
                    for(s32 r1=0; r1<3; ++r1)
                        result[c][r0] += x[c][r1] * m.x[r1][r0];
            this = result;
            return *this;
        }

        Mat3Base<T>& operator+=(const T& s)
        {
            for(s32 c=0; c<3; ++c)
                for(s32 r=0; r<3; ++r)
                    x[c][r] += s;
            return *this;
        }
        Mat3Base<T>& operator-=(const T& s)
        {
            for(s32 c=0; c<3; ++c)
                for(s32 r=0; r<3; ++r)
                    x[c][r] -= s;
            return *this;
        }
        Mat3Base<T>& operator/=(const T& s)
        {
            for(s32 c=0; c<3; ++c)
                for(s32 r=0; r<3; ++r)
                    x[c][r] /= s;
            return *this;
        }
        Mat3Base<T>& operator*=(const T& s)
        {
            for(s32 c=0; c<3; ++c)
                for(s32 r=0; r<3; ++r)
                    x[c][r] *= s;
            return *this;
        }

        Mat3Base<T> operator+(const Mat3Base& m) const
        {
            Mat3Base<T> r(*this);
            return r += m;
        }
        Mat3Base<T> operator-(const Mat3Base& m) const
        {
            Mat3Base<T> r(*this);
            return r -= m;
        }
        Mat3Base<T> operator/(const Mat3Base& m) const
        {
            Mat3Base<T> r(*this);
            return r /= m;
        }
        Mat3Base<T> operator*(const Mat3Base& m) const
        {
            Mat3Base<T> r(*this);
            return r *= m;
        }

        Mat3Base<T> operator+(const T& s) const
        {
            Mat3Base<T> r(*this);
            return r += s;
        }
        Mat3Base<T> operator-(const T& s) const
        {
            Mat3Base<T> r(*this);
            return r -= s;
        }
        Mat3Base<T> operator/(const T& s) const
        {
            Mat3Base<T> r(*this);
            return r /= s;
        }
        Mat3Base<T> operator*(const T& s) const
        {
            Mat3Base<T> r(*this);
            return r *= s;
        }

        Vec3Base<T> operator*(const Vec3Base<T>& v) const
        {
            Vec3Base<T> result;
            for(s32 c=0; c<3; ++c)
                for(s32 r=0; r<3; ++r)
                    result[c] += x[c][r] * v.raw[r];
            return result;
        }

        void Identity()
        {
            *this = {};
            x00 = 1, x11 = 1, x22 = 1;
        }
    };

    typedef Mat3Base<f32> Mat3;
    typedef Mat3Base<s32> Mat3i;
}

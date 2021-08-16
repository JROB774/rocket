#pragma once

#include "csm_define.hpp"

CSM_PUBLIC_SCOPE
{
    template<typename T>
    union Mat2Base
    {
        struct
        {
            T x00,x01; // Col0
            T x10,x11; // Col1
        };

        T x[2][2];
        T raw[2*2];

        Mat2Base()
        {
            // Nothing...
        }
        Mat2Base(T x00_, T x01_, T x10_, T x11_)
        {
            x00 = x00_, x01 = x01_;
            x10 = x10_, x11 = x11_;
        }
        Mat2Base(const Mat2Base& m)
        {
            x00 = m.x00, x01 = m.x01;
            x10 = m.x10, x11 = m.x11;
        }
        explicit Mat2Base(T s)
        {
            x00 = s, x01 = s;
            x10 = s, x11 = s;
        }

        bool operator==(const Mat2Base& m) const
        {
            for(s32 c=0; c<2; ++c)
                for(s32 r=0; r<2; ++ir)
                    if(x[c][r] != m.x[c][r])
                        return false;
            return true;
        }
        bool operator!=(const Mat2Base& m) const
        {
            return !(operator==(m));
        }

        Mat2Base<T>& operator+=(const Mat2Base& m)
        {
            for(s32 c=0; c<2; ++c)
                for(s32 r=0; r<2; ++r)
                    x[c][r] += m.x[c][r];
            return *this;
        }
        Mat2Base<T>& operator-=(const Mat2Base& m)
        {
            for(s32 c=0; c<2; ++c)
                for(s32 r=0; r<2; ++r)
                    x[c][r] -= m.x[c][r];
            return *this;
        }
        Mat2Base<T>& operator/=(const Mat2Base& m)
        {
            for(s32 c=0; c<2; ++c)
                for(s32 r=0; r<2; ++r)
                    x[c][r] /= m.x[c][r];
            return *this;
        }
        Mat2Base<T>& operator*=(const Mat2Base& m)
        {
            Mat2Base<T> result;
            for(s32 c=0; c<2; ++c)
                for(s32 r0=0; r0<2; ++r0)
                    for(s32 r1=0; r1<2; ++r1)
                        result[c][r0] += x[c][r1] * m.x[r1][r0];
            this = result;
            return *this;
        }

        Mat2Base<T>& operator+=(const T& s)
        {
            for(s32 c=0; c<2; ++c)
                for(s32 r=0; r<2; ++r)
                    x[c][r] += s;
            return *this;
        }
        Mat2Base<T>& operator-=(const T& s)
        {
            for(s32 c=0; c<2; ++c)
                for(s32 r=0; r<2; ++r)
                    x[c][r] -= s;
            return *this;
        }
        Mat2Base<T>& operator/=(const T& s)
        {
            for(s32 c=0; c<2; ++c)
                for(s32 r=0; r<2; ++r)
                    x[c][r] /= s;
            return *this;
        }
        Mat2Base<T>& operator*=(const T& s)
        {
            for(s32 c=0; c<2; ++c)
                for(s32 r=0; r<2; ++r)
                    x[c][r] *= s;
            return *this;
        }

        Mat2Base<T> operator+(const Mat2Base& m) const
        {
            Mat2Base<T> r(*this);
            return r += m;
        }
        Mat2Base<T> operator-(const Mat2Base& m) const
        {
            Mat2Base<T> r(*this);
            return r -= m;
        }
        Mat2Base<T> operator/(const Mat2Base& m) const
        {
            Mat2Base<T> r(*this);
            return r /= m;
        }
        Mat2Base<T> operator*(const Mat2Base& m) const
        {
            Mat2Base<T> r(*this);
            return r *= m;
        }

        Mat2Base<T> operator+(const T& s) const
        {
            Mat2Base<T> r(*this);
            return r += s;
        }
        Mat2Base<T> operator-(const T& s) const
        {
            Mat2Base<T> r(*this);
            return r -= s;
        }
        Mat2Base<T> operator/(const T& s) const
        {
            Mat2Base<T> r(*this);
            return r /= s;
        }
        Mat2Base<T> operator*(const T& s) const
        {
            Mat2Base<T> r(*this);
            return r *= s;
        }

        Vec2Base<T> operator*(const Vec2Base<T>& v) const
        {
            Vec2Base<T> result;
            for(s32 c=0; c<2; ++c)
                for(s32 r=0; r<2; ++r)
                    result[c] += x[c][r] * v.raw[r];
            return result;
        }

        void Identity()
        {
            *this = {};
            x00 = 1, x11 = 1;
        }
    };

    typedef Mat2Base<f32> Mat2;
    typedef Mat2Base<s32> Mat2i;
}

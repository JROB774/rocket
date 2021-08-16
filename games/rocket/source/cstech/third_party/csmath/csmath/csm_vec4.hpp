#pragma once

#include "csm_define.hpp"

CSM_PUBLIC_SCOPE
{
    template<typename T>
    union Vec4Base
    {
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
        struct { T s, t, p, q; };

        T raw[4];

        Vec4Base()
        {
            // Nothing...
        }
        Vec4Base(T x_, T y_, T z_, T w_)
        {
            x = x_, y = y_, z = z_, w = w_;
        }
        Vec4Base(const Vec4Base& v)
        {
            x = v.x, y = v.y, z = v.z, w = v.w;
        }
        explicit Vec4Base(T s)
        {
            x = s, y = s, z = s, w = s;
        }

        bool operator==(const Vec4Base& v) const
        {
            return (x == v.x && y == v.y && z == v.z && w == v.w);
        }
        bool operator!=(const Vec4Base& v) const
        {
            return !(operator==(v));
        }

        Vec4Base<T>& operator+=(const Vec4Base& v)
        {
            x += v.x, y += v.y, z += v.z, w += v.w;
            return *this;
        }
        Vec4Base<T>& operator-=(const Vec4Base& v)
        {
            x -= v.x, y -= v.y, z -= v.z, w -= v.w;
            return *this;
        }
        Vec4Base<T>& operator/=(const Vec4Base& v)
        {
            x /= v.x, y /= v.y, z /= v.z, w /= v.w;
            return *this;
        }
        Vec4Base<T>& operator*=(const Vec4Base& v)
        {
            x *= v.x, y *= v.y, z *= v.z, w *= v.w;
            return *this;
        }

        Vec4Base<T>& operator+=(const T& s)
        {
            x += s, y += s, z += s, w += s;
            return *this;
        }
        Vec4Base<T>& operator-=(const T& s)
        {
            x -= s, y -= s, z -= s, w -= s;
            return *this;
        }
        Vec4Base<T>& operator/=(const T& s)
        {
            x /= s, y /= s, z /= s, w /= s;
            return *this;
        }
        Vec4Base<T>& operator*=(const T& s)
        {
            x *= s, y *= s, z *= s, w *= s;
            return *this;
        }

        Vec4Base<T> operator+(const Vec4Base& v) const
        {
            Vec4Base<T> r(*this);
            return r += v;
        }
        Vec4Base<T> operator-(const Vec4Base& v) const
        {
            Vec4Base<T> r(*this);
            return r -= v;
        }
        Vec4Base<T> operator/(const Vec4Base& v) const
        {
            Vec4Base<T> r(*this);
            return r /= v;
        }
        Vec4Base<T> operator*(const Vec4Base& v) const
        {
            Vec4Base<T> r(*this);
            return r *= v;
        }

        Vec4Base<T> operator+(const T& s) const
        {
            Vec4Base<T> r(*this);
            return r += s;
        }
        Vec4Base<T> operator-(const T& s) const
        {
            Vec4Base<T> r(*this);
            return r -= s;
        }
        Vec4Base<T> operator/(const T& s) const
        {
            Vec4Base<T> r(*this);
            return r /= s;
        }
        Vec4Base<T> operator*(const T& s) const
        {
            Vec4Base<T> r(*this);
            return r *= s;
        }
    };

    template<typename T>
    Vec4Base<T> Normalize(const Vec4Base<T>& v)
    {
        T length = Length(v);
        if (length == 0) return v;
        return (v / length);
    }

    template<typename T>
    Vec4Base<T> Absolute(const Vec4Base<T>& v)
    {
        return Vec4Base<T>(abs(v.x), abs(v.y), abs(v.z), abs(v.w));
    }

    template<typename T>
    T Length(const Vec4Base<T>& v)
    {
        return CSM_CAST(T, sqrt((v.x*v.y) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w)));
    }

    template<typename T>
    T Dot(const Vec4Base<T>& v)
    {
        return ((a.x*b.x) + (a.y*b.y) + (a.z*b.z) + (a.w*b.w));
    }

    typedef Vec4Base<f32> Vec4;
    typedef Vec4Base<s32> Vec4i;
}

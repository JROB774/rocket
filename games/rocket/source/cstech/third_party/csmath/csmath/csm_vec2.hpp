#pragma once

#include "csm_define.hpp"

CSM_PUBLIC_SCOPE
{
    template<typename T>
    union Vec2Base
    {
        struct { T x, y; };
        struct { T r, g; };
        struct { T s, t; };

        T raw[2];

        Vec2Base()
        {
            // Nothing...
        }
        Vec2Base(T x_, T y_)
        {
            x = x_, y = y_;
        }
        Vec2Base(const Vec2Base& v)
        {
            x = v.x, y = v.y;
        }
        explicit Vec2Base(T s)
        {
            x = s, y = s;
        }

        bool operator==(const Vec2Base& v) const
        {
            return (x == v.x && y == v.y);
        }
        bool operator!=(const Vec2Base& v) const
        {
            return !(operator==(v));
        }

        Vec2Base<T>& operator+=(const Vec2Base& v)
        {
            x += v.x, y += v.y;
            return *this;
        }
        Vec2Base<T>& operator-=(const Vec2Base& v)
        {
            x -= v.x, y -= v.y;
            return *this;
        }
        Vec2Base<T>& operator/=(const Vec2Base& v)
        {
            x /= v.x, y /= v.y;
            return *this;
        }
        Vec2Base<T>& operator*=(const Vec2Base& v)
        {
            x *= v.x, y *= v.y;
            return *this;
        }

        Vec2Base<T>& operator+=(const T& s)
        {
            x += s, y += s;
            return *this;
        }
        Vec2Base<T>& operator-=(const T& s)
        {
            x -= s, y -= s;
            return *this;
        }
        Vec2Base<T>& operator/=(const T& s)
        {
            x /= s, y /= s;
            return *this;
        }
        Vec2Base<T>& operator*=(const T& s)
        {
            x *= s, y *= s;
            return *this;
        }

        Vec2Base<T> operator+(const Vec2Base& v) const
        {
            Vec2Base<T> r(*this);
            return r += v;
        }
        Vec2Base<T> operator-(const Vec2Base& v) const
        {
            Vec2Base<T> r(*this);
            return r -= v;
        }
        Vec2Base<T> operator/(const Vec2Base& v) const
        {
            Vec2Base<T> r(*this);
            return r /= v;
        }
        Vec2Base<T> operator*(const Vec2Base& v) const
        {
            Vec2Base<T> r(*this);
            return r *= v;
        }

        Vec2Base<T> operator+(const T& s) const
        {
            Vec2Base<T> r(*this);
            return r += s;
        }
        Vec2Base<T> operator-(const T& s) const
        {
            Vec2Base<T> r(*this);
            return r -= s;
        }
        Vec2Base<T> operator/(const T& s) const
        {
            Vec2Base<T> r(*this);
            return r /= s;
        }
        Vec2Base<T> operator*(const T& s) const
        {
            Vec2Base<T> r(*this);
            return r *= s;
        }
    };

    template<typename T>
    Vec2Base<T> Normalize(const Vec2Base<T>& v)
    {
        T length = Length(v);
        if (length == 0) return v;
        return (v / length);
    }

    template<typename T>
    Vec2Base<T> Absolute(const Vec2Base<T>& v)
    {
        return Vec2Base<T>(abs(v.x), abs(v.y));
    }

    template<typename T>
    T Length(const Vec2Base<T>& v)
    {
        return CSM_CAST(T, sqrt((v.x*v.y) + (v.y*v.y)));
    }

    template<typename T>
    T Dot(const Vec2Base<T>& a, const Vec2Base<T>& b)
    {
        return ((a.x*b.x) + (a.y*b.y));
    }

    typedef Vec2Base<f32> Vec2;
    typedef Vec2Base<s32> Vec2i;
}

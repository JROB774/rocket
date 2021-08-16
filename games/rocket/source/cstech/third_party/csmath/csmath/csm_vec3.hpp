#pragma once

#include "csm_define.hpp"

CSM_PUBLIC_SCOPE
{
    template<typename T>
    union Vec3Base
    {
        struct { T x, y, z; };
        struct { T r, g, b; };
        struct { T s, t, p; };

        T raw[3];

        Vec3Base()
        {
            // Nothing...
        }
        Vec3Base(T x_, T y_, T z_)
        {
            x = x_, y = y_, z = z_;
        }
        Vec3Base(const Vec3Base& v)
        {
            x = v.x, y = v.y, z = v.z;
        }
        explicit Vec3Base(T s)
        {
            x = s, y = s, z = s;
        }

        bool operator==(const Vec3Base& v) const
        {
            return (x == v.x && y == v.y && z == v.z);
        }
        bool operator!=(const Vec3Base& v) const
        {
            return !(operator==(v));
        }

        Vec3Base<T>& operator+=(const Vec3Base& v)
        {
            x += v.x, y += v.y, z += v.z;
            return *this;
        }
        Vec3Base<T>& operator-=(const Vec3Base& v)
        {
            x -= v.x, y -= v.y, z -= v.z;
            return *this;
        }
        Vec3Base<T>& operator/=(const Vec3Base& v)
        {
            x /= v.x, y /= v.y, z /= v.z;
            return *this;
        }
        Vec3Base<T>& operator*=(const Vec3Base& v)
        {
            x *= v.x, y *= v.y, z *= v.z;
            return *this;
        }

        Vec3Base<T>& operator+=(const T& s)
        {
            x += s, y += s, z += s;
            return *this;
        }
        Vec3Base<T>& operator-=(const T& s)
        {
            x -= s, y -= s, z -= s;
            return *this;
        }
        Vec3Base<T>& operator/=(const T& s)
        {
            x /= s, y /= s, z /= s;
            return *this;
        }
        Vec3Base<T>& operator*=(const T& s)
        {
            x *= s, y *= s, z *= s;
            return *this;
        }

        Vec3Base<T> operator+(const Vec3Base& v) const
        {
            Vec3Base<T> r(*this);
            return r += v;
        }
        Vec3Base<T> operator-(const Vec3Base& v) const
        {
            Vec3Base<T> r(*this);
            return r -= v;
        }
        Vec3Base<T> operator/(const Vec3Base& v) const
        {
            Vec3Base<T> r(*this);
            return r /= v;
        }
        Vec3Base<T> operator*(const Vec3Base& v) const
        {
            Vec3Base<T> r(*this);
            return r *= v;
        }

        Vec3Base<T> operator+(const T& s) const
        {
            Vec3Base<T> r(*this);
            return r += s;
        }
        Vec3Base<T> operator-(const T& s) const
        {
            Vec3Base<T> r(*this);
            return r -= s;
        }
        Vec3Base<T> operator/(const T& s) const
        {
            Vec3Base<T> r(*this);
            return r /= s;
        }
        Vec3Base<T> operator*(const T& s) const
        {
            Vec3Base<T> r(*this);
            return r *= s;
        }
    };

    template<typename T>
    Vec3Base<T> Normalize(const Vec3Base<T>& v)
    {
        T length = Length(v);
        if (length == 0) return v;
        return (v / length);
    }

    template<typename T>
    Vec3Base<T> Absolute(const Vec3Base<T>& v)
    {
        return Vec3Base<T>(abs(v.x), abs(v.y), abs(v.z));
    }

    template<typename T>
    T Length(const Vec3Base<T>& v)
    {
        return CSM_CAST(T, sqrt((v.x*v.y) + (v.y*v.y) + (v.z*v.z)));
    }

    template<typename T>
    T Dot(const Vec3Base<T>& v)
    {
        return ((a.x*b.x) + (a.y*b.y) + (a.z*b.z));
    }

    template<typename T>
    Vec3Base<T> Cross(const Vec3Base<T>& a, const Vec3Base<T>& b)
    {
        Vec3Base<T> r;
        r.x = a.y * b.z - b.y * a.z;
        r.y = a.z * b.x - b.z * a.x;
        r.z = a.x * b.y - b.x * a.y;
        return r;
    }

    typedef Vec3Base<f32> Vec3;
    typedef Vec3Base<s32> Vec3i;
}

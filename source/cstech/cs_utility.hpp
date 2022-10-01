#pragma once

#include "cs_math.hpp"

#include <string>
#include <map>
#include <vector>
#include <algorithm>

#define CS_GET_PTR_TYPE(ptr) cs::RemovePtr<decltype(ptr)>::Type

#if defined(__COUNTER__)
#define CS_DEFER const auto& JOIN(defer, __COUNTER__) = cs::DeferHelp() + [&]()
#else
#define CS_DEFER const auto& JOIN(defer, __LINE__) = cs::DeferHelp() + [&]()
#endif

CS_PUBLIC_SCOPE
{
    CS_API void RandomSeed(s32 seed = -1);
    CS_API s32  RandomS32();
    CS_API s32  RandomS32(s32 min, s32 max);
    CS_API f32  RandomF32();
    CS_API f32  RandomF32(f32 min, f32 max);

    CS_API inline Vec4 RGBAToVec4(u8 r, u8 g, u8 b, u8 a = 255)
    {
        Vec4 color;
        color.r = CS_CAST(f32,r) / 255.0f;
        color.g = CS_CAST(f32,g) / 255.0f;
        color.b = CS_CAST(f32,b) / 255.0f;
        color.a = CS_CAST(f32,a) / 255.0f;
        return color;
    }

    template<typename T>
    CS_API inline bool Contains(std::vector<T>& vec, const T& x)
    {
        return (std::find(vec.begin(), vec.end(), x) != vec.end());
    }
    template<typename K, typename V>
    CS_API inline bool Contains(std::map<K,V>& map, const K& x)
    {
        return (map.find(x) != map.end());
    }

    CS_API inline constexpr u32 FourCC(u8 a, u8 b, u8 c, u8 d)
    {
        return ((CS_CAST(u32,a) << 0) | (CS_CAST(u32,b) << 8) | (CS_CAST(u32,c) << 16) | (CS_CAST(u32,d) << 24));
    }
    CS_API inline constexpr u32 FourCC(const char cc[4])
    {
        return ((CS_CAST(u32,cc[0]) << 0) | (CS_CAST(u32,cc[1]) << 8) | (CS_CAST(u32,cc[2]) << 16) | (CS_CAST(u32,cc[3]) << 24));
    }

    CS_API inline std::string ToSnakeCase(std::string str)
    {
        std::replace(str.begin(), str.end(), ' ', '_');
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }

    // Internal implementation details for CS_DEFER.
    template<typename T>
    struct Defer
    {
        T lambda;
        Defer(T lambda): lambda(lambda) {}
       ~Defer() { lambda(); }
        Defer(const Defer&) = delete;
        Defer& operator=(const Defer&) = delete;
    };
    struct DeferHelp
    {
        template<typename T>
        Defer<T> operator+(T type) { return type; }
    };

    // Internal implementation details for CS_GET_PTR_TYPE.
    template<typename T>
    struct RemovePtr
    {
        typedef T Type;
    };
    template<typename T>
    struct RemovePtr<T*>
    {
        typedef T Type;
    };
    template<typename T>
    struct RemovePtr<T*&>
    {
        typedef T Type;
    };
}

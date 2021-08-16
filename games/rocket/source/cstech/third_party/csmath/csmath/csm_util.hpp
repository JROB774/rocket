#pragma once

#include "csm_define.hpp"

CSM_PUBLIC_SCOPE
{
    //
    // Constants
    //

    static constexpr f32 k_pi32 = 3.141592653590f;
    static constexpr f32 k_tau32 = 6.283185307180f;
    static constexpr f64 k_pi64 = 3.14159265358979323846;
    static constexpr f64 k_tau64 = 6.28318530717958647692;

    //
    // Functions
    //

    template<typename T>
    CSM_API CSM_INLINE constexpr T Min(const T& a, const T& b)
    {
        return (a < b) ? a : b;
    }
    template<typename T>
    CSM_API CSM_INLINE constexpr T Max(const T& a, const T& b)
    {
        return (a < b) ? b : a;
    }

    template<typename T>
    CSM_API CSM_INLINE constexpr T Clamp(const T& x, const T& min, const T& max)
    {
        return (x < min ? min : max < x ? max : x);
    }

    template<typename T>
    CSM_API CSM_INLINE T Lerp(const T& a, const T& b, const T& t)
    {
        return (a + t * (b - a));
    }

    template<typename T>
    CSM_API CSM_INLINE constexpr T MapRange(const T& x, const T& aMin, const T& aMax, const T& bMin, const T& bMax)
    {
        return bMin + (bMax - bMin) * ((x - aMin) / (aMax - aMin));
    }

    template<typename T>
    CSM_API CSM_INLINE constexpr T ToRad(const T& deg)
    {
        return deg * CSM_CAST(T, k_pi64) / CSM_CAST(T, 180);
    }
    template<typename T>
    CSM_API CSM_INLINE constexpr T ToDeg(const T& rad)
    {
        return rad * CSM_CAST(T, 180) / CSM_CAST(T, k_pi64);
    }
}

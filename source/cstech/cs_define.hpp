#pragma once

#include <stdint.h>

#define CS_PUBLIC_SCOPE namespace cs
#define CS_PRIVATE_SCOPE namespace

#define CS_API

#define CS_STRINGIFY( x) CS_STRINGIFY1(x)
#define CS_STRINGIFY1(x) #x

#define CS_JOIN( a, b) CS_JOIN2(a, b)
#define CS_JOIN2(a, b) CS_JOIN1(a, b)
#define CS_JOIN1(a, b) a##b

#define CS_INLINE inline

#define CS_CCAST(t,x) const_cast<t>(x)
#define CS_DCAST(t,x) dynamic_cast<t>(x)
#define CS_RCAST(t,x) reinterpret_cast<t>(x)
#define CS_SCAST(t,x) static_cast<t>(x)

#define CS_CAST(t,x) ((t)(x))

#if CS_DEBUG
#include <assert.h>
#define CS_ASSERT(e,msg) assert(e)
#else
#define CS_ASSERT(e,msg) (void)(0)
#endif

#define CS_KILOBYTES(kb) ((kb)*1024)
#define CS_MEGABYTES(mb) ((mb)*1024*1024)
#define CS_GIGABYTES(gb) ((gb)*1024*1024*1024)
#define CS_TERABYTES(tb) ((tb)*1024*1024*1024*1024)

#define CS_ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

#define CS_NO_COPY(classname)                     \
classname (const classname&) = delete;            \
classname& operator= (const classname&) = delete;
#define CS_NO_MOVE(classname)                     \
classname (classname&&) = delete;                 \
classname& operator= (classname&&) = delete;

#define CS_DECLARE_PRIVATE_STRUCT(name) struct name##__Type; typedef name##__Type* name;
#define CS_DEFINE_PRIVATE_STRUCT(name) struct name##__Type

#define CS_CHECK_FLAGS(val, flags) (((val) & (flags)) != CS_CAST(decltype(val),0))
#define CS_SET_FLAGS(val, flags) ((val) |= (flags))
#define CS_UNSET_FLAGS(val, flags) ((val) &= ~(flags))

CS_PUBLIC_SCOPE
{
    typedef  uint8_t  u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;
    typedef   int8_t  s8;
    typedef  int16_t s16;
    typedef  int32_t s32;
    typedef  int64_t s64;
    typedef    float f32;
    typedef   double f64;

    struct Rect
    {
        f32 x, y, w, h;
    };
}

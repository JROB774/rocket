#pragma once

// @Incomplete: Clear out but keep around for now!!!
#define CS_PUBLIC_SCOPE namespace cs
#define CS_PRIVATE_SCOPE namespace

#define CS_API

#define CS_CAST(t,x) ((t)(x))

#define CS_ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

#define JOIN( a, b) JOIN2(a, b)
#define JOIN2(a, b) JOIN1(a, b)
#define JOIN1(a, b) a##b

// @Incomplete: Sort this out a bit...
#define ASSERT(e,msg)

#define DECLARE_PRIVATE_STRUCT(name) struct name##__Type; typedef name##__Type* name;
#define DEFINE_PRIVATE_STRUCT(name) struct name##__Type

#define CHECK_FLAGS(val, flags) (((val) & (flags)) != CS_CAST(decltype(val),0))
#define SET_FLAGS(val, flags) ((val) |= (flags))
#define UNSET_FLAGS(val, flags) ((val) &= ~(flags))

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

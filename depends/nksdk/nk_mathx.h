#ifndef NK_MATHX_H__ /*///////////////////////////////////////////////////////*/
#define NK_MATHX_H__

#include "nk_define.h"

#include <math.h>

#if !defined(NK_HAS_CPP)
#error nk_mathx requires C++ in order to be used
#endif

static NKCONSTEXPR nkF32 NK_PI  = 3.141592653590f;
static NKCONSTEXPR nkF32 NK_TAU = 6.283185307180f;

// nkVec2 ======================================================================
union nkVec2
{
    struct { nkF32 x, y; };
    struct { nkF32 r, g; };
    struct { nkF32 s, t; };

    nkF32 raw[2];

    nkVec2& operator+=(const nkVec2& rhs) { x += rhs.x, y += rhs.y; return *this; }
    nkVec2& operator-=(const nkVec2& rhs) { x -= rhs.x, y -= rhs.y; return *this; }
    nkVec2& operator/=(const nkVec2& rhs) { x /= rhs.x, y /= rhs.y; return *this; }
    nkVec2& operator*=(const nkVec2& rhs) { x *= rhs.x, y *= rhs.y; return *this; }
    nkVec2& operator+=(const nkF32&  rhs) { x += rhs,   y += rhs;   return *this; }
    nkVec2& operator-=(const nkF32&  rhs) { x -= rhs,   y -= rhs;   return *this; }
    nkVec2& operator/=(const nkF32&  rhs) { x /= rhs,   y /= rhs;   return *this; }
    nkVec2& operator*=(const nkF32&  rhs) { x *= rhs,   y *= rhs;   return *this; }

    const nkF32& operator[](size_t idx) const { return raw[idx]; }
          nkF32& operator[](size_t idx)       { return raw[idx]; }
};

NKAPI NKFORCEINLINE nkVec2 operator+(nkVec2 a, const nkVec2& b) { a += b; return a; }
NKAPI NKFORCEINLINE nkVec2 operator-(nkVec2 a, const nkVec2& b) { a -= b; return a; }
NKAPI NKFORCEINLINE nkVec2 operator/(nkVec2 a, const nkVec2& b) { a /= b; return a; }
NKAPI NKFORCEINLINE nkVec2 operator*(nkVec2 a, const nkVec2& b) { a *= b; return a; }
NKAPI NKFORCEINLINE nkVec2 operator+(nkVec2 a, const nkF32&  b) { a += b; return a; }
NKAPI NKFORCEINLINE nkVec2 operator-(nkVec2 a, const nkF32&  b) { a -= b; return a; }
NKAPI NKFORCEINLINE nkVec2 operator/(nkVec2 a, const nkF32&  b) { a /= b; return a; }
NKAPI NKFORCEINLINE nkVec2 operator*(nkVec2 a, const nkF32&  b) { a *= b; return a; }
NKAPI NKFORCEINLINE nkVec2 operator+(nkF32  a, const nkVec2& b) { nkVec2 v = { a,a }; v += b; return v; }
NKAPI NKFORCEINLINE nkVec2 operator-(nkF32  a, const nkVec2& b) { nkVec2 v = { a,a }; v -= b; return v; }
NKAPI NKFORCEINLINE nkVec2 operator/(nkF32  a, const nkVec2& b) { nkVec2 v = { a,a }; v /= b; return v; }
NKAPI NKFORCEINLINE nkVec2 operator*(nkF32  a, const nkVec2& b) { nkVec2 v = { a,a }; v *= b; return v; }

NKAPI NKFORCEINLINE nkBool operator==(const nkVec2& a, const nkVec2& b)
{
    return (a.x == b.x && a.y == b.y);
}
NKAPI NKFORCEINLINE nkBool operator!=(const nkVec2& a, const nkVec2& b)
{
    return !(a == b);
}

static NKCONSTEXPR nkVec2 NK_V2_ZERO   = {  0, 0 };
static NKCONSTEXPR nkVec2 NK_V2_ONE    = {  1, 1 };
static NKCONSTEXPR nkVec2 NK_V2_UNIT_X = {  1, 0 };
static NKCONSTEXPR nkVec2 NK_V2_UNIT_Y = {  0, 1 };
static NKCONSTEXPR nkVec2 NK_V2_POS_X  = {  1, 0 };
static NKCONSTEXPR nkVec2 NK_V2_POS_Y  = {  0, 1 };
static NKCONSTEXPR nkVec2 NK_V2_NEG_X  = { -1, 0 };
static NKCONSTEXPR nkVec2 NK_V2_NEG_Y  = {  0,-1 };
// =============================================================================

// nkVec3 ======================================================================
union nkVec3
{
    struct { nkF32 x, y, z; };
    struct { nkF32 r, g, b; };
    struct { nkF32 s, t, p; };

    nkF32 raw[3];

    nkVec3& operator+=(const nkVec3& rhs) { x += rhs.x, y += rhs.y, z += rhs.z; return *this; }
    nkVec3& operator-=(const nkVec3& rhs) { x -= rhs.x, y -= rhs.y, z -= rhs.z; return *this; }
    nkVec3& operator/=(const nkVec3& rhs) { x /= rhs.x, y /= rhs.y, z /= rhs.z; return *this; }
    nkVec3& operator*=(const nkVec3& rhs) { x *= rhs.x, y *= rhs.y, z *= rhs.z; return *this; }
    nkVec3& operator+=(const nkF32&  rhs) { x += rhs,   y += rhs,   z += rhs;   return *this; }
    nkVec3& operator-=(const nkF32&  rhs) { x -= rhs,   y -= rhs,   z -= rhs;   return *this; }
    nkVec3& operator/=(const nkF32&  rhs) { x /= rhs,   y /= rhs,   z /= rhs;   return *this; }
    nkVec3& operator*=(const nkF32&  rhs) { x *= rhs,   y *= rhs,   z *= rhs;   return *this; }

    const nkF32& operator[](size_t idx) const { return raw[idx]; }
          nkF32& operator[](size_t idx)       { return raw[idx]; }
};

NKAPI NKFORCEINLINE nkVec3 operator+(nkVec3 a, const nkVec3& b) { a += b; return a; }
NKAPI NKFORCEINLINE nkVec3 operator-(nkVec3 a, const nkVec3& b) { a -= b; return a; }
NKAPI NKFORCEINLINE nkVec3 operator/(nkVec3 a, const nkVec3& b) { a /= b; return a; }
NKAPI NKFORCEINLINE nkVec3 operator*(nkVec3 a, const nkVec3& b) { a *= b; return a; }
NKAPI NKFORCEINLINE nkVec3 operator+(nkVec3 a, const nkF32&  b) { a += b; return a; }
NKAPI NKFORCEINLINE nkVec3 operator-(nkVec3 a, const nkF32&  b) { a -= b; return a; }
NKAPI NKFORCEINLINE nkVec3 operator/(nkVec3 a, const nkF32&  b) { a /= b; return a; }
NKAPI NKFORCEINLINE nkVec3 operator*(nkVec3 a, const nkF32&  b) { a *= b; return a; }
NKAPI NKFORCEINLINE nkVec3 operator+(nkF32  a, const nkVec3& b) { nkVec3 v = { a,a,a }; v += b; return v; }
NKAPI NKFORCEINLINE nkVec3 operator-(nkF32  a, const nkVec3& b) { nkVec3 v = { a,a,a }; v -= b; return v; }
NKAPI NKFORCEINLINE nkVec3 operator/(nkF32  a, const nkVec3& b) { nkVec3 v = { a,a,a }; v /= b; return v; }
NKAPI NKFORCEINLINE nkVec3 operator*(nkF32  a, const nkVec3& b) { nkVec3 v = { a,a,a }; v *= b; return v; }

NKAPI NKFORCEINLINE nkBool operator==(const nkVec3& a, const nkVec3& b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}
NKAPI NKFORCEINLINE nkBool operator!=(const nkVec3& a, const nkVec3& b)
{
    return !(a == b);
}

static NKCONSTEXPR nkVec3 NK_V3_ZERO    = {  0, 0, 0 };
static NKCONSTEXPR nkVec3 NK_V3_ONE     = {  1, 1, 1 };
static NKCONSTEXPR nkVec3 NK_V3_UNIT_X  = {  1, 0, 0 };
static NKCONSTEXPR nkVec3 NK_V3_UNIT_Y  = {  0, 1, 0 };
static NKCONSTEXPR nkVec3 NK_V3_UNIT_Z  = {  0, 0, 1 };
static NKCONSTEXPR nkVec3 NK_V3_POS_X   = {  1, 0, 0 };
static NKCONSTEXPR nkVec3 NK_V3_POS_Y   = {  0, 1, 0 };
static NKCONSTEXPR nkVec3 NK_V3_POS_Z   = {  0, 0, 1 };
static NKCONSTEXPR nkVec3 NK_V3_NEG_X   = { -1, 0, 0 };
static NKCONSTEXPR nkVec3 NK_V3_NEG_Y   = {  0,-1, 0 };
static NKCONSTEXPR nkVec3 NK_V3_NEG_Z   = {  0, 0,-1 };
static NKCONSTEXPR nkVec3 NK_V3_BLACK   = {  0, 0, 0 };
static NKCONSTEXPR nkVec3 NK_V3_WHITE   = {  1, 1, 1 };
static NKCONSTEXPR nkVec3 NK_V3_RED     = {  1, 0, 0 };
static NKCONSTEXPR nkVec3 NK_V3_GREEN   = {  0, 1, 0 };
static NKCONSTEXPR nkVec3 NK_V3_BLUE    = {  0, 0, 1 };
static NKCONSTEXPR nkVec3 NK_V3_CYAN    = {  0, 1, 1 };
static NKCONSTEXPR nkVec3 NK_V3_MAGENTA = {  1, 0, 1 };
static NKCONSTEXPR nkVec3 NK_V3_YELLOW  = {  1, 1, 0 };
// =============================================================================

// nkVec4 ======================================================================
union nkVec4
{
    struct { nkF32 x, y, z, w; };
    struct { nkF32 r, g, b, a; };
    struct { nkF32 s, t, p, q; };

    nkF32 raw[4];

    nkVec4& operator+=(const nkVec4& rhs) { x += rhs.x, y += rhs.y, z += rhs.z, w += rhs.w; return *this; }
    nkVec4& operator-=(const nkVec4& rhs) { x -= rhs.x, y -= rhs.y, z -= rhs.z, w -= rhs.w; return *this; }
    nkVec4& operator/=(const nkVec4& rhs) { x /= rhs.x, y /= rhs.y, z /= rhs.z, w /= rhs.w; return *this; }
    nkVec4& operator*=(const nkVec4& rhs) { x *= rhs.x, y *= rhs.y, z *= rhs.z, w *= rhs.w; return *this; }
    nkVec4& operator+=(const nkF32&  rhs) { x += rhs,   y += rhs,   z += rhs,   w += rhs;   return *this; }
    nkVec4& operator-=(const nkF32&  rhs) { x -= rhs,   y -= rhs,   z -= rhs,   w -= rhs;   return *this; }
    nkVec4& operator/=(const nkF32&  rhs) { x /= rhs,   y /= rhs,   z /= rhs,   w /= rhs;   return *this; }
    nkVec4& operator*=(const nkF32&  rhs) { x *= rhs,   y *= rhs,   z *= rhs,   w *= rhs;   return *this; }

    const nkF32& operator[](size_t idx) const { return raw[idx]; }
          nkF32& operator[](size_t idx)       { return raw[idx]; }
};

NKAPI NKFORCEINLINE nkVec4 operator+(nkVec4 a, const nkVec4& b) { a += b; return a; }
NKAPI NKFORCEINLINE nkVec4 operator-(nkVec4 a, const nkVec4& b) { a -= b; return a; }
NKAPI NKFORCEINLINE nkVec4 operator/(nkVec4 a, const nkVec4& b) { a /= b; return a; }
NKAPI NKFORCEINLINE nkVec4 operator*(nkVec4 a, const nkVec4& b) { a *= b; return a; }
NKAPI NKFORCEINLINE nkVec4 operator+(nkVec4 a, const nkF32&  b) { a += b; return a; }
NKAPI NKFORCEINLINE nkVec4 operator-(nkVec4 a, const nkF32&  b) { a -= b; return a; }
NKAPI NKFORCEINLINE nkVec4 operator/(nkVec4 a, const nkF32&  b) { a /= b; return a; }
NKAPI NKFORCEINLINE nkVec4 operator*(nkVec4 a, const nkF32&  b) { a *= b; return a; }
NKAPI NKFORCEINLINE nkVec4 operator+(nkF32  a, const nkVec4& b) { nkVec4 v = { a,a,a,a }; v += b; return v; }
NKAPI NKFORCEINLINE nkVec4 operator-(nkF32  a, const nkVec4& b) { nkVec4 v = { a,a,a,a }; v -= b; return v; }
NKAPI NKFORCEINLINE nkVec4 operator/(nkF32  a, const nkVec4& b) { nkVec4 v = { a,a,a,a }; v /= b; return v; }
NKAPI NKFORCEINLINE nkVec4 operator*(nkF32  a, const nkVec4& b) { nkVec4 v = { a,a,a,a }; v *= b; return v; }

NKAPI NKFORCEINLINE nkBool operator==(const nkVec4& a, const nkVec4& b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w);
}
NKAPI NKFORCEINLINE nkBool operator!=(const nkVec4& a, const nkVec4& b)
{
    return !(a == b);
}

static NKCONSTEXPR nkVec4 NK_V4_ZERO    = {  0, 0, 0, 0 };
static NKCONSTEXPR nkVec4 NK_V4_ONE     = {  1, 1, 1, 1 };
static NKCONSTEXPR nkVec4 NK_V4_UNIT_X  = {  1, 0, 0, 0 };
static NKCONSTEXPR nkVec4 NK_V4_UNIT_Y  = {  0, 1, 0, 0 };
static NKCONSTEXPR nkVec4 NK_V4_UNIT_Z  = {  0, 0, 1, 0 };
static NKCONSTEXPR nkVec4 NK_V4_UNIT_W  = {  0, 0, 0, 1 };
static NKCONSTEXPR nkVec4 NK_V4_POS_X   = {  1, 0, 0, 0 };
static NKCONSTEXPR nkVec4 NK_V4_POS_Y   = {  0, 1, 0, 0 };
static NKCONSTEXPR nkVec4 NK_V4_POS_Z   = {  0, 0, 1, 0 };
static NKCONSTEXPR nkVec4 NK_V4_POS_W   = {  0, 0, 0, 1 };
static NKCONSTEXPR nkVec4 NK_V4_NEG_X   = { -1, 0, 0, 0 };
static NKCONSTEXPR nkVec4 NK_V4_NEG_Y   = {  0,-1, 0, 0 };
static NKCONSTEXPR nkVec4 NK_V4_NEG_Z   = {  0, 0,-1, 0 };
static NKCONSTEXPR nkVec4 NK_V4_NEG_W   = {  0, 0, 0,-1 };
static NKCONSTEXPR nkVec4 NK_V4_BLACK   = {  0, 0, 0, 1 };
static NKCONSTEXPR nkVec4 NK_V4_WHITE   = {  1, 1, 1, 1 };
static NKCONSTEXPR nkVec4 NK_V4_RED     = {  1, 0, 0, 1 };
static NKCONSTEXPR nkVec4 NK_V4_GREEN   = {  0, 1, 0, 1 };
static NKCONSTEXPR nkVec4 NK_V4_BLUE    = {  0, 0, 1, 1 };
static NKCONSTEXPR nkVec4 NK_V4_CYAN    = {  0, 1, 1, 1 };
static NKCONSTEXPR nkVec4 NK_V4_MAGENTA = {  1, 0, 1, 1 };
static NKCONSTEXPR nkVec4 NK_V4_YELLOW  = {  1, 1, 0, 1 };
// =============================================================================

// nkMat2 ======================================================================
union nkMat2
{
    struct
    {
        nkF32 x00,x01;
        nkF32 x10,x11;
    };

    nkF32 x[2][2];
    nkF32 raw[2*2];

    nkMat2& operator+=(const nkMat2& rhs) { for(nkS32 c=0; c<2; ++c) for(nkS32 r=0; r<2; ++r) x[c][r] += rhs.x[c][r]; return *this; }
    nkMat2& operator-=(const nkMat2& rhs) { for(nkS32 c=0; c<2; ++c) for(nkS32 r=0; r<2; ++r) x[c][r] -= rhs.x[c][r]; return *this; }
    nkMat2& operator/=(const nkMat2& rhs) { for(nkS32 c=0; c<2; ++c) for(nkS32 r=0; r<2; ++r) x[c][r] /= rhs.x[c][r]; return *this; }

    nkMat2& operator*=(const nkMat2& rhs)
    {
        nkMat2 m = NK_ZERO_MEM;
        for(nkS32 r=0; r<2; ++r)
            for(nkS32 c1=0; c1<2; ++c1)
                for(nkS32 c0=0; c0<2; ++c0)
                    m.x[c1][r] += x[c0][r] * rhs.x[c1][c0];
        *this = m;
        return *this;
    }
};

NKAPI NKFORCEINLINE nkMat2 nk_m2_identity()
{
    nkMat2 m = NK_ZERO_MEM;
    m.x00 = 1;
    m.x11 = 1;
    return m;
}

NKAPI NKFORCEINLINE nkMat2 operator+(nkMat2 a, const nkMat2& b) { a += b; return a; }
NKAPI NKFORCEINLINE nkMat2 operator-(nkMat2 a, const nkMat2& b) { a -= b; return a; }
NKAPI NKFORCEINLINE nkMat2 operator/(nkMat2 a, const nkMat2& b) { a /= b; return a; }
NKAPI NKFORCEINLINE nkMat2 operator*(nkMat2 a, const nkMat2& b) { a *= b; return a; }

NKAPI NKFORCEINLINE nkVec2 operator*(nkMat2 a, const nkVec2& b)
{
    nkVec2 v = NK_ZERO_MEM;
    for(nkS32 r=0; r<2; ++r)
        for(nkS32 c=0; c<2; ++c)
            v.raw[r] += a.x[c][r] * b.raw[c];
    return v;
}

NKAPI NKFORCEINLINE nkBool operator==(const nkMat2& a, const nkMat2& b)
{
    for(nkS32 r=0; r<2; ++r)
        for(nkS32 c=0; c<2; ++c)
            if(a.x[c][r] != b.x[c][r])
                return NK_FALSE;
    return NK_TRUE;
}
NKAPI NKFORCEINLINE nkBool operator!=(const nkMat2& a, const nkMat2& b)
{
    return !(a == b);
}
// =============================================================================

// nkMat3 ======================================================================
union nkMat3
{
    struct
    {
        nkF32 x00,x01,x02;
        nkF32 x10,x11,x12;
        nkF32 x20,x21,x22;
    };

    nkF32 x[3][3];
    nkF32 raw[3*3];

    nkMat3& operator+=(const nkMat3& rhs) { for(nkS32 c=0; c<3; ++c) for(nkS32 r=0; r<3; ++r) x[c][r] += rhs.x[c][r]; return *this; }
    nkMat3& operator-=(const nkMat3& rhs) { for(nkS32 c=0; c<3; ++c) for(nkS32 r=0; r<3; ++r) x[c][r] -= rhs.x[c][r]; return *this; }
    nkMat3& operator/=(const nkMat3& rhs) { for(nkS32 c=0; c<3; ++c) for(nkS32 r=0; r<3; ++r) x[c][r] /= rhs.x[c][r]; return *this; }

    nkMat3& operator*=(const nkMat3& rhs)
    {
        nkMat3 m = NK_ZERO_MEM;
        for(nkS32 r=0; r<3; ++r)
            for(nkS32 c1=0; c1<3; ++c1)
                for(nkS32 c0=0; c0<3; ++c0)
                    m.x[c1][r] += x[c0][r] * rhs.x[c1][c0];
        *this = m;
        return *this;
    }
};

NKAPI NKFORCEINLINE nkMat3 nk_m3_identity()
{
    nkMat3 m = NK_ZERO_MEM;
    m.x00 = 1;
    m.x11 = 1;
    m.x22 = 1;
    return m;
}

NKAPI NKFORCEINLINE nkMat3 operator+(nkMat3 a, const nkMat3& b) { a += b; return a; }
NKAPI NKFORCEINLINE nkMat3 operator-(nkMat3 a, const nkMat3& b) { a -= b; return a; }
NKAPI NKFORCEINLINE nkMat3 operator/(nkMat3 a, const nkMat3& b) { a /= b; return a; }
NKAPI NKFORCEINLINE nkMat3 operator*(nkMat3 a, const nkMat3& b) { a *= b; return a; }

NKAPI NKFORCEINLINE nkVec3 operator*(nkMat3 a, const nkVec3& b)
{
    nkVec3 v = NK_ZERO_MEM;
    for(nkS32 r=0; r<3; ++r)
        for(nkS32 c=0; c<3; ++c)
            v.raw[r] += a.x[c][r] * b.raw[c];
    return v;
}

NKAPI NKFORCEINLINE nkBool operator==(const nkMat3& a, const nkMat3& b)
{
    for(nkS32 r=0; r<3; ++r)
        for(nkS32 c=0; c<3; ++c)
            if(a.x[c][r] != b.x[c][r])
                return NK_FALSE;
    return NK_TRUE;
}
NKAPI NKFORCEINLINE nkBool operator!=(const nkMat3& a, const nkMat3& b)
{
    return !(a == b);
}
// =============================================================================

// nkMat4 ======================================================================
union nkMat4
{
    struct
    {
        nkF32 x00,x01,x02,x03;
        nkF32 x10,x11,x12,x13;
        nkF32 x20,x21,x22,x23;
        nkF32 x30,x31,x32,x33;
    };

    nkF32 x[4][4];
    nkF32 raw[4*4];

    nkMat4& operator+=(const nkMat4& rhs) { for(nkS32 c=0; c<4; ++c) for(nkS32 r=0; r<4; ++r) x[c][r] += rhs.x[c][r]; return *this; }
    nkMat4& operator-=(const nkMat4& rhs) { for(nkS32 c=0; c<4; ++c) for(nkS32 r=0; r<4; ++r) x[c][r] -= rhs.x[c][r]; return *this; }
    nkMat4& operator/=(const nkMat4& rhs) { for(nkS32 c=0; c<4; ++c) for(nkS32 r=0; r<4; ++r) x[c][r] /= rhs.x[c][r]; return *this; }

    nkMat4& operator*=(const nkMat4& rhs)
    {
        nkMat4 m = NK_ZERO_MEM;
        for(nkS32 r=0; r<4; ++r)
            for(nkS32 c1=0; c1<4; ++c1)
                for(nkS32 c0=0; c0<4; ++c0)
                    m.x[c1][r] += x[c0][r] * rhs.x[c1][c0];
        *this = m;
        return *this;
    }
};

NKAPI NKFORCEINLINE nkMat4 nk_m4_identity()
{
    nkMat4 m = NK_ZERO_MEM;
    m.x00 = 1;
    m.x11 = 1;
    m.x22 = 1;
    m.x33 = 1;
    return m;
}

NKAPI NKFORCEINLINE nkMat4 operator+(nkMat4 a, const nkMat4& b) { a += b; return a; }
NKAPI NKFORCEINLINE nkMat4 operator-(nkMat4 a, const nkMat4& b) { a -= b; return a; }
NKAPI NKFORCEINLINE nkMat4 operator/(nkMat4 a, const nkMat4& b) { a /= b; return a; }
NKAPI NKFORCEINLINE nkMat4 operator*(nkMat4 a, const nkMat4& b) { a *= b; return a; }

NKAPI NKFORCEINLINE nkVec4 operator*(nkMat4 a, const nkVec4& b)
{
    nkVec4 v = NK_ZERO_MEM;
    for(nkS32 r=0; r<4; ++r)
        for(nkS32 c=0; c<4; ++c)
            v.raw[r] += a.x[c][r] * b.raw[c];
    return v;
}

NKAPI NKFORCEINLINE nkBool operator==(const nkMat4& a, const nkMat4& b)
{
    for(nkS32 r=0; r<4; ++r)
        for(nkS32 c=0; c<4; ++c)
            if(a.x[c][r] != b.x[c][r])
                return NK_FALSE;
    return NK_TRUE;
}
NKAPI NKFORCEINLINE nkBool operator!=(const nkMat4& a, const nkMat4& b)
{
    return !(a == b);
}
// =============================================================================

NKAPI NKFORCEINLINE nkF32 nk_torad(nkF32 deg)
{
    return (deg * NK_PI / 180.0f);
}
NKAPI NKFORCEINLINE nkF32 nk_todeg(nkF32 rad)
{
    return (rad * 180.0f / NK_PI);
}

NKAPI NKFORCEINLINE nkF32 nk_clamp(nkF32 x, nkF32 l, nkF32 u)
{
    return ((x < l) ? l : ((u < x) ? u : x));
}

NKAPI NKFORCEINLINE nkF32 nk_min(nkF32 a, nkF32 b)
{
    return ((a < b) ? a : b);
}
NKAPI NKFORCEINLINE nkF32 nk_max(nkF32 a, nkF32 b)
{
    return ((a < b) ? b : a);
}

NKAPI NKFORCEINLINE nkF32 nk_map_range(nkF32 x, nkF32 amin, nkF32 amax, nkF32 bmin, nkF32 bmax)
{
    return (bmin + (bmax - bmin) * ((x - amin) / (amax - amin)));
}

NKAPI NKFORCEINLINE nkF32 nk_sin_range(nkF32 min, nkF32 max, nkF32 t)
{
    nkF32 half_range = (max - min) / 2.0f;
    return (min + half_range + sinf(t) * half_range);
}

NKAPI NKFORCEINLINE nkF32 nk_lerp(nkF32 a, nkF32 b, nkF32 t)
{
    return (a + t * (b - a));
}
NKAPI NKFORCEINLINE nkVec2 nk_lerp(const nkVec2& a, const nkVec2& b, nkF32 t)
{
    return (a + t * (b - a));
}
NKAPI NKFORCEINLINE nkVec3 nk_lerp(const nkVec3& a, const nkVec3& b, nkF32 t)
{
    return (a + t * (b - a));
}
NKAPI NKFORCEINLINE nkVec4 nk_lerp(const nkVec4& a, const nkVec4& b, nkF32 t)
{
    return (a + t * (b - a));
}

NKAPI NKFORCEINLINE nkF32 nk_length(const nkVec2& v)
{
    return sqrtf((v.x*v.x) + (v.y*v.y));
}
NKAPI NKFORCEINLINE nkF32 nk_length(const nkVec3& v)
{
    return sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}
NKAPI NKFORCEINLINE nkF32 nk_length(const nkVec4& v)
{
    return sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
}

NKAPI NKFORCEINLINE nkF32 nk_length2(const nkVec2& v)
{
    return ((v.x*v.y) + (v.y*v.y));
}
NKAPI NKFORCEINLINE nkF32 nk_length2(const nkVec3& v)
{
    return ((v.x*v.y) + (v.y*v.y) + (v.z*v.z));
}
NKAPI NKFORCEINLINE nkF32 nk_length2(const nkVec4& v)
{
    return ((v.x*v.y) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
}

NKAPI NKFORCEINLINE nkVec2 nk_normalize(const nkVec2& v)
{
    nkF32 len = nk_length(v);
    return ((len) ? v / len : v);
}
NKAPI NKFORCEINLINE nkVec3 nk_normalize(const nkVec3& v)
{
    nkF32 len = nk_length(v);
    return ((len) ? v / len : v);
}
NKAPI NKFORCEINLINE nkVec4 nk_normalize(const nkVec4& v)
{
    nkF32 len = nk_length(v);
    return ((len) ? v / len : v);
}

NKAPI NKFORCEINLINE nkF32 nk_dot(const nkVec2& a, const nkVec2& b)
{
    return ((a.x*b.x) + (a.y*b.y));
}
NKAPI NKFORCEINLINE nkF32 nk_dot(const nkVec3& a, const nkVec3& b)
{
    return ((a.x*b.x) + (a.y*b.y) + (a.z*b.z));
}
NKAPI NKFORCEINLINE nkF32 nk_dot(const nkVec4& a, const nkVec4& b)
{
    return ((a.x*b.x) + (a.y*b.y) + (a.z*b.z) + (a.w*b.w));
}

NKAPI NKFORCEINLINE nkVec2 nk_rotate(const nkVec2& v, nkF32 angle)
{
    nkVec2 r;
    r.x = v.x * cosf(angle) - v.y * sinf(angle);
    r.y = v.x * sinf(angle) + v.y * cosf(angle);
    return r;
}

NKAPI NKFORCEINLINE nkVec3 nk_cross(const nkVec3& a, const nkVec3& b)
{
    nkVec3 v;
    v.x = (a.y*b.z) - (a.z*b.y);
    v.y = (a.z*b.x) - (a.x*b.z);
    v.z = (a.x*b.y) - (a.y*b.x);
    return v;
}

// Matrix inversion implemented using the Gauss Jordan Method.
static void nk__matrix_inverse(nkF32* m, nkF32* r, nkS32 n)
{
    NK_ASSERT(r);
    NK_ASSERT(m);

    for(nkS32 i=0; i<n; ++i)
    {
        nkF32 t = m[i*n+i];
        for(nkS32 j=0; j<n; ++j)
        {
            m[j*n+i] /= t;
            r[j*n+i] /= t;
        }
        for(nkS32 j=0; j<n; ++j)
        {
            t = m[i*n+j];
            for(nkS32 k=0; k<n; ++k)
            {
                if(j == i)
                    break;
                m[k*n+j] -= m[k*n+i] * t;
                r[k*n+j] -= r[k*n+i] * t;
            }
        }
    }
}

NKAPI NKFORCEINLINE nkMat2 nk_inverse(nkMat2 m)
{
    nkMat2 r = nk_m2_identity();
    nk__matrix_inverse(m.raw, r.raw, 2);
    return r;
}
NKAPI NKFORCEINLINE nkMat3 nk_inverse(nkMat3 m)
{
    nkMat3 r = nk_m3_identity();
    nk__matrix_inverse(m.raw, r.raw, 3);
    return r;
}
NKAPI NKFORCEINLINE nkMat4 nk_inverse(nkMat4 m)
{
    nkMat4 r = nk_m4_identity();
    nk__matrix_inverse(m.raw, r.raw, 4);
    return r;
}

NKAPI NKFORCEINLINE nkMat4 nk_orthographic(nkF32 l, nkF32 r, nkF32 b, nkF32 t, nkF32 n, nkF32 f)
{
    nkF32 inv_right_left =  1.0f / (r - l);
    nkF32 inv_top_bottom =  1.0f / (t - b);
    nkF32 inv_far_near   = -1.0f / (f - n);

    nkMat4 res = NK_ZERO_MEM;
    res.x00 =  (2.0f * inv_right_left);
    res.x11 =  (2.0f * inv_top_bottom);
    res.x22 =  (2.0f * inv_far_near);
    res.x30 = -(r + l) * inv_right_left;
    res.x31 = -(t + b) * inv_top_bottom;
    res.x32 =  (f + n) * inv_far_near;
    res.x33 =  (1.0f);
    return res;
}

NKAPI NKFORCEINLINE nkMat4 nk_perspective(nkF32 fov, nkF32 aspect, nkF32 znear, nkF32 zfar)
{
    nkF32 inv_tan_half_fovy = 1.0f / tanf(fov * 0.5f);
    nkF32 inv_far_near = 1.0f / (znear - zfar);

    nkMat4 res = NK_ZERO_MEM;
    res.x00 =  (inv_tan_half_fovy / aspect);
    res.x11 =  (inv_tan_half_fovy);
    res.x22 =  (znear + zfar) * inv_far_near;
    res.x23 = -(1.0f);
    res.x32 =  (2.0f * znear * zfar * inv_far_near);
    return res;
}

NKAPI NKFORCEINLINE nkMat4 nk_lookat(const nkVec3& eye, const nkVec3& center, const nkVec3& up)
{
    nkVec3 f = nk_normalize(center - eye);
    nkVec3 s = nk_normalize(nk_cross(f, up));
    nkVec3 u = nk_cross(s, f);

    nkMat4 res;
    res.x00 =  s.x;
    res.x01 =  u.x;
    res.x02 = -f.x;
    res.x10 =  s.y;
    res.x11 =  u.y;
    res.x12 = -f.y;
    res.x20 =  s.z;
    res.x21 =  u.z;
    res.x22 = -f.z;
    res.x30 = -nk_dot(s, eye);
    res.x31 = -nk_dot(u, eye);
    res.x32 =  nk_dot(f, eye);
    res.x03 =  0.0f;
    res.x13 =  0.0f;
    res.x23 =  0.0f;
    res.x33 =  1.0f;
    return res;
}

NKAPI NKFORCEINLINE nkMat4 nk_translate(const nkMat4& m, const nkVec3& pos)
{
    nkF32 x30 = m.x00 * pos.x + m.x10 * pos.y + m.x20 * pos.z + m.x30;
    nkF32 x31 = m.x01 * pos.x + m.x11 * pos.y + m.x21 * pos.z + m.x31;
    nkF32 x32 = m.x02 * pos.x + m.x12 * pos.y + m.x22 * pos.z + m.x32;

    nkMat4 res = m;
    res.x30 = x30;
    res.x31 = x31;
    res.x32 = x32;
    return res;
}

NKAPI NKFORCEINLINE nkMat4 nk_rotate(const nkMat4& m, const nkVec3& axis, nkF32 angle)
{
    nkF32 c = cosf(angle);
    nkF32 s = sinf(angle);

    nkVec3 a = nk_normalize(axis); // Ensure that the rotation axis is unit length.
    nkVec3 t = { ((1.f-c)*a.x), ((1.f-c)*a.y), ((1.f-c)*a.z) };

    nkMat4 r;
    r.x00 = c + t.x * a.x;
    r.x01 = t.x * a.y + s * a.z;
    r.x02 = t.x * a.z - s * a.y;
    r.x10 = t.y * a.x - s * a.z;
    r.x11 = c + t.y * a.y;
    r.x12 = t.y * a.z + s * a.x;
    r.x20 = t.z * a.x + s * a.y;
    r.x21 = t.z * a.y - s * a.x;
    r.x22 = c + t.z * a.z;

    nkMat4 tmp;
    tmp.x00 = m.x00 * r.x00 + m.x10 * r.x01 + m.x20 * r.x02;
    tmp.x01 = m.x01 * r.x00 + m.x11 * r.x01 + m.x21 * r.x02;
    tmp.x02 = m.x02 * r.x00 + m.x12 * r.x01 + m.x22 * r.x02;
    tmp.x10 = m.x00 * r.x10 + m.x10 * r.x11 + m.x20 * r.x12;
    tmp.x11 = m.x01 * r.x10 + m.x11 * r.x11 + m.x21 * r.x12;
    tmp.x12 = m.x02 * r.x10 + m.x12 * r.x11 + m.x22 * r.x12;
    tmp.x20 = m.x00 * r.x20 + m.x10 * r.x21 + m.x20 * r.x22;
    tmp.x21 = m.x01 * r.x20 + m.x11 * r.x21 + m.x21 * r.x22;
    tmp.x22 = m.x02 * r.x20 + m.x12 * r.x21 + m.x22 * r.x22;

    nkMat4 res = m;
    res.x00 = tmp.x00;
    res.x01 = tmp.x01;
    res.x02 = tmp.x02;
    res.x10 = tmp.x10;
    res.x11 = tmp.x11;
    res.x12 = tmp.x12;
    res.x20 = tmp.x20;
    res.x21 = tmp.x21;
    res.x22 = tmp.x22;
    return res;
}

NKAPI NKFORCEINLINE nkMat4 nk_scale(const nkMat4& m, const nkVec3& scale)
{
    nkMat4 res = m;
    res.x00 *= scale.x;
    res.x01 *= scale.x;
    res.x02 *= scale.x;
    res.x10 *= scale.y;
    res.x11 *= scale.y;
    res.x12 *= scale.y;
    res.x20 *= scale.z;
    res.x21 *= scale.z;
    res.x22 *= scale.z;
    return res;
}

#endif /* NK_MATHX_H__ ///////////////////////////////////////////////////////*/

/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2022 Joshua Robertson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
*******************************************************************************/

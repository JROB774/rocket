#pragma once

#include <stdint.h>

#define CSM_PUBLIC_SCOPE namespace csm
#define CSM_PRIVATE_SCOPE namespace

#ifndef CSM_API
#ifdef CSM_STATIC
#define CSM_API static
#else
#define CSM_API
#endif
#endif

#ifndef CSM_INLINE
#define CSM_INLINE inline
#endif

#ifndef CSM_CAST
#define CSM_CAST(t,x) ((t)(x))
endif

#ifndef CSM_ASSERT
#include <assert.h>
#define CSM_ASSERT(e,msg) assert(e)
#else
#define CSM_ASSERT(e,msg) (void)(0)
#endif
#endif

CSM_PUBLIC_SCOPE
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
}

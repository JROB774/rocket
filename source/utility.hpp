#ifdef BUILD_DEBUG
#include <assert.h>
#define ASSERT(e,msg) assert(e)
#else
#define ASSERT(e,msg) ((void)0)
#endif

#define DECLARE_PRIVATE_STRUCT(name) struct name##__Type; typedef name##__Type* name;
#define DEFINE_PRIVATE_STRUCT(name) struct name##__Type

#define GET_PTR_TYPE(ptr) RemovePtr<decltype(ptr)>::Type

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

static void RandomSeed(s32 seed = -1);
static s32 RandomS32();
static s32 RandomS32(s32 min, s32 max);
static f32 RandomF32();
static f32 RandomF32(f32 min, f32 max);

template<typename T>
static bool Contains(std::vector<T>& vec, const T& x);
template<typename K, typename V>
static bool Contains(std::map<K,V>& map, const K& x);

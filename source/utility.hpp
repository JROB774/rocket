#if defined(__COUNTER__)
#define DEFER const auto& JOIN(defer, __COUNTER__) = DeferHelp() + [&]()
#else
#define DEFER const auto& JOIN(defer, __LINE__) = DeferHelp() + [&]()
#endif

#define GET_PTR_TYPE(ptr) RemovePtr<decltype(ptr)>::Type

static void RandomSeed(s32 seed = -1);
static s32 RandomS32();
static s32 RandomS32(s32 min, s32 max);
static f32 RandomF32();
static f32 RandomF32(f32 min, f32 max);

static f32 SinRange(f32 min, f32 max, f32 t);
static nkVec2 RotateVec2(nkVec2 vec, f32 rad);

template<typename T>
static bool Contains(std::vector<T>& vec, const T& x);
template<typename K, typename V>
static bool Contains(std::map<K,V>& map, const K& x);

// @Incomplete: Move into the source file...

// Internal implementation details for DEFER.
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

// Internal implementation details for GET_PTR_TYPE.
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

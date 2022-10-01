static std::random_device s_randomDevice;
static std::mt19937       s_randomGenerator(s_randomDevice());

static void RandomSeed(s32 seed)
{
    if(seed < 0) seed = CS_CAST(s32, time(NULL));
    s_randomGenerator.seed(seed);
}

static s32 RandomS32()
{
    return RandomS32(0, RAND_MAX);
}

static s32 RandomS32(s32 min, s32 max)
{
    std::uniform_int_distribution<s32> distribution(min, max);
    return distribution(s_randomGenerator);
}

static f32 RandomF32()
{
    return RandomF32(0, CS_CAST(f32, RAND_MAX));
}

static f32 RandomF32(f32 min, f32 max)
{
    std::uniform_real_distribution<f32> distribution(min, max);
    return distribution(s_randomGenerator);
}

static f32 SinRange(f32 min, f32 max, f32 t)
{
    f32 halfRange = (max - min) / 2;
    return (min + halfRange + sinf(t) * halfRange);
}

static nkVec2 RotateVec2(nkVec2 vec, f32 rad)
{
    f32 x = vec.x * cosf(rad) - vec.y * sinf(rad);
    f32 y = vec.x * sinf(rad) + vec.y * cosf(rad);
    return { x,y };
}

template<typename T>
static bool Contains(std::vector<T>& vec, const T& x)
{
    return (std::find(vec.begin(), vec.end(), x) != vec.end());
}
template<typename K, typename V>
static bool Contains(std::map<K,V>& map, const K& x)
{
    return (map.find(x) != map.end());
}

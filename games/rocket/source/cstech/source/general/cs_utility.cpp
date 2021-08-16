#include "cs_utility.hpp"
#include "cs_platform.hpp"

#include <random>

CS_PRIVATE_SCOPE
{
    static std::random_device s_randomDevice;
    static std::mt19937 s_randomGenerator(s_randomDevice());
}

CS_PUBLIC_SCOPE
{
    CS_API void RandomSeed(s32 seed)
    {
        if(seed < 0) seed = CS_CAST(s32, time(NULL));
        s_randomGenerator.seed(seed);
    }

    CS_API s32 RandomS32()
    {
        return RandomS32(0, RAND_MAX);
    }

    CS_API s32 RandomS32(s32 min, s32 max)
    {
        std::uniform_int_distribution<s32> distribution(min, max);
        return distribution(s_randomGenerator);
    }

    CS_API f32 RandomF32()
    {
        return RandomF32(0, CS_CAST(f32, RAND_MAX));
    }

    CS_API f32 RandomF32(f32 min, f32 max)
    {
        std::uniform_real_distribution<f32> distribution(min, max);
        return distribution(s_randomGenerator);
    }

    CS_API nlohmann::json LoadJSONFromFile(std::string fileName)
    {
        std::string jstr = ReadEntireFile(fileName);
        if(!jstr.empty()) return nlohmann::json::parse(jstr.c_str());
        return nlohmann::json();
    }
}

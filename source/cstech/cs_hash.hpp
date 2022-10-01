#pragma once

#include <sstream>
#include <string>
#include <iomanip>

CS_PUBLIC_SCOPE
{
    // 128-bit MD5 hash value.
    struct MD5
    {
        u8 digest[16];
    };

    CS_API MD5 HashMD5(const u8* msg, size_t length);

    CS_API inline std::string MD5ToString(const MD5& md5)
    {
        std::stringstream stream;
        stream << std::hex;
        for(size_t i=0; i<CS_ARRAY_SIZE(md5.digest); ++i)
            stream << std::setw(2) << std::setfill('0') << CS_CAST(s32,md5.digest[i]);
        return stream.str();
    }
}

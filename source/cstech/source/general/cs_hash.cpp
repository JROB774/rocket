#include "cs_hash.hpp"
#include "cs_utility.hpp"
#include "cs_memory.hpp"

// Simple implementation of the MD5 hashing algorithm based off the pseudocode found here:
//     https://en.wikipedia.org/wiki/MD5#pseudocode

CS_PUBLIC_SCOPE
{
    CS_API MD5 HashMD5(const u8* msg, size_t length)
    {
        // Binary integer part of the sines of integers (radians).
        const u32 k[64]
        {
            0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
            0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
            0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
            0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
            0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
            0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
            0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
            0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
            0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
            0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
            0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
            0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
            0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
            0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
            0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
            0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
        };

        // Per-round shift amounts.
        const u32 s[64]
        {
            7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
            5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
            4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
            6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
        };

        // Calculate the new message length and allocate it.
        size_t newLength = length+1;
        while(newLength % (512/8) != 448/8) ++newLength; // The new length is based on adding bits until length in bits == 448 (mod 512).
        u8* newMsg = Allocate<u8>(CS_MEM_SYSTEM, newLength+8); // We add +8 for storing the u64 original length in bits at the end.
        if(!newMsg) return {};
        CS_DEFER { Deallocate(newMsg); };
        memcpy(newMsg, msg, length);

        // Pre-processing of the message.

        newMsg[length] = 0x80; // Append "1" bit to message.
        size_t padding = length+1;
        while(padding < newLength) // Append "0" bits to message.
            newMsg[padding++] = 0x00;

        // Append the length in bits at the end.
        u64 lengthInBits = length*8;
        newMsg[newLength+0] = CS_CAST(u8, lengthInBits      );
        newMsg[newLength+1] = CS_CAST(u8, lengthInBits >>  8);
        newMsg[newLength+2] = CS_CAST(u8, lengthInBits >> 16);
        newMsg[newLength+3] = CS_CAST(u8, lengthInBits >> 24);
        newMsg[newLength+4] = CS_CAST(u8, lengthInBits >> 32);
        newMsg[newLength+5] = CS_CAST(u8, lengthInBits >> 40);
        newMsg[newLength+6] = CS_CAST(u8, lengthInBits >> 48);
        newMsg[newLength+7] = CS_CAST(u8, lengthInBits >> 56);

        // Process message in successive 512-bit chunks.

        u32 a = 0x67452301;
        u32 b = 0xefcdab89;
        u32 c = 0x98badcfe;
        u32 d = 0x10325476;

        // For each 512-bit message chunk.
        u32 m[16];
        for(size_t i=0; i<newLength; i+=(512/8))
        {
            // Break chunk into sixteen 32-bit words.
            for(size_t j=0; j<16; ++j)
            {
                const u8* bytes = newMsg+i+j*4;
                m[j] = (CS_CAST(u32, bytes[0]      ) |
                        CS_CAST(u32, bytes[1] <<  8) |
                        CS_CAST(u32, bytes[2] << 16) |
                        CS_CAST(u32, bytes[3] << 24));
            }

            // Initialise hash value for this chunk.
            u32 a0 = a;
            u32 b0 = b;
            u32 c0 = c;
            u32 d0 = d;

            // Main loop.
            for(u32 j=0; j<64; ++j)
            {
                u32 f,g;

                if(j <= 15)
                {
                    f = (b0 & c0) | ((~b0) & d0);
                    g = j;
                }
                else if(j <= 31)
                {
                    f = (d0 & b0) | ((~d0) & c0);
                    g = (5*j + 1) % 16;
                }
                else if(j <= 47)
                {
                    f = b0 ^ c0 ^ d0;
                    g = (3*j + 5) % 16;
                }
                else // Is (j <= 63) but it's equivalent to "else" and will avoid warnings of f and g being uninitialised.
                {
                    f = c0 ^ (b0 | (~d0));
                    g = (7*j) % 16;
                }

                f = f + a0 + k[j] + m[g];

                a0 = d0;
                d0 = c0;
                c0 = b0;
                b0 = b0 + ((f << s[j]) | (f >> (32-s[j]))); // Left rotate.
            }

            // Add this chunk's result to the hash.
            a += a0;
            b += b0;
            c += c0;
            d += d0;
        }

        // Construct the final hash digest.
        MD5 md5;
        md5.digest[ 0] = CS_CAST(u8, a      );
        md5.digest[ 1] = CS_CAST(u8, a >>  8);
        md5.digest[ 2] = CS_CAST(u8, a >> 16);
        md5.digest[ 3] = CS_CAST(u8, a >> 24);
        md5.digest[ 4] = CS_CAST(u8, b      );
        md5.digest[ 5] = CS_CAST(u8, b >>  8);
        md5.digest[ 6] = CS_CAST(u8, b >> 16);
        md5.digest[ 7] = CS_CAST(u8, b >> 24);
        md5.digest[ 8] = CS_CAST(u8, c      );
        md5.digest[ 9] = CS_CAST(u8, c >>  8);
        md5.digest[10] = CS_CAST(u8, c >> 16);
        md5.digest[11] = CS_CAST(u8, c >> 24);
        md5.digest[12] = CS_CAST(u8, d      );
        md5.digest[13] = CS_CAST(u8, d >>  8);
        md5.digest[14] = CS_CAST(u8, d >> 16);
        md5.digest[15] = CS_CAST(u8, d >> 24);
        return md5;
    }
}

/*////////////////////////////////////////////////////////////////////////////*/

#define NK_PACK_IMPLEMENTATION
#define NK_FILESYS_IMPLEMENTATION

#define NK_STATIC

#include <stdio.h>

#include <nk_pack.h>

int main(int argc, char** argv)
{
    printf("packing game assets into npak... ");
    nkBool res = nk_npak_pack("binary/win32/assets.npak", "assets");
    printf("%s!\n", res ? "successful" : "failure");
    return 0;
}

/*////////////////////////////////////////////////////////////////////////////*/

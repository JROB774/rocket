#include "cs_platform.hpp"
#include "cs_application.hpp"
#include "cs_graphics.hpp"
#include "cs_utility.hpp"

using namespace cs;

class ChompApp: public Application
{
public:
    static inline const f32 k_chompPosMin = 104.0f;
    static inline const f32 k_chompPosMax = 168.0f;
    static inline const f32 k_chompDownVelocity = 180.0f;
    static inline const f32 k_chompUpVelocity = 35.0f;
    static inline const f32 k_chompMinVelocity = -325.0f;
    static inline const f32 k_chompMaxVelocity = 870.0f;
    static inline const f32 k_chompCooldown = 0.15f;
    static inline const f32 k_cloudSpeed = 10.0f;
    static inline const f32 k_saddoSpeed = 60.0f;
    static inline const f32 k_saddoMinSpawn = 0.33f;
    static inline const f32 k_saddoMaxSpawn = 2.25f;

    static inline const Vec4 k_bloodColors[] {
    RGBAToVec4(114,44,42,255),
    RGBAToVec4(158,14,31,255),
    RGBAToVec4(107,36,39,255),
    RGBAToVec4(119,21,14,255),
    RGBAToVec4(142, 0,21,255),
    RGBAToVec4(122, 6,39,255),
    RGBAToVec4( 86,22,32,255),
    RGBAToVec4( 99, 0, 4,255)};

    static const inline Rect k_splatX[] {
    {   0,  0,32,32 },
    {  32,  0,32,32 },
    {  64,  0,32,32 },
    {  96,  0,32,32 },
    { 128,  0,32,32 },
    {   0, 32,32,32 },
    {  32, 32,32,32 },
    {  64, 32,32,32 },
    {  96, 32,32,32 }};
    static const inline Rect k_splatL[] {
    {   0, 64,24,24 },
    {  24, 64,24,24 },
    {  48, 64,24,24 },
    {  72, 64,24,24 },
    {  96, 64,24,24 },
    { 120, 64,24,24 },
    {   0, 88,24,24 },
    {  24, 88,24,24 },
    {  48, 88,24,24 }};
    static const inline Rect k_splatM[] {
    {   0,112,16,16 },
    {  16,112,16,16 },
    {  32,112,16,16 },
    {  48,112,16,16 },
    {  64,112,16,16 },
    {  80,112,16,16 },
    {  96,112,16,16 },
    { 112,112,16,16 },
    { 128,112,16,16 }};
    static const inline Rect k_splatS[] {
    {   0,128,16,16 },
    {  16,128,16,16 },
    {  32,128,16,16 },
    {  48,128,16,16 },
    {  64,128,16,16 },
    {  80,128,16,16 },
    {  96,128,16,16 },
    { 112,128,16,16 },
    { 128,128,16,16 }};

    enum SaddoType
    {
        SaddoType_0,
        SaddoType_1,
        SaddoType_2
    };

    struct Chomp
    {
        Vec2 pos;
        f32 vel;
        f32 landedCooldown;
        Rect collider;
        bool chomping;
        bool landed;
        bool bloody;
    };

    struct Saddo
    {
        Vec2 pos;
        SaddoType type;
        Rect collider;
        bool dead;
    };

    gfx::Framebuffer m_bloodMap = NULL;
    std::vector<Saddo> m_saddos;
    f32 m_saddoSpawnTimer = 0.0f;
    f32 m_cloudOffset = 0.0f;
    Chomp m_chomp = {};

    bool CollisionCheck(Vec2 aPos, Rect aBounds, Vec2 bPos, Rect bBounds)
    {
        Rect a,b;

        a.x = aPos.x + aBounds.x;
        a.y = aPos.y + aBounds.y;
        a.w =          aBounds.w;
        a.h =          aBounds.h;
        b.x = bPos.x + bBounds.x;
        b.y = bPos.y + bBounds.y;
        b.w =          bBounds.w;
        b.h =          bBounds.h;

        return ((a.x + a.w > b.x) && (a.y + a.h > b.y) &&
                (a.x < b.x + b.w) && (a.y < b.y + b.h));
    }

    void SpawnSplats(f32 minX, f32 maxX, f32 minY, f32 maxY, f32 min, f32 max, const Rect* splats)
    {
        s32 count = RandomS32(min,max);
        for(s32 i=0; i<count; ++i)
        {
            Vec4 color = k_bloodColors[RandomS32(0,7)];
            color.a = RandomF32(0.85f, 1.0f);
            imm::DrawTexture("blood_splats", roundf(RandomF32(minX,maxX)),roundf(RandomF32(minY,maxY)), &splats[RandomS32(0,8)], color);
        }
    }

    void DoSaddoSplats(const Saddo& saddo)
    {
        gfx::SetRenderTarget(m_bloodMap);
        CS_DEFER { gfx::SetRenderTarget(); };

        f32 minX = (saddo.pos.x+10) - 35.0f;
        f32 maxX = (saddo.pos.x+10) + 55.0f;
        f32 minY = (saddo.pos.y-10) + 10.0f;
        f32 maxY = (saddo.pos.y-10) + 45.0f;

        SpawnSplats(minX,maxX,minY,maxY, 1,3, k_splatX);
        SpawnSplats(minX,maxX,minY,maxY, 2,5, k_splatL);
        SpawnSplats(minX,maxX,minY,maxY, 3,6, k_splatM);
        SpawnSplats(minX,maxX,minY,maxY, 2,8, k_splatS);
    }

    void Init()
    {
        ShowCursor(false);

        gfx::SetScreenScaleMode(gfx::ScaleMode_Pixel);
        gfx::SetScreenFilter(gfx::Filter_Nearest);

        LoadAllAssets<gfx::Texture>();
        LoadAllAssets<gfx::Shader>();

        gfx::SetTextureFilter(*GetAsset<gfx::Texture>("mask"), gfx::Filter_Nearest);

        f32 screenX = gfx::GetScreenWidth();
        f32 screenY = gfx::GetScreenHeight();

        gfx::CreateFramebuffer(m_bloodMap, 340,200, gfx::Filter_Nearest, gfx::Wrap_Clamp, Vec4(0));

        m_saddoSpawnTimer = RandomF32(k_saddoMinSpawn, k_saddoMaxSpawn);
        m_cloudOffset = gfx::GetScreenWidth();

        m_chomp.pos = Vec2(196.0f,104.0f);
        m_chomp.vel = 0.0f;
        m_chomp.landedCooldown = 0.0f;
        m_chomp.collider = { -16,-72,32,32 };
        m_chomp.chomping = false;
        m_chomp.landed = false;
    }

    void Quit()
    {
        FreeFramebuffer(m_bloodMap);
    }

    void Update(f32 dt)
    {
        f32 screenX = gfx::GetScreenWidth();
        f32 screenY = gfx::GetScreenHeight();

        // Cloud Logic
        {
            m_cloudOffset -= k_cloudSpeed * dt;
            if(m_cloudOffset <= -screenX) // Loop around.
            {
                m_cloudOffset += screenX * 2.0f;
            }
        }

        // Chomp Logic
        {
            // If we've landed we cooldown/freeze for a bit.
            if(m_chomp.landedCooldown > 0.0f)
            {
                m_chomp.landedCooldown -= dt;
            }
            else
            {
                // Once we've landed and cooled down we go straight up.
                if(m_chomp.landed)
                {
                    m_chomp.chomping = false;
                    m_chomp.landed = false;
                }
                // Check input.
                if(IsKeyPressed(KeyCode_Space))
                {
                    m_chomp.vel += k_chompDownVelocity; // Extra boost on initial chomp.
                    m_chomp.chomping = true;
                    m_chomp.landed = false;
                }
                // Update velocity.
                if(m_chomp.chomping) m_chomp.vel += k_chompDownVelocity;
                else m_chomp.vel -= k_chompUpVelocity;
                m_chomp.vel = csm::Clamp(m_chomp.vel, k_chompMinVelocity, k_chompMaxVelocity);
                // Update position.
                m_chomp.pos.y += m_chomp.vel * dt;
                m_chomp.pos.y = csm::Clamp(m_chomp.pos.y, k_chompPosMin, k_chompPosMax);
                // Check landed.
                if(m_chomp.pos.y >= k_chompPosMax)
                {
                    m_chomp.vel = 0.0f;
                    m_chomp.landed = true;
                    m_chomp.landedCooldown = k_chompCooldown;
                }
            }
        }

        // Saddo Logic
        {
            // Spawn more saddos.
            m_saddoSpawnTimer -= dt;
            if(m_saddoSpawnTimer <= 0.0f)
            {
                Saddo saddo;
                saddo.pos.x = -64.0f;
                saddo.pos.y = 122.0f;
                saddo.type = CS_CAST(SaddoType, RandomS32(SaddoType_0, SaddoType_2));
                saddo.collider = { -6,-2,12,14 };
                saddo.dead = false;
                m_saddos.push_back(saddo);
                m_saddoSpawnTimer = RandomF32(k_saddoMinSpawn, k_saddoMaxSpawn);
            }
            // Update each of the saddos.
            for(auto& saddo: m_saddos)
            {
                saddo.pos.x += k_saddoSpeed * dt;
                if(saddo.pos.x >= screenX+32.0f)
                    saddo.dead = true;
                // Check collision with chomp.
                if(CollisionCheck(m_chomp.pos,m_chomp.collider, saddo.pos,saddo.collider))
                {
                    m_chomp.bloody = true;
                    saddo.dead = true;
                    DoSaddoSplats(saddo);
                }
            }
            // Remove dead saddos.
            m_saddos.erase(std::remove_if(m_saddos.begin(), m_saddos.end(),
            [](const Saddo& saddo)
            {
                return saddo.dead;
            }),
            m_saddos.end());
        }
    }

    void Render(f32 dt)
    {
        gfx::Texture background = *GetAsset<gfx::Texture>("background");
        gfx::Texture foreground = *GetAsset<gfx::Texture>("foreground");
        gfx::Texture clouds     = *GetAsset<gfx::Texture>("clouds");
        gfx::Texture chomp      = *GetAsset<gfx::Texture>("chomp");
        gfx::Texture mask       = *GetAsset<gfx::Texture>("mask");

        f32 screenX = gfx::GetScreenWidth();
        f32 screenY = gfx::GetScreenHeight();
        f32 halfX   = screenX / 2.0f;
        f32 halfY   = screenY / 2.0f;

        f32 cloudX = roundf(m_cloudOffset);
        f32 cloudY = halfY;

        f32 chompTopX = roundf(m_chomp.pos.x);
        f32 chompTopY = roundf(m_chomp.pos.y) - 56;
        f32 chompMidX = chompTopX;
        f32 chompMidY = roundf(m_chomp.pos.y);
        f32 chompBtmX = chompMidX;
        f32 chompBtmY = 158.0f;

        Rect chompTop = { 0,  0,40, 40 };
        Rect chompMid = { 0, 40,40,152 };
        Rect chompBtm = { 0,192,40, 64 };

        // If we're bloody then use the appropriate graphics.
        if(m_chomp.bloody)
        {
            chompTop.x += 40;
            chompMid.x += 40;
            chompBtm.x += 40;
        }

        // Do actual drawing.
        gfx::Clear(RGBAToVec4(61,63,191));
        imm::DrawTexture(background, halfX,halfY);
        imm::DrawTexture(clouds, cloudX,cloudY);
        imm::DrawTexture(clouds, cloudX+(screenX*2.0f),halfY);
        imm::DrawTexture(chomp, chompMidX,chompMidY, &chompMid);
        imm::DrawTexture(foreground, halfX,halfY);
        for(auto& saddo: m_saddos)
            imm::DrawTexture("saddo_" + std::to_string(saddo.type), roundf(saddo.pos.x),roundf(saddo.pos.y));
        imm::DrawTexture(chomp, chompTopX,chompTopY, &chompTop);
        imm::DrawTexture(chomp, chompBtmX,chompBtmY, &chompBtm);

        // Draw the blood map.
        imm::SetCurrentShader("blood");
        imm::SetCurrentTexture(GetFramebufferTexture(m_bloodMap), 0);
        imm::SetCurrentTexture(mask, 1);
        bool textureMapping = imm::IsTextureMappingEnabled();
        imm::EnableTextureMapping(true);
        imm::BeginDraw(gfx::DrawMode_TriangleStrip);
        gfx::SetShaderInt("u_texture0", 0);
        gfx::SetShaderInt("u_bloodMask", 1);
        imm::PutVertex({ {-10.0f,  190.0f}, Vec4(1,0,0,1), {0,0} });
        imm::PutVertex({ {-10.0f,  -10.0f}, Vec4(1,0,0,1), {0,1} });
        imm::PutVertex({ {330.0f,  190.0f}, Vec4(1,0,0,1), {1,0} });
        imm::PutVertex({ {330.0f,  -10.0f}, Vec4(1,0,0,1), {1,1} });
        imm::EndDraw();
        imm::EnableTextureMapping(textureMapping);
        imm::SetCurrentShader("");
    }

    void DebugRender(f32 dt)
    {
        f32 x = roundf(m_chomp.pos.x), y = roundf(m_chomp.pos.y);
        Rect c = m_chomp.collider;
        imm::DrawRectFilled(x+c.x,y+c.y,x+c.x+c.w,y+c.y+c.h, Vec4(0,1,0,0.5f));
        imm::DrawRectOutline(x+c.x,y+c.y,x+c.x+c.w,y+c.y+c.h, Vec4(0,1,0,1));

        for(auto& saddo: m_saddos)
        {
            x = roundf(saddo.pos.x), y = roundf(saddo.pos.y);
            c = saddo.collider;
            imm::DrawRectFilled(x+c.x,y+c.y,x+c.x+c.w,y+c.y+c.h, Vec4(1,0,0,0.5f));
            imm::DrawRectOutline(x+c.x,y+c.y,x+c.x+c.w,y+c.y+c.h, Vec4(1,0,0,1));
        }
    }
};

AppConfig csMain(int argc, char** argv)
{
    AppConfig appConfig;
    appConfig.title = "Chomp";
    appConfig.window.min = Vec2i(320,180);
    appConfig.screenSize = Vec2i(320,180);
    appConfig.app = Allocate<ChompApp>(CS_MEM_GAME);
    return appConfig;
}

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
    };

    struct Saddo
    {
        Vec2 pos;
        SaddoType type;
        Rect collider;
        bool dead;
    };

    std::vector<Saddo> m_saddos;
    f32 m_saddoSpawnTimer;
    f32 m_cloudOffset;
    Chomp m_chomp;

    void Init()
    {
        gfx::SetScreenScaleMode(gfx::ScaleMode_Pixel);
        gfx::SetScreenFilter(gfx::Filter_Nearest);

        m_saddoSpawnTimer = RandomF32(k_saddoMinSpawn, k_saddoMaxSpawn);
        m_cloudOffset = gfx::GetScreenWidth();

        m_chomp.pos = Vec2(196.0f,104.0f);
        m_chomp.vel = 0.0f;
        m_chomp.landedCooldown = 0.0f;
        m_chomp.collider = { -16,-72,32,32 };
        m_chomp.chomping = false;
        m_chomp.landed = false;
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
                m_chomp.vel = Clamp(m_chomp.vel, k_chompMinVelocity, k_chompMaxVelocity);
                // Update position.
                m_chomp.pos.y += m_chomp.vel * dt;
                m_chomp.pos.y = Clamp(m_chomp.pos.y, k_chompPosMin, k_chompPosMax);
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

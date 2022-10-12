static void CreateSmoke()
{
    s_smoke.reserve(4096);
}

static void SpawnSmoke(SmokeType type, f32 x, f32 y, s32 count)
{
    for(s32 i=0; i<count; ++i)
    {
        Smoke s = {};
        s.type = type;
        s.pos = { x,y };
        s.frame = 0;
        s.angle = RandomF32(0,360.0f);
        s.vel = nk_v2_rotate({ RandomF32(80,140),0 }, nk_torad(s.angle));
        if(type == SmokeType_Blood) s.vel = nk_v2_rotate({ 180.0f,0 }, nk_torad(RandomF32(45.0f,135.0f)));
        s.spin = RandomF32(400,600);
        s.scale = (s.type == SmokeType_Small || s.type == SmokeType_SmallStationary) ? 0.5f : 1.0f;
        if(s.type == SmokeType_Thruster || s.type == SmokeType_Blood) s.scale *= 1.0f;
        s.timer = 0.0f;
        s.frameTime = RandomF32(0.05f, 0.15f);
        if(type == SmokeType_Explosion) s.spawner = RandomS32(1,100) <= 10;
        else s.spawner = false;
        s.dead = false;
        if(s.spawner) s.vel += (s.vel * 3.0f) + RandomF32(0,40);
        s_smoke.push_back(s);
    }
}

static void UpdateSmoke(f32 dt)
{
    for(auto& s: s_smoke)
    {
        s.timer += dt;
        if(s.type == SmokeType_Small)
        {
            s.timer += dt;
        }
        if(s.timer >= s.frameTime)
        {
            s.frame++;
            s.timer = 0.0f;
            s.frameTime = RandomF32(0.05f, 0.15f);
        }
        if(s.frame >= 8)
        {
            s.dead = true;
        }

        // Different smoke types move differently.
        switch(s.type)
        {
            case(SmokeType_Thruster):
            {
                s.pos.y += 180.0f * dt;
            } break;
            case(SmokeType_Blood):
            {
                s.pos += s.vel * dt;
            } break;
            case(SmokeType_Small):
            case(SmokeType_Explosion):
            {
                s.pos += s.vel * dt;
                s.angle += s.spin * dt;
                if(s.type != SmokeType_Small)
                {
                    if(s.spawner)
                    {
                        if(RandomS32(1,100) < 25)
                        {
                            SpawnSmoke(SmokeType_Stationary, s.pos.x, s.pos.y, 1);
                        }
                    }
                    else
                    {
                        if(RandomS32(1,100) < 5)
                        {
                            SpawnSmoke(SmokeType_Stationary, s.pos.x, s.pos.y, 1);
                        }
                    }
                }
            } break;
            default:
            {
                // Nothing...
            } break;
        }
    }

    s_smoke.erase(std::remove_if(s_smoke.begin(), s_smoke.end(),
    [](const Smoke& s)
    {
        return s.dead;
    }),
    s_smoke.end());
}

static void RenderSmoke(f32 dt)
{
    imm::BeginTextureBatch("smoke");
    for(auto& s: s_smoke)
    {
        Rect clip = { NK_CAST(f32, 16*s.frame), 16*NK_CAST(f32, s_rocket.costume), 16, 16 };
        imm::DrawBatchedTexture(s.pos.x, s.pos.y, s.scale,s.scale, nk_torad(s.angle), imm::Flip_None, NULL, &clip);
    }
    imm::EndTextureBatch();
}

static void StartThruster()
{
    if(s_rocket.thruster != k_invalidSoundRef) return;
    std::string thruster;
    thruster = "thruster";
    switch(s_rocket.costume)
    {
        case(Costume_Meat): thruster = "squirt"; break;
        case(Costume_Doodle): thruster = "mouth0"; break;
        case(Costume_Rainbow): thruster = "sparkle"; break;
        case(Costume_Glitch): thruster = "static"; break;
        default:
        {
            // Nothing...
        } break;
    }
    s_rocket.thruster = PlaySound(thruster, -1);
}

static void StopThruster()
{
    StopSound(s_rocket.thruster);
    s_rocket.thruster = k_invalidSoundRef;
}

static void CreateRocket()
{
    s_rocket.pos.x = (GetScreenWidth()  *  0.5f);
    s_rocket.pos.y = (GetScreenHeight() - 32.0f);
    s_rocket.vel   = { 0,0 };
    s_rocket.angle = 0.0f;
    s_rocket.shake = 0.0f;
    s_rocket.timer = 1000.0f; // Stop the explosion on start.
    s_rocket.score = 0;
    s_rocket.frame = 0;
    s_rocket.dead  = true;
    s_rocket.collider = { { 0,-8 }, 8.0f };
    s_rocket.collector = { { 0,-8 }, 40.0f };
    s_rocket.costume = Costume_Red;
    s_rocket.thruster = k_invalidSoundRef;
    s_rocket.random = (s_rocket.costume == Costume_Random);
    LoadGame();
}

static s32 ScoreCompare(const void* a, const void* b)
{
    u32* aa = NK_CAST(u32*,a);
    u32* bb = NK_CAST(u32*,b);
    if(*aa > *bb) return -1;
    if(*aa < *bb) return  1;
    return 0;
}

static void HitRocket()
{
    StopThruster();

    SpawnSmoke(SmokeType_Explosion, s_rocket.pos.x, s_rocket.pos.y, RandomS32(20,40));

    std::string explosion = "explosion";
    switch(s_rocket.costume)
    {
        case(Costume_Meat): explosion = "splat"; break;
        case(Costume_Doodle): explosion = "mouth2"; break;
        case(Costume_Rainbow): explosion = "ignite"; break;
        case(Costume_Glitch): explosion = "glitch"; break;
        default:
        {
            // Nothing...
        } break;
    }
    PlaySound(explosion);

    s_rocket.timer = 0.0f;
    s_rocket.dead = true;

    // Check to see if any new costumes were unlocked.
    for(s32 i=0; i<NK_ARRAY_SIZE(k_unlocks); ++i)
    {
        const Unlock& unlock = k_unlocks[i];
        if(s_rocket.score >= unlock.score)
        {
            if(!s_rocket.unlocks[unlock.costume]) // Not already unlocked.
            {
                s_rocket.unlocks[unlock.costume] = true;
                s_gameOverUnlocks.push_back(unlock.costume);
            }
        }
    }

    // Determine if the new score is a highscore and then save.
    if(s_rocket.score > s_rocket.highscores[9])
    {
        s_rocket.highscores[9] = s_rocket.score;
        qsort(s_rocket.highscores,NK_ARRAY_SIZE(s_rocket.highscores),sizeof(s_rocket.highscores[0]),ScoreCompare);
    }
    SaveGame();

    GoToGameOverMenu();
}

static void UpdateRocket(f32 dt)
{
    if(s_gameState != GameState_Game) return;

    s_rocket.timer += dt;

    if(s_rocket.costume == Costume_Doodle)
    {
        if(s_gameFrame % 3 == 0)
            s_rocket.frame++;
    }
    else
    {
        s_rocket.frame++;
    }
    // Make sure the frame stays in-bounds of the animation.
    if(s_rocket.frame >= 5)
    {
        s_rocket.frame = 0;
    }

    if(!s_rocket.dead)
    {
        if(IsMouseLocked() && (s_gameState == GameState_Game))
        {
            static f32 s_prevMouseX = 0.0f;
            static f32 s_currMouseX = 0.0f;

            s_prevMouseX = s_currMouseX;
            s_currMouseX = GetRelativeMousePos().x;

            s_rocket.vel.x += GetRelativeMousePos().x / 10.0f;
            s_rocket.vel.y += GetRelativeMousePos().y / 20.0f;

            // If the player moved the mouse fast enough then play a whoosh sound.
            static bool s_canPlayWhoosh = true;
            static f32 s_whooshVel = 0.0f;
            if(abs((s_currMouseX - s_prevMouseX) * dt) >= 35.0f * dt)
            {
                if(s_canPlayWhoosh)
                {
                    std::string whoosh = "whoosh";
                    switch(s_rocket.costume)
                    {
                        case(Costume_Meat): whoosh = "squelch"; SpawnSmoke(SmokeType_Small, s_rocket.pos.x, s_rocket.pos.y, RandomS32(2,5)); break;
                        case(Costume_Doodle): whoosh = "mouth1"; break;
                        case(Costume_Rainbow): whoosh = "magic"; break;
                        case(Costume_Glitch): whoosh = "fuzz"; break;
                        default:
                        {
                            // Nothing...
                        } break;
                    }
                    PlaySound(whoosh);
                    s_whooshVel = s_rocket.vel.x;
                    s_canPlayWhoosh = false;
                }
            }
            if(!s_canPlayWhoosh)
            {
                // Direction change.
                if(s_whooshVel > 0.0f && s_rocket.vel.x < 0.0f || s_whooshVel < 0.0f && s_rocket.vel.x > 0.0f)
                    s_canPlayWhoosh = true;
                // Speed goes down.
                if(s_rocket.vel.x <= 5.0f && s_rocket.vel.x >= -5.0f)
                    s_canPlayWhoosh = true;
            }
        }

        s_rocket.angle = nk_clamp(s_rocket.vel.x, -k_rocketMaxAngle, k_rocketMaxAngle);
        s_rocket.shake = RandomF32(-k_rocketMaxShake, k_rocketMaxShake);

        s_rocket.vel.x = nk_clamp(s_rocket.vel.x, -(k_rocketTerminalVelocity*1.5f), (k_rocketTerminalVelocity*1.5f));
        s_rocket.vel.y = nk_clamp(s_rocket.vel.y, -k_rocketTerminalVelocity, k_rocketTerminalVelocity);

        s_rocket.pos = nk_v2addv(s_rocket.pos, nk_v2mulf(nk_v2mulf(s_rocket.vel, k_rocketVelocityMultiplier), dt)); // @Incomplete: Math operators!
        s_rocket.pos.x = nk_clamp(s_rocket.pos.x, 0.0f, GetScreenWidth());
        s_rocket.pos.y = nk_clamp(s_rocket.pos.y, 0.0f, GetScreenHeight());

        // @Incomplete: Lerp for vectors!
        s_rocket.vel.x = nk_lerp(s_rocket.vel.x, 0.0f, 0.1f);
        s_rocket.vel.y = nk_lerp(s_rocket.vel.y, 0.0f, 0.1f);

        if(s_rocket.timer >= 0.05f)
        {
            SmokeType smokeType = (s_rocket.costume == Costume_Meat) ? SmokeType_Blood : SmokeType_Thruster;
            s32 smokeCount = 1;
            if(s_rocket.costume == Costume_Meat) smokeCount = 2;
            if(s_rocket.costume == Costume_Rainbow) smokeCount = 2;
            SpawnSmoke(smokeType, s_rocket.pos.x+RandomF32(-3.0f,3.0f), s_rocket.pos.y+20.0f, NK_CAST(s32,smokeCount));
            s_rocket.timer -= 0.05f;
        }

        // Handle collision checks.
        if(s_gameState == GameState_Game)
        {
            for(auto& asteroid: s_asteroids)
            {
                if(CheckCollision(s_rocket.pos, s_rocket.collider, asteroid.pos, asteroid.collider))
                {
                    HitRocket();
                    return;
                }
            }
        }

        // Increment the score.
        if((s_gameState == GameState_Game) && !s_gameResetting)
        {
            u32 oldScore = s_rocket.score;
            s_rocket.score += 2;
            if(s_rocket.score > 999999)
                s_rocket.score = 999999;
            if(s_rocket.highscores[0] != 0 && oldScore <= s_rocket.highscores[0] && s_rocket.score > s_rocket.highscores[0])
                PlaySound("highscore");
        }
    }
}

static void RenderRocket(f32 dt)
{
    if(s_gameState == GameState_Game || s_gamePaused)
    {
        if(s_rocket.dead)
        {
            // Draw the explosion.
            f32 frame = floorf(s_rocket.timer / 0.04f);
            if(frame < 13)
            {
                imm::BeginTextureBatch("explosion");
                Rect clip = { 96*frame, 96*NK_CAST(f32, s_rocket.costume), 96, 96 };
                imm::DrawBatchedTexture(s_rocket.pos.x, s_rocket.pos.y, &clip);
                if(s_rocket.costume != Costume_Doodle)
                {
                    imm::DrawBatchedTexture(s_rocket.pos.x-20, s_rocket.pos.y-10, 0.5f,0.5f, 0.0f, imm::Flip_None, NULL, &clip);
                    imm::DrawBatchedTexture(s_rocket.pos.x+10, s_rocket.pos.y+30, 0.5f,0.5f, 0.0f, imm::Flip_None, NULL, &clip);
                }
                imm::EndTextureBatch();
            }
        }
        else
        {
            // Draw the rocket.
            Rect clip = { 48*NK_CAST(f32,s_rocket.frame), 96*NK_CAST(f32,s_rocket.costume), 48, 96 };
            f32 angle = nk_torad(s_rocket.angle + s_rocket.shake);
            imm::DrawTexture("rocket", s_rocket.pos.x, s_rocket.pos.y, 1.0f, 1.0f, angle, imm::Flip_None, NULL, &clip);

            // Draw the score.
            bool beatHighscore = ((s_rocket.score > s_rocket.highscores[0]) && (s_rocket.highscores[0] != 0));
            BitmapFont* font = (beatHighscore) ? &s_font1 : &s_font0;
            std::string scoreStr = std::to_string(s_rocket.score);
            f32 textWidth = GetTextLineWidth(*font, scoreStr);
            if(beatHighscore) scoreStr += "!";
            f32 screenWidth = GetScreenWidth();
            f32 screenHeight = GetScreenHeight();
            DrawBitmapFont(*font, roundf((screenWidth-textWidth)*0.5f),4.0f, scoreStr);
        }
    }
}

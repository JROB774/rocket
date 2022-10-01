static void ResetGame(GameState target)
{
    s_resetTarget = target;
    s_gameResetting = true;
    s_fadeOut = true;
    s_fadeHeight = 0.0f;
}

static void RenderTransition(f32 dt)
{
    f32 screenW = GetScreenWidth();
    f32 screenH = GetScreenHeight();

    f32 speed = 1400.0f;

    nkVec4 color = { 0,0,0,1 };

    if(s_fadeOut)
    {
        s_fadeHeight += speed * dt;
        f32 y = screenH - s_fadeHeight;
        imm::DrawRectFilled(0,y,screenW,y+s_fadeHeight, color);
        imm::DrawTexture("transition", screenW*0.5f, y-8.0f);

        if(s_fadeHeight >= GetScreenHeight())
        {
            s_gameState = s_resetTarget;
            s_rocket.pos = { screenW*0.5f, screenH-32.0f };
            s_rocket.vel = { 0,0 };
            s_rocket.score = 0;
            s_rocket.timer = 0.0f;
            s_rocket.dead = false;
            s_entitySpawnCooldown = k_entitySpawnCooldownTime;
            s_entitySpawnTimer = 0.0f;
            s_difficultyTimer = 0.0f;
            s_difficulty = 50;
            s_asteroids.clear();
            s_smoke.clear();
            s_gamePaused = false;
            s_fadeOut = false;
            if(s_rocket.random)
            {
                // Pick a random costume.
                Costume costume = Costume_Random;
                while((!s_rocket.unlocks[costume]) || (costume == Costume_Random) || (costume == s_rocket.costume))
                    costume = CAST(Costume, RandomS32(Costume_Red,Costume_Glitch));
                s_rocket.costume = costume;
            }
            if(s_gameState == GameState_Game)
            {
                StartThruster();
            }
            else if(s_gameState == GameState_MainMenu)
            {
                GoToMainMenu();
            }
        }
    }
    else
    {
        s_fadeHeight -= speed * dt;
        imm::DrawRectFilled(0,0,screenW,s_fadeHeight, color);
        imm::DrawTexture("transition", screenW*0.5f, s_fadeHeight+8.0f, 1.0f,1.0f, 0.0f, imm::Flip_Vertical);

        if(s_fadeHeight <= 0.0f)
        {
            s_gameResetting = false;
        }
    }
}

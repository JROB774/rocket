#pragma once

static constexpr const char* k_playerName[]
{
    "Runner",
    "Spooks",
    "Cyclops",
    "Robot",
    "Mr. Tops",
    "Samson",
    "Pumpking",
    "ERR_ARRAY_OUT_OF_BOUNDS"
};

enum PlayerState
{
    PlayerState_Run,
    PlayerState_Jump,
    PlayerState_Slide,
    PlayerState_Dead,
    PlayerState_TOTAL
};

enum PlayerType
{
    PlayerType_Runner,
    PlayerType_Spooks,
    PlayerType_Cyclops,
    PlayerType_Robot,
    PlayerType_MrTops,
    PlayerType_Samson,
    PlayerType_Pumpking,
    PlayerType_Glitch,
    PlayerType_TOTAL
};

struct Player
{
    Vec2 pos,vel;
    AnimationState anim;
    PlayerState state;
    PlayerType type;
};

static Player s_player;

static void InitPlayer();
static void UpdatePlayer(f32 dt);
static void RenderPlayer(f32 dt);
static void KillPlayer();

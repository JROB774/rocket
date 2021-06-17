#pragma once

static GameState s_resetTarget = GameState_Game;
static f32 s_fadeHeight = 0.0f;
static bool s_fadeOut = false;

static void ResetGame(GameState target);
static void RenderTransition(f32 dt);

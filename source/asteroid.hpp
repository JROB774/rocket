#pragma once

enum AsteroidType
{
    AsteroidType_Large,
    AsteroidType_Medium,
    AsteroidType_Small,
    AsteroidType_TOTAL
};

struct Asteroid
{
    Vec2 pos;
    bool dead;
    imm::Flip flip;
    Collider collider;
    AsteroidType type;
};

static constexpr f32 k_asteroidMinSpinSpeed = 240.0f;
static constexpr f32 k_asteroidMaxSpinSpeed = 420.0f;
static constexpr f32 k_asteroidFallSpeed = 400.0f;

static constexpr f32 k_entitySpawnCooldownTime = 2.0f;
static constexpr f32 k_difficultyIncreaseInterval = 5.0f;
static constexpr s32 k_maxDifficulty = 75;

static std::vector<Asteroid> s_asteroids;

static f32 s_entitySpawnCooldown;
static f32 s_entitySpawnTimer;
static f32 s_difficultyTimer;
static s32 s_difficulty;

static void SpawnAsteroid();
static void UpdateAsteroids(f32 dt);
static void RenderAsteroids(f32 dt);
static void MaybeSpawnEntity(f32 dt);

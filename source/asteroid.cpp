static void SpawnAsteroid()
{
    Asteroid asteroid = {};
    asteroid.pos = { RandomF32(0, GetScreenWidth()), -48.0f };
    asteroid.dead = false;
    asteroid.flip = (RandomS32() % 2 == 0) ? imm::Flip_None : imm::Flip_Horizontal;
    asteroid.type = NK_CAST(AsteroidType, RandomS32(0,AsteroidType_TOTAL-1));
    asteroid.collider.offset = { 0,-2 };
    switch(asteroid.type)
    {
        case AsteroidType_Large: asteroid.collider.radius = 12.0f; break;
        case AsteroidType_Medium: asteroid.collider.radius = 8.0f; break;
        case AsteroidType_Small: asteroid.collider.radius = 4.0f; break;
        default:
        {
            // Nothing...
        } break;
    }
    s_asteroids.push_back(asteroid);
}

static void UpdateAsteroids(f32 dt)
{
    for(auto& asteroid: s_asteroids)
        asteroid.pos.y += k_asteroidFallSpeed * dt;

    s_asteroids.erase(std::remove_if(s_asteroids.begin(), s_asteroids.end(),
    [](const Asteroid& asteroid)
    {
        return (asteroid.dead || (asteroid.pos.y >= (GetScreenHeight()+48.0f)));
    }),
    s_asteroids.end());
}

static void RenderAsteroids(f32 dt)
{
    imm::BeginTextureBatch("asteroid");
    for(auto& asteroid: s_asteroids)
    {
        Rect clip = { NK_CAST(f32, 48*asteroid.type), 0, 48, 48 };
        imm::DrawBatchedTexture(asteroid.pos.x, asteroid.pos.y, 1.0f, 1.0f, 0.0f, asteroid.flip, NULL, &clip);
    }
    imm::EndTextureBatch();
}

static void MaybeSpawnEntity(f32 dt)
{
    if(s_entitySpawnCooldown > 0.0f)
        s_entitySpawnCooldown -= dt;
    else
    {
        // Slowly increase the difficulty as the game goes on.
        if(s_difficulty <= k_maxDifficulty)
        {
            s_difficultyTimer += dt;
            if(s_difficultyTimer >= k_difficultyIncreaseInterval)
            {
                s_difficultyTimer -= k_difficultyIncreaseInterval;
                s_difficulty++;
            }
        }

        s_entitySpawnTimer += dt;
        if(s_entitySpawnTimer >= 0.017f)
        {
            s_entitySpawnTimer -= 0.017f;
            if(RandomS32(0,1000) <= s_difficulty)
                SpawnAsteroid();
        }
    }
}

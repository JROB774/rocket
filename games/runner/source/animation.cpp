static std::vector<std::string> TokenizeString(std::string str, char delim)
{
    std::stringstream test(str);
    std::string segment;
    std::vector<std::string> seglist;
    while(std::getline(test, segment, delim)) seglist.push_back(segment);
    return seglist;
}

static bool LoadAnimation(Animation& anim, std::string fileName)
{
    s32 frameCount = 0;
    s32 loopCount = 0; // Unused!
    s32 direction = 0; // Unused!
    f32 startX = 0.0f;
    f32 startY = 0.0f;
    f32 frameWidth = 0.0f;
    f32 frameHeight = 0.0f;
    std::string rawDurations;

    std::istringstream stream(ReadEntireFile(fileName));
    if(stream.str().empty()) return false;
    stream >> frameCount >> loopCount >> direction >> startX >> startY >> frameWidth >> frameHeight >> rawDurations;

    auto durations = TokenizeString(rawDurations, ':');
    for(s32 i=0; i<frameCount; ++i)
    {
        Rect clip = { startX+CS_CAST(f32,i*frameWidth), startY, frameWidth, frameHeight };
        anim.frames.push_back({ std::stof(durations[i]), clip });
    }
    anim.looped = (loopCount < 0);

    return true;
}

static void FreeAnimation(Animation& anim)
{
    anim.frames.clear();
}

static void UpdateAnimation(std::string name, AnimationState& state, f32 dt)
{
    Animation& anim = *GetAsset<Animation>(name);
    if(anim.looped || state.frame < anim.frames.size())
    {
        f32 time = 0.0f;
        for(size_t i=0, n=anim.frames.size(); i<n; ++i)
        {
            time += anim.frames.at(i).duration;
            if(state.timer <= time)
            {
                state.frame = CS_CAST(s32,i);
                break;
            }
        }

        state.timer += dt;

        if(anim.looped)
        {
            f32 totalTime = 0.0f;
            for(auto& frame: anim.frames)
                totalTime += frame.duration;
            if(totalTime > 0.0f)
                while(state.timer >= totalTime)
                    state.timer -= totalTime;
        }
    }
}

static void ResetAnimation(std::string name, AnimationState& state)
{
    state.timer = 0.0f;
    state.frame = 0;
}

static bool IsAnimationDone(std::string name, AnimationState& state)
{
    Animation& anim = *GetAsset<Animation>(name);
    if(anim.looped) return false;
    f32 totalTime = 0.0f;
    for(auto& frame: anim.frames) totalTime += frame.duration;
    return (state.timer >= totalTime);
}

static Rect GetAnimationFrame(std::string name, AnimationState& state)
{
    Animation& anim = *GetAsset<Animation>(name);
    return anim.frames.at(state.frame).clip;
}

static f32 GetAnimationFrameWidth(std::string name, AnimationState& state)
{
    return GetAnimationFrame(name, state).w;
}

static f32 GetAnimatioNFrameHeight(std::string name, AnimationState& state)
{
    return GetAnimationFrame(name, state).h;
}

// Asset Boilerplate

bool Asset<Animation>::Load(std::string fileName)
{
    return LoadAnimation(m_data, fileName);
}
void Asset<Animation>::Free()
{
    FreeAnimation(m_data);
}
void Asset<Animation>::DoDebugView()
{
    ImGui::Text("Looped: %s", (m_data.looped) ? "True" : "False");
    ImGui::Text("Frames:");
    for(size_t i=0,n=m_data.frames.size(); i<n; ++i)
    {
        const auto& f = m_data.frames[i];
        ImGui::Text("%2d) %.2f : %.2f %.2f %.2f %.2f", i, f.duration, f.clip.x,f.clip.y,f.clip.w,f.clip.h);
    }
}
const char* Asset<Animation>::GetPath() const
{
    return "anims/";
}
const char* Asset<Animation>::GetExt() const
{
    return ".anim";
}
const char* Asset<Animation>::GetType() const
{
    return "Animation";
}

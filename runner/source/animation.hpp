#pragma once

struct AnimationFrame
{
    f32 duration;
    Rect clip;
};

struct AnimationState
{
    f32 timer;
    s32 frame;
};

struct Animation
{
    std::vector<AnimationFrame> frames;
    bool looped;
};

static bool LoadAnimation(Animation& anim, std::string fileName);
static void FreeAnimation(Animation& anim);
static void UpdateAnimation(std::string name, AnimationState& state, f32 dt);
static void ResetAnimation(std::string name, AnimationState& state);
static bool IsAnimationDone(std::string name, AnimationState& state);
static Rect GetAnimationFrame(std::string name, AnimationState& state);
static f32  GetAnimationFrameWidth(std::string name, AnimationState& state);
static f32  GetAnimatioNFrameHeight(std::string name, AnimationState& state);

CS_DECLARE_ASSET(Animation)
{
public:
    bool  Load(std::string fileName) override;
    void  Free() override;
    void  DoDebugView() override;
    const char* GetPath() const override;
    const char* GetExt() const override;
    const char* GetType() const override;

    Animation m_data;
};

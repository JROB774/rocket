#pragma once

struct Animation
{
    // @INCOMPLETE: ...
};

struct AnimationState
{
    // @INCOMPLETE: ...
};

static bool LoadAnimation(Animation& anim, std::string fileName);

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

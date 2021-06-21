static bool LoadAnimation(Animation& anim, std::string fileName)
{
    // @INCOMPLETE: ...
    return true;
}

bool Asset<Animation>::Load(std::string fileName)
{
    return LoadAnimation(m_data, fileName);
}
void Asset<Animation>::Free()
{
    // Nothing...
}
void Asset<Animation>::DoDebugView()
{
    // @INCOMPLETE: ...
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

#include "J_Audio.h"

/// J_SOUND ///////////////////////////////////////////////////////////////////

J_Sound::J_Sound ()
{
    channel = 0;
    chunk = nullptr;
}



void J_Sound::create (const std::string a_file, const int a_channel)
{
    if (chunk != nullptr) { destroy(); }

    std::string directory = "Resources\\Sounds\\" + a_file + ".wav";
    chunk = Mix_LoadWAV(directory.c_str());
    if (chunk == nullptr) { J_Error::log("J_ERROR_SOUND_CREATE"); }

    channel = a_channel;
    if (channel > J_Mixer::getChannels()) { J_Mixer::setChannels(channel); }
}



void J_Sound::play (const int a_loops)
{
    Mix_PlayChannel(channel, chunk, a_loops);
}



void J_Sound::destroy ()
{
    if (chunk != nullptr)
    {
        channel = 0;
        Mix_FreeChunk(chunk);
        chunk = nullptr;
    }
}

J_Sound::~J_Sound ()
{
    destroy();
}

/// J_SOUND ///////////////////////////////////////////////////////////////////



/// J_MIXER ///////////////////////////////////////////////////////////////////

const std::string J_Mixer::AUDIO_FILE = "Resources\\Data\\Audio.dat";
int J_Mixer::maxChannels = 0;
float J_Mixer::soundVolume = 0.0;
float J_Mixer::volumePiece = 0.0;
bool J_Mixer::muted = false;



void J_Mixer::initialise ()
{
    std::ifstream audioFile(AUDIO_FILE, std::ifstream::in);

    if (audioFile.is_open())
    {
        std::string rawData = "\0";
        std::istringstream data;

        std::getline(audioFile, rawData);
        data.str(rawData);
        data >> maxChannels >> volumePiece;

        audioFile.close();
    }
    else { J_Error::log("J_ERROR_MIXER_FILE_READ"); }

    Mix_AllocateChannels(maxChannels);
}



void J_Mixer::handle (const SDL_Event& a_event)
{
    if (a_event.type == SDL_KEYDOWN)
    {
        switch (a_event.key.keysym.sym)
        {
            case (SDLK_m): { toggleMute(); break; }

            case (SDLK_EQUALS): { if (!muted) { setSoundVolume(soundVolume + volumePiece); } break; }
            case (SDLK_MINUS): { if (!muted) { setSoundVolume(soundVolume - volumePiece); } break; }
        }
    }
}



void J_Mixer::setChannels (const int a_channels)
{
    Mix_AllocateChannels(a_channels);
}

void J_Mixer::setSoundVolume (const float a_volume)
{
    soundVolume = a_volume;

    if (soundVolume > MIX_MAX_VOLUME) { soundVolume = MIX_MAX_VOLUME; }
    else if (soundVolume < 0.0) { soundVolume = 0.0; }

    Mix_Volume(-1, soundVolume);
}

void J_Mixer::toggleMute ()
{
    muted = !muted;

    if (muted) { Mix_Volume(-1, 0.0); }
    else { Mix_Volume(-1, soundVolume); }
}



int J_Mixer::getChannels ()
{
    return Mix_AllocateChannels(-1);
}

float J_Mixer::getSoundVolume ()
{
    return soundVolume;
}

float J_Mixer::getVolumePiece ()
{
    return volumePiece;
}

bool J_Mixer::isMuted ()
{
    return muted;
}



void J_Mixer::terminate ()
{
    maxChannels = 0;
    soundVolume = 0.0;
    volumePiece = 0.0;
    muted = false;
}

/// J_MIXER ///////////////////////////////////////////////////////////////////
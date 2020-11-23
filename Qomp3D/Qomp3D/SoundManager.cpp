#include "SoundManager.h"
#include <stdio.h>



void SoundManager::init()
{
    FMOD_RESULT result;

    result = FMOD::System_Create(&system);      // Create the main system object.
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        //exit(-1);
    }

    result = system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        //exit(-1);
    }
}


FMOD::Sound* SoundManager::loadSound(const std::string& file, FMOD_MODE mode) const
{
    FMOD::Sound* sound;
    system->createSound(file.c_str(), mode, nullptr, &sound);
    return sound;
}


FMOD::Channel* SoundManager::playSound(FMOD::Sound* sound) const
{
    FMOD::Channel* channel;
    system->playSound(sound, nullptr, false, &channel);
    return channel;
}


void SoundManager::update()
{
    system->update();
}


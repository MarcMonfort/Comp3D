#ifndef _SOUNDMANAGER_INCLUDE
#define _SOUNDMANAGER_INCLUDE

#include "fmod.hpp"
#include "fmod_errors.h"
#include <string>



class SoundManager
{
public:
	SoundManager() {}


	static SoundManager& instance()
	{
		static SoundManager S;

		return S;
	}

	void init();
	void update();
	FMOD::Sound* loadSound(const std::string& file, FMOD_MODE mode) const;
	FMOD::Channel* playSound(FMOD::Sound* sound) const;

private:
	FMOD::System* system;

};

#endif

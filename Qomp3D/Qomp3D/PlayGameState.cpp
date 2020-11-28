#include "PlayGameState.h"


void PlayGameState::init()
{
	scene.init();
	resetKeys();
}

void PlayGameState::update(int deltaTime)
{
	scene.update(deltaTime);
}

void PlayGameState::render()
{
	scene.render();
}

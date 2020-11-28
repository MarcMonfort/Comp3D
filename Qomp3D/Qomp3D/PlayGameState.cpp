#include "Game.h"
#include "PlayGameState.h"


void PlayGameState::init()
{
	scene = Scene();
	scene.init();
}

void PlayGameState::update(int deltaTime)
{
	scene.update(deltaTime);
}

void PlayGameState::render()
{
	scene.render();
}

void PlayGameState::keyPressed(int key)
{
	if (key == 27) // Escape code
		Game::instance().goBackToMenu();
	else {
		scene.keyPressed(key);
	}
}

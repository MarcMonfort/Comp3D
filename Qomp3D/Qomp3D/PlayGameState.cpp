#include "Game.h"
#include "PlayGameState.h"


void PlayGameState::init()
{
	currentLevel = 1;
	scene = Scene();
	scene.init(currentLevel);
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

void PlayGameState::nextLevel() {
	//animation->restart();
	++currentLevel;
	scene = Scene();
	scene.init(currentLevel);
}

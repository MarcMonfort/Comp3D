#include "Game.h"
#include "PlayGameState.h"

#define NUM_LEVELS 5


void PlayGameState::init()
{
	currentLevel = 1;
	scene = new Scene();
	scene->init(currentLevel);
}

void PlayGameState::update(int deltaTime)
{
	if (nextlevel)
	{
		nextLevel();
	}

	scene->update(deltaTime);

}

void PlayGameState::render()
{
	scene->render();
}

void PlayGameState::keyPressed(int key)
{
	if (key == 27) // Escape code
		Game::instance().goBackToMenu();
	if (key == 'n')
		finalBlockTaken();
	if (key == 'g' || key == 'G')
	{
		bGodMode = !bGodMode;
	}
	else {
		scene->keyPressed(key);
	}
}

void PlayGameState::finalBlockTaken() 
{
	nextlevel = true;
}

void PlayGameState::nextLevel() 
{
	//animation->restart();
	nextlevel = false;
	++currentLevel;

	if (currentLevel <= NUM_LEVELS + 1) {
		delete scene;
		scene = new Scene();
		scene->init(currentLevel);
	}
	else
		Game::instance().goBackToMenu();	
}

bool PlayGameState::getGodMode()
{
	return bGodMode;
}

void PlayGameState::setGodMode(bool b)
{
	bGodMode = b;
}

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
	

	scene->update(deltaTime);

	if (nextlevel)
	{
		nextLevel();
	}

}

void PlayGameState::render()
{
	scene->render();
}

void PlayGameState::keyPressed(int key)
{
	if (key == 27) // Escape code
	{
		//Game::instance().goBackToMenu();
		scene->setFade(true);
		scene->setEscape(true);
	}
	else if (key == 'n')
	{
		scene->setFade(true);
	}
	else if (key == 'g' || key == 'G')
	{
		bGodMode = !bGodMode;
	}
	else if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5')
	{
		scene->setFade(true);
		setLevel = true;
		numSetLevel = key - '0';
	}
	else {
		scene->keyPressed(key);
	}
}

void PlayGameState::startFade()
{
	scene->setFade(true);
}

void PlayGameState::finalBlockTaken() 
{
	nextlevel = true;
}

void PlayGameState::nextLevel() 
{
	//animation->restart();
	nextlevel = false;

	if (setLevel)
	{
		currentLevel = numSetLevel;
		setLevel = false;
	}
	else
	{
		++currentLevel;
	}

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

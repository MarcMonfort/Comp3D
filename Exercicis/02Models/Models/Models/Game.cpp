#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"


void Game::init()
{
	bPlay = true;
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	scene.init();
}

bool Game::update(int deltaTime)
{
	scene.update(deltaTime);
	
	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.render();
}

void Game::keyPressed(int key)
{
	if(key == 27) // Escape code
		bPlay = false;
	keys[key] = true;
}

void Game::keyReleased(int key)
{
	keys[key] = false;
}

void Game::specialKeyPressed(int key)
{
	specialKeys[key] = true;
}

void Game::specialKeyReleased(int key)
{
	specialKeys[key] = false;
}

void Game::mouseMove(int x, int y)
{
	mouseOffset.x = x - SCREEN_WIDTH / 2.f;
	mouseOffset.y = SCREEN_HEIGHT / 2.f - y;

	mouseOffset *= sensitivity;
	glutWarpPointer(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
}

void Game::mousePress(int button)
{
}

void Game::mouseRelease(int button)
{
}

bool Game::getKey(int key) const
{
	return keys[key];
}

bool Game::getSpecialKey(int key) const
{
	return specialKeys[key];
}

glm::vec2 Game::getMouseOffset() const
{
	return mouseOffset;
}






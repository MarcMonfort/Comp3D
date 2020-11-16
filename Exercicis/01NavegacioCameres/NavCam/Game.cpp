#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"


void Game::init()
{
	bPlay = true;
	glClearColor(0.f, 0.f, 0.f, 1.0f);

	SoundManager::instance().init();

	FMOD::Sound* sound = SoundManager::instance().createSound("sounds/eBall.mp3", FMOD_DEFAULT);
	//FMOD::Channel* channel = SoundManager::instance().playSound(sound);
	
	lastMousePosition = glm::vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	
	scene.init();
}

bool Game::update(int deltaTime)
{
	SoundManager::instance().update();
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

	scene.keyPressed(key);
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
	// ¿no se actualiza al mover, sino al apretar?
	//mousePosition = glm::vec2(x, y);

	mouseOffset.x = x - SCREEN_WIDTH / 2.f;
	mouseOffset.y = SCREEN_HEIGHT / 2.f - y; // reversed since y-coordinates range from bottom to top
	//lastMousePosition.x = x;	// mouse is always on center, not free (so we dont use lastMousePos...)
	//lastMousePosition.y = y;

	mouseOffset *= sensitivity;

	glutWarpPointer(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	//SetCursorPos(0, 0);
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






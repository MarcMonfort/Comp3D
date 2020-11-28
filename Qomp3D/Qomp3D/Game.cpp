#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"
#include "SoundManager.h"


void Game::init()
{
	bPlay = true;
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.0f);

	currentGameState = &MenuGameState::instance();
	currentGameState->init();

	SoundManager::instance().init();
	FMOD::Sound* sound = SoundManager::instance().loadSound("sounds/eBall.mp3", FMOD_DEFAULT);
	//FMOD::Channel* channel = SoundManager::instance().playSound(sound);
}

bool Game::update(int deltaTime)
{
	SoundManager::instance().update();
	currentGameState->update(deltaTime);
	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	currentGameState->render();
}

void Game::keyPressed(int key)
{
	currentGameState->keyPressed(key);
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
}

void Game::mousePress(int button)
{
}

void Game::mouseRelease(int button)
{
}

//void Game::reshape(int width, int height)
//{
//	scene.reshape(width, height);
//}

bool Game::getKey(int key) const
{
	return keys[key];
}

bool Game::getSpecialKey(int key) const
{
	return specialKeys[key];
}

void Game::startGame() {
	currentGameState = &PlayGameState::instance();
	currentGameState->init();
}

void Game::goBackToMenu() {
	currentGameState = &MenuGameState::instance();
	currentGameState->init();
}

void Game::setBplay(bool b)
{
	bPlay = b;
}

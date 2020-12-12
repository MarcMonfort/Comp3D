#include "MenuGameState.h"
#include <iostream>
#include <glm\gtc\matrix_transform.hpp>
#include "Game.h"
#include "PlayGameState.h"


void MenuGameState::init()
{
	initShaders();

	spritesheet.loadFromFile("images/menu_background.png", TEXTURE_PIXEL_FORMAT_RGBA);
	background = Sprite::createSprite(glm::ivec2(SCREEN_WIDTH, SCREEN_HEIGHT), glm::vec2(1.f, 1.f), &spritesheet, &texProgram);
	background->setPosition(glm::vec2(0, 0));

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);

	main_theme = SoundManager::instance().loadSound("sounds/main_theme.mp3", FMOD_DEFAULT);
	channel = SoundManager::instance().playSound(main_theme);
	channel->setVolume(0.3f);
}

void MenuGameState::update(int deltaTime)
{
}

void MenuGameState::render()
{
	glm::mat4 modelMatrix, viewMatrix;
	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelMatrix = glm::mat4(1.0f);
	viewMatrix = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("model", modelMatrix);
	texProgram.setUniformMatrix4f("view", viewMatrix);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

	background->render();
}

void MenuGameState::keyPressed(int key)
{
	if (key == 27) // Escape code
	{
		Game::instance().setBplay(false);
	}
	else if (key == 32) // Space code
	{
		channel->setVolume(0.1f);
		Game::instance().startGame();
	}
}

void MenuGameState::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if (!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if (!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if (!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}

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

	// Init Fade
	totalFadeTime = 1000;
	fadeTime = 0;
	fadeIn = true;
	fadeOut = false;

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);

	if (main_theme == NULL)
		main_theme = SoundManager::instance().loadSound("sounds/main_theme.mp3", FMOD_LOOP_NORMAL);

	channel = SoundManager::instance().playSound(main_theme);
	channel->setVolume(0.8f);
}

void MenuGameState::update(int deltaTime)
{
	if (fadeIn || fadeOut)
		fadeTime += deltaTime;
}

void MenuGameState::render()
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	texProgram.setUniformMatrix4f("model", modelMatrix);
	texProgram.setUniformMatrix4f("view", viewMatrix);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

	if (fadeIn)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		float alpha = min(1.0f, fadeTime / totalFadeTime);
		texProgram.setUniform1f("alpha", alpha);
		background->render();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDisable(GL_BLEND);

		channel->setVolume(alpha);

		if (fadeTime >= totalFadeTime) {
			fadeIn = false;
			fadeTime = 0;
			channel->setVolume(1.0f);
		}
	}
	else if (fadeOut)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		float alpha = min(1.0f, fadeTime / totalFadeTime);
		texProgram.setUniform1f("alpha", 1 - alpha);
		background->render();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDisable(GL_BLEND);

		channel->setVolume(1 - alpha);


		if (fadeTime >= totalFadeTime) {
			channel->stop();
			Game::instance().startGame();
		}
	}
	else {
		background->render();
	}
}

void MenuGameState::keyPressed(int key)
{
	if (key == 27) // Escape code
	{
		Game::instance().setBplay(false);
	}
	else if (key == 32) // Space code
	{
		channel->setVolume(0.f);
		fadeIn = false;
		fadeOut = true;
		fadeTime = 0;
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

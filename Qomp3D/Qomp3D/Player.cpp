#include <iostream>
#include "Player.h"
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>

#define PI 3.14159f


Player::Player()
{
	model = NULL;
	particles = NULL;
	particles_dead = NULL;
	channel = NULL;
	line_channel = NULL;

}

Player::~Player()
{
	if (model != NULL)
		delete model;
	if (particles != NULL)
		delete particles;
	if (particles_dead != NULL)
		delete particles_dead;
	if (channel != NULL)
		channel->stop();
	if (line_channel != NULL)
		line_channel->stop();
}


void Player::init(ShaderProgram& shaderProgram, TileMap* tileMap)
{
	// Init Model and Particles
	map = tileMap;
	int style = map->getStyle();
	model = new AssimpModel();
	particles = new ParticleSystem();
	particles_dead = new ParticleSystem();

	switch (style)
	{
	case 0:
		model->loadFromFile("models/cube10.obj", shaderProgram);
		particles->init(glm::vec2(0.4f, 0.4f), shaderProgram, "images/original_particle.png", 0.f, 0.5f);
		particles_dead->init(glm::vec2(0.5f, 0.5f), shaderProgram, "images/original_particle.png", 0.f, 0.5f);
		break;
	case 1:
		model->loadFromFile("models/water_player.obj", shaderProgram);
		particles->init(glm::vec2(0.4f, 0.4f), shaderProgram, "images/water_particle.png", 0.f, 0.5f);
		particles_dead->init(glm::vec2(0.5f, 0.5f), shaderProgram, "images/water_particle.png", 0.f, 0.5f);
		break;
	case 2:
		model->loadFromFile("models/box.obj", shaderProgram);
		particles->init(glm::vec2(0.4f, 0.4f), shaderProgram, "images/box_particle.png", 0.f, 0.5f);
		particles_dead->init(glm::vec2(0.5f, 0.5f), shaderProgram, "images/box_particle.png", 0.f, 0.5f);
		break;
	case 3:
		model->loadFromFile("models/mario_player_2.obj", shaderProgram);
		particles->init(glm::vec2(0.4f, 0.4f), shaderProgram, "images/mario_particle.png", 0.f, 0.5f);
		particles_dead->init(glm::vec2(0.5f, 0.5f), shaderProgram, "images/mario_particle.png", 0.f, 0.5f);
		break;
	case 4:
		model->loadFromFile("models/minecraft_player.obj", shaderProgram);
		particles->init(glm::vec2(0.4f, 0.4f), shaderProgram, "images/minecraft_particle.png", 0.f, 0.5f);
		particles_dead->init(glm::vec2(0.5f, 0.5f), shaderProgram, "images/minecraft_particle.png", 0.f, 0.5f);
		break;
	}
	size = model->getSize();


	// Init Player
	velocity.x = 0.01f;
	velocity.y = 0.01f;

	// Init Sound
	wall_sound = SoundManager::instance().loadSound("sounds/wall3.mp3", FMOD_DEFAULT);
	player_sound = SoundManager::instance().loadSound("sounds/player2.mp3", FMOD_DEFAULT);
	button_sound = SoundManager::instance().loadSound("sounds/button.mp3", FMOD_DEFAULT);
	line_sound = SoundManager::instance().loadSound("sounds/line.mp3", FMOD_LOOP_NORMAL);
	death_sound = SoundManager::instance().loadSound("sounds/death.mp3", FMOD_DEFAULT);
	basic_sound = SoundManager::instance().loadSound("sounds/basic2.mp3", FMOD_DEFAULT);

	currentTime = 0.0f;
}

void Player::update(int deltaTime, vector<Wall*>* walls, vector<BallSpike*>* ballSpike, vector<Button*>* buttons, vector<Switch*>* switchs)
{
	// Update Particles
	//int nParticlesToSpawn = 20 * (int((currentTime + deltaTime) / 100.f) - int(currentTime / 100.f));
	currentTime += deltaTime;

	if (bDead)
	{
		particles_dead->update(deltaTime / 1000.f);
	}

	else // NOT DEAD
	{

		if (bSpace)
		{
			int nParticlesToSpawn = 3;
			ParticleSystem::Particle particle;
			float angle;

			particle.lifetime = 0.5f;

			int dirX = (velocity.x > 0) - (velocity.x < 0);
			int dirY = (velocity.y > 0) - (velocity.y < 0);

			glm::vec3 direction = glm::vec3(-(posPlayer.x + 0.5) - dirX, (posPlayer.y + 0.5) + dirY, 0.f);

			for (int i = 0; i < nParticlesToSpawn; i++)
			{
				angle = 2.f * PI * (i + float(rand()) / RAND_MAX) / nParticlesToSpawn;
				//angle = glm::radians(angle);
				particle.position = glm::vec3(cos(angle) * 0.25 + (posPlayer.x + 0.5), sin(angle) * 0.25 - (posPlayer.y + 0.5), 0.f);

				particle.speed = 5.f * glm::vec3(glm::normalize(particle.position + direction));

				particles->addParticle(particle);
			}
			bSpace = false;
		}
		particles->update(deltaTime / 1000.f);


		// End Update Particles

		if (timeRotate > 0)
			timeRotate -= deltaTime;

		if (timeScale > 0)
			timeScale -= deltaTime;



		// Update X direction
		posPlayer.x += deltaTime * velocity.x;

		if (map->collisionMoveRight(posPlayer, size, 1))
		{
			posPlayer.x -= deltaTime * velocity.x;
			velocity.x = -abs(velocity.x);

			timeScale = 200;
			eScaleDir = RIGHT;
			timeRotate = 200;
		}
		else if (map->collisionMoveLeft(posPlayer, size, 1))
		{
			posPlayer.x -= deltaTime * velocity.x;
			velocity.x = abs(velocity.x);

			timeScale = 200;
			eScaleDir = LEFT;
			timeRotate = 200;
		}

		for (int i = 0; i < (*walls).size(); ++i) {
			if (!PlayGameState::instance().getGodMode() || (*walls)[i]->getType() == 2)
				if (collideWall((*walls)[i])) {
					posPlayer.x -= deltaTime * velocity.x;
					velocity.x = -velocity.x;
					timeRotate = 200;
					timeScale = 200;
					if (velocity.x > 0)
						eScaleDir = LEFT;
					else
						eScaleDir = RIGHT;
					channel = SoundManager::instance().playSound(wall_sound);
				}
		}
		if (!PlayGameState::instance().getGodMode())
		{
			for (int i = 0; i < (*ballSpike).size(); ++i) {
				if (collideBallSpike((*ballSpike)[i])) {
					map->setPlayerDead(true);
					channel = SoundManager::instance().playSound(death_sound);
				}
			}
		}

		for (int i = 0; i < (*buttons).size(); ++i) {
			Button* button = (*buttons)[i];
			if (collideButton(button)) {
				posPlayer.x -= deltaTime * velocity.x;
				velocity.x = -velocity.x;
				if (!button->getPressed() && (button->getOrientation() == orientation::RIGHT || button->getOrientation() == orientation::LEFT)) {
					unpressAllButtons(buttons);
					button->setPressed(true);
					switchAllSwitchs(switchs);
					channel = SoundManager::instance().playSound(button_sound);
				}
			}
		}

		for (int i = 0; i < (*switchs).size(); ++i) {
			if (collideSwitch((*switchs)[i])) {
				posPlayer.x -= deltaTime * velocity.x;
				velocity.x = -velocity.x;
				channel = SoundManager::instance().playSound(basic_sound);
			}
		}

		// Update Y direction
		posPlayer.y += deltaTime * velocity.y;

		if (map->collisionMoveUp(posPlayer, size, 1))
		{
			posPlayer.y -= deltaTime * velocity.y;
			velocity.y = abs(velocity.y);

			timeScale = 200;
			eScaleDir = UP;
		}

		else if (map->collisionMoveDown(posPlayer, size, 1))
		{
			posPlayer.y -= deltaTime * velocity.y;
			velocity.y = -abs(velocity.y);

			timeScale = 200;
			eScaleDir = DOWN;
		}

		for (int i = 0; i < (*walls).size(); ++i) {
			if (!PlayGameState::instance().getGodMode() || (*walls)[i]->getType() == 2)
				if (collideWall((*walls)[i])) {
					posPlayer.y -= deltaTime * velocity.y;
					velocity.y = -velocity.y;
					timeRotate = 200;
					timeScale = 200;
					if (velocity.y > 0)
						eScaleDir = UP;
					else
						eScaleDir = DOWN;
					channel = SoundManager::instance().playSound(wall_sound);
				}
		}

		if (!PlayGameState::instance().getGodMode())
		{
			for (int i = 0; i < (*ballSpike).size(); ++i) {
				if (collideBallSpike((*ballSpike)[i])) {
					map->setPlayerDead(true);
					channel = SoundManager::instance().playSound(death_sound);
				}
			}
		}

		for (int i = 0; i < (*buttons).size(); ++i) {
			Button* button = (*buttons)[i];
			if (collideButton(button)) {
				posPlayer.y -= deltaTime * velocity.y;
				velocity.y = -velocity.y;
				if (!button->getPressed() && (button->getOrientation() == UP || button->getOrientation() == DOWN)) {
					unpressAllButtons(buttons);
					button->setPressed(true);
					switchAllSwitchs(switchs);
					channel = SoundManager::instance().playSound(button_sound);
				}
			}
		}

		for (int i = 0; i < (*switchs).size(); ++i) {
			if (collideSwitch((*switchs)[i])) {
				posPlayer.y -= deltaTime * velocity.y;
				velocity.y = -velocity.y;
				channel = SoundManager::instance().playSound(basic_sound);
			}
		}

		if (map->lineCollision(posPlayer, size, false)) {
			if (lastVelocity == 0) {
				lastVelocity = velocity.y;
				line_channel = SoundManager::instance().playSound(line_sound);
				line_channel->setVolume(1.5f);
			}
			velocity.y = 0;
		}
		else if (map->lineCollision(posPlayer, size, true)) {
			if (lastVelocity == 0) {
				lastVelocity = velocity.x;
				line_channel = SoundManager::instance().playSound(line_sound);
				line_channel->setVolume(1.5f);
			}
			velocity.x = 0;
		}
		else {
			if (velocity.y == 0) {
				velocity.y = lastVelocity;
				lastVelocity = 0;
				line_channel->setVolume(0.f);
			}
			if (velocity.x == 0) {
				velocity.x = lastVelocity;
				lastVelocity = 0;
				line_channel->setVolume(0.f);
			}
		}
	}
}

void Player::render(ShaderProgram& program, const glm::vec3& eye, float rotation)
{
	glm::mat4 modelMatrix;
	if (!bDead)
	{
		modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(posPlayer.x, -posPlayer.y, 0));

		if (rotation > 0)
		{
			modelMatrix = glm::translate(modelMatrix, model->getCenter());
			modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation), glm::vec3(0, 1, 0));
			modelMatrix = glm::translate(modelMatrix, -model->getCenter());
		}

		if (timeRotate > 0)
		{
			int dir = ((velocity.x > 0) - (velocity.x < 0)) * ((velocity.y > 0) - (velocity.y < 0));

			modelMatrix = glm::translate(modelMatrix, model->getCenter());
			modelMatrix = glm::rotate(modelMatrix, glm::radians(15.f * dir), glm::vec3(0, 0, 1));
			modelMatrix = glm::translate(modelMatrix, -model->getCenter());

		}

		if (timeScale > 0)
		{
			if (eScaleDir == DOWN)
			{
				modelMatrix = glm::translate(modelMatrix, glm::vec3(model->getCenter().x, -size.y-(timeScale*0.0005), 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(1.25, 0.8, 1));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(-model->getCenter().x, size.y,0));
			}
			else if (eScaleDir == UP)
			{
				modelMatrix = glm::translate(modelMatrix, glm::vec3(model->getCenter().x, timeScale*0.0005, 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(1.25, 0.8, 1));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(-model->getCenter().x, 0, 0));
			}
			else if (eScaleDir == LEFT)
			{
				modelMatrix = glm::translate(modelMatrix, glm::vec3(-timeScale * 0.0005, model->getCenter().y, 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.8, 1.25, 1));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -model->getCenter().y, 0));
			}
			else
			{
				modelMatrix = glm::translate(modelMatrix, glm::vec3(size.x+ timeScale * 0.0005, model->getCenter().y, 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.8, 1.25, 1));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(-size.x, -model->getCenter().y, 0));

			}
		}


		program.setUniformMatrix4f("model", modelMatrix);
		model->render(program);

		// Render particles
		if (!particles->empty())
		{
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			modelMatrix = glm::mat4(1.0f);
			program.setUniformMatrix4f("model", modelMatrix);
			particles->render(program, eye);
			program.setUniform1f("alpha", 1);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}
	}

	// Render particles dead
	else if (!particles_dead->empty())
	{
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		modelMatrix = glm::mat4(1.0f);
		program.setUniformMatrix4f("model", modelMatrix);
		particles_dead->render(program, eye);
		program.setUniform1f("alpha", 1);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
}

void Player::setPosition(const glm::vec3& position)
{
	posPlayer = position;
}

void Player::setVelocity(const glm::vec3& vel)
{
	velocity = vel;
}

glm::vec3 Player::getPosition()
{
	return posPlayer;
}

glm::vec3 Player::getSize()
{
	return size;
}

void Player::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}


void Player::keyPressed(int key)
{
	bool already_pressed = Game::instance().getKey(key);
	if (key == ' ' && !already_pressed)
	{
		if (velocity.y == 0)
			velocity.x = -velocity.x;
		else
			velocity.y = -velocity.y;
		bSpace = true;
		timeRotate = 200.f;

		channel = SoundManager::instance().playSound(player_sound);
	}
}

bool Player::collideWall(Wall* wall)
{
	glm::vec3 wallPos = wall->getPosition();
	glm::vec3 wallSize = wall->getSize();

	int distX = abs(posPlayer.x - wallPos.x);
	int distY = abs(posPlayer.y - wallPos.y);


	if (distX > wallSize.x+1 || distY > wallSize.y+1)
		return false;
	else
	{
		float Wxmin = wallPos.x;
		float Wxmax = wallPos.x + wallSize.x;
		float Wymin = wallPos.y;
		float Wymax = wallPos.y + wallSize.y;

		float Pxmin = posPlayer.x;
		float Pxmax = posPlayer.x + size.x;
		float Pymin = posPlayer.y;
		float Pymax = posPlayer.y + size.y;

		return ((Wxmin < Pxmax&& Pxmin < Wxmax) && (Wymin < Pymax&& Pymin < Wymax));
	}
}


bool Player::collideBallSpike(BallSpike* ballSpike)
{
	glm::vec3 ballSpikePos = ballSpike->getPosition();
	glm::vec3 ballSpikeSize = ballSpike->getSize();

	int distX = abs(posPlayer.x - ballSpikePos.x);
	int distY = abs(posPlayer.y - ballSpikePos.y);


	if (distX > ballSpikeSize.x + 2 || distY > ballSpikeSize.y + 2)
	{
		return false;
	}
	else
	{
		float Wxmin = (ballSpikePos.x-0.5);
		float Wxmax = (ballSpikePos.x-0.5) + ballSpikeSize.x;
		float Wymin = (ballSpikePos.y-0.5);
		float Wymax = (ballSpikePos.y-0.5) + ballSpikeSize.y;

		float Pxmin = posPlayer.x;
		float Pxmax = posPlayer.x + size.x;
		float Pymin = posPlayer.y;
		float Pymax = posPlayer.y + size.y;

		return ((Wxmin < Pxmax && Pxmin < Wxmax) && (Wymin < Pymax && Pymin < Wymax));
	}
}


bool Player::collideButton(Button* button)
{
	glm::vec3 buttonPos = button->getPosition();
	glm::vec3 buttonSize = button->getSize();

	int orientation = button->getOrientation();

	if (orientation == 1 || orientation == 3)
		buttonSize = glm::vec3(buttonSize.y, buttonSize.x, buttonSize.z);

	float Bxmin = buttonPos.x;
	if (orientation == 1)
		Bxmin += 0.5;
	float Bxmax = Bxmin + buttonSize.x;
	float Bymin = buttonPos.y;
	if (orientation == 0)
		Bymin += 0.5;
	float Bymax = Bymin + buttonSize.y;

	float Pxmin = posPlayer.x;
	float Pxmax = posPlayer.x + size.x;
	float Pymin = posPlayer.y;
	float Pymax = posPlayer.y + size.y;

	return ((Bxmin < Pxmax&& Pxmin < Bxmax) && (Bymin < Pymax&& Pymin < Bymax));
}

bool Player::collideSwitch(Switch* switx)
{
	if (switx->getActivated()) {
		glm::vec3 switchPos = switx->getPosition();
		glm::vec3 switchSize = switx->getSize();

		float Sxmin = switchPos.x;
		float Sxmax = switchPos.x + switchSize.x;
		float Symin = switchPos.y;
		float Symax = switchPos.y + switchSize.y;

		float Pxmin = posPlayer.x;
		float Pxmax = posPlayer.x + size.x;
		float Pymin = posPlayer.y;
		float Pymax = posPlayer.y + size.y;

		return ((Sxmin < Pxmax&& Pxmin < Sxmax) && (Symin < Pymax&& Pymin < Symax));
	}
	else return false;
}

void Player::switchAllSwitchs(vector<Switch*>* switchs) {
	for (int i = 0; i < (*switchs).size(); ++i) {
		(*switchs)[i]->toggle();
	}
}

void Player::unpressAllButtons(vector<Button*>* buttons) {
	for (int i = 0; i < (*buttons).size(); ++i) {
		(*buttons)[i]->setPressed(false);
	}
}

void Player::setDead(bool b)
{
	bDead = b;
	if (bDead)
	{
		int nParticlesToSpawn = 50;
		ParticleSystem::Particle particle;
		float angle;

		particle.lifetime = 1.f;

		glm::vec3 direction = glm::vec3(-(posPlayer.x + 0.5), (posPlayer.y + 0.5), 0.f);

		for (int i = 0; i < nParticlesToSpawn; i++)
		{
			angle = 2.f * PI * (i + float(rand()) / RAND_MAX) / nParticlesToSpawn;
			particle.position = glm::vec3(cos(angle) * 0.25 + (posPlayer.x + 0.5), sin(angle) * 0.25 - (posPlayer.y + 0.5), 0.f);
			particle.speed = 30.f * glm::vec3(glm::normalize(particle.position + direction));
			particles_dead->addParticle(particle);
		}
	}
}

void Player::setLineVolume(float lv) {
	if (lv == 0.f)
		line_channel->stop();
	else
		line_channel->setVolume(lv);
}

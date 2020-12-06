#include <iostream>
#include "Player.h"
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>

#define PI 3.14159f


void Player::init(ShaderProgram& shaderProgram)
{
	// Init Model
	model = new AssimpModel();
	model->loadFromFile("models/cube10.obj", shaderProgram);
	size = model->getSize();

	// Init particle system
	ParticleSystem::Particle particle;
	particle.lifetime = 1e10f;
	particles = new ParticleSystem();
	particles->init(glm::vec2(0.5f, 0.5f), shaderProgram, "images/smoke_1.png", 0.f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// Init Player
	velocity.x = 0.01f;
	velocity.y = 0.01f;

	currentTime = 0.0f;
}

void Player::update(int deltaTime, vector<Wall*>* walls, vector<BallSpike*>* ballSpike, vector<Button*>* buttons, vector<Switch*>* switchs)
{

	// Update Particles
	//int nParticlesToSpawn = 20 * (int((currentTime + deltaTime) / 100.f) - int(currentTime / 100.f));
	if (bSpace)
	{
		int nParticlesToSpawn = 20;
		ParticleSystem::Particle particle;
		float angle;

		particle.lifetime = 1.5f;

		int dirX = (velocity.x > 0) - (velocity.x < 0);
		int dirY = (velocity.y > 0) - (velocity.y < 0);

		glm::vec3 direction = glm::vec3(-(posPlayer.x + 0.5) - dirX, (posPlayer.y + 0.5) + dirY, 0.f);

		for (int i = 0; i < nParticlesToSpawn; i++)
		{
			angle = 2.f * PI * (i + float(rand()) / RAND_MAX) / nParticlesToSpawn;
			//angle = glm::radians(angle);
			particle.position = glm::vec3(cos(angle) * 0.25 + (posPlayer.x+0.5), sin(angle) * 0.25 - (posPlayer.y+0.5), 0.f);

			particle.speed = 5.f * glm::vec3(glm::normalize(particle.position + direction));

			particles->addParticle(particle);
		}



		bSpace = false;
	}
	particles->update(deltaTime / 1000.f);
	currentTime += deltaTime;
	// End Update Particles


	// Update X direction
	posPlayer.x += deltaTime * velocity.x;

	if (map->collisionMoveRight(posPlayer, model->getSize()))
	{
		posPlayer.x -= deltaTime * velocity.x;
		velocity.x = -abs(velocity.x);
	}
	else if (map->collisionMoveLeft(posPlayer, model->getSize()))
	{
		posPlayer.x -= deltaTime * velocity.x;
		velocity.x = abs(velocity.x);
	}

	for (int i = 0; i < (*walls).size(); ++i) {
		if (!PlayGameState::instance().getGodMode() || (*walls)[i]->getType() == 2)
			if (collideWall((*walls)[i])) {
				posPlayer.x -= deltaTime * velocity.x;
				velocity.x = -velocity.x;
			}
	}
	if (!PlayGameState::instance().getGodMode())
	{
		for (int i = 0; i < (*ballSpike).size(); ++i) {
			if (collideBallSpike((*ballSpike)[i])) {
				map->setPlayerDead(true);
			}
		}
	}

	for (int i = 0; i < (*buttons).size(); ++i) {
		Button* button = (*buttons)[i];
		if (collideButton(button)) {
			posPlayer.x -= deltaTime * velocity.x;
			velocity.x = -velocity.x;
			if (!button->getPressed() && (button->getOrientation() == orientation::right || button->getOrientation() == orientation::left)) {
				unpressAllButtons(buttons);
				button->setPressed(true);
				switchAllSwitchs(switchs);
			}
		}
	}

	for (int i = 0; i < (*switchs).size(); ++i) {
		if (collideSwitch((*switchs)[i])) {
			posPlayer.x -= deltaTime * velocity.x;
			velocity.x = -velocity.x;
		}
	}

	// Update Y direction
	posPlayer.y += deltaTime * velocity.y;

	if (map->collisionMoveUp(posPlayer, model->getSize()))
	{
		posPlayer.y -= deltaTime * velocity.y;
		velocity.y = abs(velocity.y);
	}

	else if (map->collisionMoveDown(posPlayer, model->getSize()))
	{
		posPlayer.y -= deltaTime * velocity.y;
		velocity.y = -abs(velocity.y);
	}

	for (int i = 0; i < (*walls).size(); ++i) {
		if (!PlayGameState::instance().getGodMode() || (*walls)[i]->getType() == 2)
			if (collideWall((*walls)[i])) {
				posPlayer.y -= deltaTime * velocity.y;
				velocity.y = -velocity.y;
			}
	}

	if (!PlayGameState::instance().getGodMode())
	{
		for (int i = 0; i < (*ballSpike).size(); ++i) {
			if (collideBallSpike((*ballSpike)[i])) {
				map->setPlayerDead(true);
			}
		}
	}

	for (int i = 0; i < (*buttons).size(); ++i) {
		Button* button = (*buttons)[i];
		if (collideButton(button)) {
			posPlayer.y -= deltaTime * velocity.y;
			velocity.y = -velocity.y;
			if (!button->getPressed() && (button->getOrientation() == up || button->getOrientation() == down)) {
				unpressAllButtons(buttons);
				button->setPressed(true);
				switchAllSwitchs(switchs);
			}
		}
	}

	for (int i = 0; i < (*switchs).size(); ++i) {
		if (collideSwitch((*switchs)[i])) {
			posPlayer.y -= deltaTime * velocity.y;
			velocity.y = -velocity.y;
		}
	}

	if (map->lineCollision(posPlayer, model->getSize(), false)) {
		if (lastVelocity == 0)
			lastVelocity = velocity.y;
		velocity.y = 0;
	}
	else if (map->lineCollision(posPlayer, model->getSize(), true)) {
		if (lastVelocity == 0)
			lastVelocity = velocity.x;
		velocity.x = 0;
	}
	else {
		if (velocity.y == 0) {
			velocity.y = lastVelocity;
			lastVelocity = 0;
		}
		if (velocity.x == 0) {
			velocity.x = lastVelocity;
			lastVelocity = 0;
		}
	}
}

void Player::render(ShaderProgram& program, const glm::vec3& eye)
{
	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(posPlayer.x, -posPlayer.y, 0));
	program.setUniformMatrix4f("model", modelMatrix);

	model->render(program);

	// Render particles
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);

	modelMatrix = glm::mat4(1.0f);
	program.setUniformMatrix4f("model", modelMatrix);
	//normalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
	//texProgram.setUniformMatrix3f("normalmatrix", normalMatrix);
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(5, 0, 10));
	//program.setUniformMatrix4f("model", modelMatrix);

	particles->render(eye);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
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
	return model->getSize();
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

	int distX = abs(posPlayer.x - ballSpikePos.x);
	int distY = abs(posPlayer.y - ballSpikePos.y);


	if (distX > map->getMovementCamera().x || distY > map->getMovementCamera().y)
	{
		return false;
	}
	else
	{
		glm::vec3 ballSpikeSize = ballSpike->getSize();
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

	glm::vec3 playerSize = getSize();

	float Bxmin = buttonPos.x;
	float Bxmax = buttonPos.x + buttonSize.x;
	float Bymin = buttonPos.y;
	float Bymax = buttonPos.y + buttonSize.y;

	float Pxmin = posPlayer.x;
	float Pxmax = posPlayer.x + playerSize.x;
	float Pymin = posPlayer.y;
	float Pymax = posPlayer.y + playerSize.y;

	return ((Bxmin < Pxmax&& Pxmin < Bxmax) && (Bymin < Pymax&& Pymin < Bymax));
}

bool Player::collideSwitch(Switch* switx)
{
	if (switx->getActivated()) {
		glm::vec3 switchPos = switx->getPosition();
		glm::vec3 switchSize = switx->getSize();

		glm::vec3 playerSize = getSize();

		float Sxmin = switchPos.x;
		float Sxmax = switchPos.x + switchSize.x;
		float Symin = switchPos.y;
		float Symax = switchPos.y + switchSize.y;

		float Pxmin = posPlayer.x;
		float Pxmax = posPlayer.x + playerSize.x;
		float Pymin = posPlayer.y;
		float Pymax = posPlayer.y + playerSize.y;

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
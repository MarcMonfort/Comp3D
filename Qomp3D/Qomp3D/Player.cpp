#include <iostream>
#include "Player.h"
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>


void Player::init(ShaderProgram& shaderProgram)
{
	model = new AssimpModel();
	model->loadFromFile("models/cube10.obj", shaderProgram);

	velocity.x = 0.01f;
	velocity.y = 0.01f;

}

void Player::update(int deltaTime, vector<Wall*>* walls, vector<Button*>* buttons, vector<Switch*>* switchs)
{
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
		if (collideWall((*walls)[i])) {
			posPlayer.x -= deltaTime * velocity.x;
			velocity.x = -velocity.x;
		}
	}

	for (int i = 0; i < (*buttons).size(); ++i) {
		Button* button = (*buttons)[i];
		if (collideButton(button)) {
			posPlayer.x -= velocity.x;
			velocity.x = -velocity.x;
		}
	}

	for (int i = 0; i < (*switchs).size(); ++i) {
		if (collideSwitch((*switchs)[i])) {
			posPlayer.x -= velocity.x;
			velocity.x = -velocity.x;
		}
	}

	
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
		if (collideWall((*walls)[i])) {
			posPlayer.y -= deltaTime * velocity.y;
			velocity.y = -velocity.y;
		}
	}

	for (int i = 0; i < (*buttons).size(); ++i) {
		Button* button = (*buttons)[i];
		if (collideButton(button)) {
			posPlayer.y -= velocity.y;
			velocity.y = -velocity.y;
			if (!button->getPressed()) {
				button->setPressed(true);
				switchAllSwitchs(switchs);
			}
		}
	}

	for (int i = 0; i < (*switchs).size(); ++i) {
		if (collideSwitch((*switchs)[i])) {
			posPlayer.y -= velocity.y;
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

void Player::render(ShaderProgram& program)
{
	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(posPlayer.x, -posPlayer.y, 0));
	program.setUniformMatrix4f("model", modelMatrix);

	model->render(program);
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
	}
}

bool Player::collideWall(Wall* wall)
{
	glm::vec3 wallPos = wall->getPosition();
	glm::vec3 wallSize = wall->getSize();
	bool bVertical = wall->getOrientation();

	glm::vec3 playerSize = getSize();

	float Wxmin = wallPos.x;
	float Wxmax = wallPos.x + wallSize.x;
	float Wymin = wallPos.y;
	float Wymax = wallPos.y + wallSize.y;
	
	float Pxmin = posPlayer.x;
	float Pxmax = posPlayer.x + playerSize.x;
	float Pymin = posPlayer.y;
	float Pymax = posPlayer.y + playerSize.y;

	return ((Wxmin < Pxmax&& Pxmin < Wxmax) && (Wymin < Pymax&& Pymin < Wymax));
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
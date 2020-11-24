#include <iostream>
#include "Player.h"
#include <glm/gtc/matrix_transform.hpp>


void Player::init(ShaderProgram& shaderProgram)
{
	model = new AssimpModel();
	model->loadFromFile("models/cube10.obj", shaderProgram);

	velocity.x = 0.01f;
	velocity.y = -0;
}

void Player::update(int deltaTime)
{
	posPlayer.x += velocity.x * deltaTime;	// * deltaTime (in the future)
	posPlayer.y += velocity.y * deltaTime;

	//if (map->collisionMoveRight(posPlayer + glm::vec3(-0.5, 1.0, 0), model->getSize()))
	//{
	//	//cout << "hola" << endl;
	//	//posPlayer.x -= velocity.x;
	//	//velocity.x -= velocity.x;
	//}
	if (map->collisionMoveRight(posPlayer, model->getSize()))
	{
		velocity.x = -velocity.x;
	}
	if (map->collisionMoveLeft(posPlayer, model->getSize()))
	{
		velocity.x = -velocity.x;
	}
}

void Player::render(ShaderProgram& program)
{
	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(posPlayer.x, -posPlayer.y, posPlayer.z));
	program.setUniformMatrix4f("model", modelMatrix);

	model->render(program);
}

void Player::setPosition(const glm::vec3& position)
{
	posPlayer = position;
}

void Player::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}



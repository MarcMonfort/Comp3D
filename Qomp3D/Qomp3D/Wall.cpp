#include "Wall.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


void Wall::init(ShaderProgram& shaderProgram, bool bVertical)
{
	this->bVertical = bVertical;	//vertical or horizontal
	model = new AssimpModel();
	if (bVertical)
		model->loadFromFile("models/cube40_v.obj", shaderProgram);	// vertical
	else
		model->loadFromFile("models/cube40_h.obj", shaderProgram);  //horizontal

	size = model->getSize();
	velocity = 0.1;

}

void Wall::update(int deltaTime, Player* player)
{
	glm::vec3 posPlayer = player->getPosition();
	followPlayer(posPlayer);

	//should collide also with player, to avoid problems???
	if (bVertical)
	{
		glm::vec3 aux_size = glm::vec3(size.x - 0.0001, size.y, size.z);
		if (map->collisionMoveUp(position, aux_size))
		{
			velocity = abs(velocity);
		}
		else if (map->collisionMoveDown(position, aux_size))
		{
			velocity = -abs(velocity);
		}
		position.y += velocity;
	}
	else   //horizontal
	{
		glm::vec3 aux_size = glm::vec3(size.x, size.y - 0.0001, size.z);
		if (map->collisionMoveRight(position, aux_size))
		{
			velocity = -abs(velocity);
		}
		else if (map->collisionMoveLeft(position, aux_size))
		{
			velocity = abs(velocity);
		}
		position.x += velocity;
	}
}

void Wall::render(ShaderProgram& program)
{
	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, -position.y, 0));
	program.setUniformMatrix4f("model", modelMatrix);

	model->render(program);
}

void Wall::setPosition(const glm::vec3& pos)
{
	if (bVertical)
	{
		position = glm::vec3(pos.x, pos.y - (size.y/2) + 0.5, 0);
	}
	else
	{
		position = glm::vec3(pos.x - (size.x/2) + 0.5, pos.y, 0);
	}
}

void Wall::setVelocity(float vel)
{
	velocity = vel;
}

glm::vec3 Wall::getPosition()
{
	return position;
}

glm::vec3 Wall::getSize()
{
	return size;
}

void Wall::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}


void Wall::keyPressed(int key)
{

	if (key == ' ')
	{
		velocity = -velocity;
	}
}

void Wall::followPlayer(glm::vec3 posPlayer)
{
	if (bVertical)
	{
		if (posPlayer.y < position.y)
			velocity = -abs(velocity);
		else if (posPlayer.y > position.y + size.y)
			velocity = abs(velocity);
	}
	else
	{
		if (posPlayer.x < position.x)
			velocity = -abs(velocity);
		else if (posPlayer.x > position.x + size.x)
			velocity = abs(velocity);
	}
}

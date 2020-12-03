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
	velocity = 0.01;

}

void Wall::update(int deltaTime, const glm::vec3& posPlayer, const glm::vec3& sizePlayer)
{
	int distX = abs(posPlayer.x - position.x);
	int distY = abs(posPlayer.y - position.y);
	if (distX < map->getMovementCamera().x && distY < map->getMovementCamera().y)
	{
		followPlayer(posPlayer, sizePlayer);

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
			position.y += deltaTime * velocity;
			if (collidePlayer(posPlayer, sizePlayer)) {
				position.y -= deltaTime * velocity;
			}
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
			position.x += deltaTime * velocity;
			if (collidePlayer(posPlayer, sizePlayer)) {
				position.x -= deltaTime * velocity;
			}
		}
	}
}

void Wall::render(ShaderProgram& program, const glm::vec3& posPlayer)
{
	int distX = abs(posPlayer.x - position.x);
	int distY = abs(posPlayer.y - position.y);
	if (distX < map->getMovementCamera().x+2 && distY < map->getMovementCamera().y+2)
	{
		glm::mat4 modelMatrix;
		modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, -position.y, 0));
		program.setUniformMatrix4f("model", modelMatrix);

		model->render(program);
	}
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

void Wall::followPlayer(const glm::vec3& posPlayer, const glm::vec3& sizePlayer)
{

	float offset = 0.5;
	if (bVertical)
	{
		if (posPlayer.y + 1 + offset < position.y)
			velocity = -abs(velocity);
		else if (posPlayer.y - offset > position.y + size.y)
			velocity = abs(velocity);
	}
	else
	{
		if (posPlayer.x + 1 + offset < position.x)
			velocity = -abs(velocity);
		else if (posPlayer.x - offset > position.x + size.x)
			velocity = abs(velocity);
	}
}

bool Wall::getOrientation()
{
	return bVertical;
}

bool Wall::collidePlayer(const glm::vec3& posPlayer, const glm::vec3& sizePlayer)
{
	float Wxmin = position.x;
	float Wxmax = position.x + size.x;
	float Wymin = position.y;
	float Wymax = position.y + size.y;

	float Pxmin = posPlayer.x;
	float Pxmax = posPlayer.x + sizePlayer.x;
	float Pymin = posPlayer.y;
	float Pymax = posPlayer.y + sizePlayer.y;

	return ((Wxmin < Pxmax && Pxmin < Wxmax) && (Wymin < Pymax && Pymin < Wymax));
}

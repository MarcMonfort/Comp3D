#include "BallSpike.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>



BallSpike::BallSpike()
{
	model = NULL;
}

BallSpike::~BallSpike()
{
	if (model != NULL)
		delete model;
}


void BallSpike::init(ShaderProgram& shaderProgram, bool bVertical, TileMap* tileMap)
{
	map = tileMap;

	this->bVertical = bVertical;	//vertical or horizontal

	model = new AssimpModel();

	int style = map->getStyle();

	switch (style)
	{
	case 0:
		model->loadFromFile("models/ballSpike.obj", shaderProgram);
		break;
	case 1:
		model->loadFromFile("models/water_ballSpike.obj", shaderProgram);
		break;
	case 2:
		model->loadFromFile("models/box_ballSpike.obj", shaderProgram);
		break;
	case 3:
		model->loadFromFile("models/mario_ballSpike.obj", shaderProgram);
		break;
	case 4:
		model->loadFromFile("models/minecraft_ballSpike.obj", shaderProgram);
		break;
	}

	size = model->getSize();
	velocity = 0.005;

	currentTime = 0;
	stopTime = 0;

	if (bVertical)
		axis = glm::vec3(1, 0, 0);
	else
		axis = glm::vec3(0, 1, 0);

}

void BallSpike::update(int deltaTime, const glm::vec3& posPlayer)
{
	currentTime += deltaTime;
	int distX = abs(posPlayer.x - position.x);
	int distY = abs(posPlayer.y - position.y);

	if (distX > map->getMovementCamera().x + 2 || distY > map->getMovementCamera().y + 2)
	{
		state = State::OUT;
	}
	else if (stopTime >= 0)
	{
		stopTime -= deltaTime;
		state = State::STATIC;
	}
	else
	{
		state = State::MOVE;
	}


	if (state == State::MOVE)
	{

		if (bVertical)
		{
			glm::vec3 aux_size = glm::vec3(1);
			if (map->collisionMoveUp(position, aux_size))
			{
				stopTime = 2000;
				position.y += ceil(position.y) - position.y;
				velocity = abs(velocity);
			}
			else if (map->collisionMoveDown(position, aux_size))
			{
				stopTime = 2000;
				position.y -= (position.y + size.y) - floor(position.y + size.y);
				velocity = -abs(velocity);
			}
			position.y += deltaTime * velocity;

		}
		else   //horizontal
		{
			glm::vec3 aux_size = glm::vec3(1);
			if (map->collisionMoveRight(position, aux_size))
			{
				stopTime = 2000;
				position.x -= (position.x + size.x) - floor(position.x + size.x);
				velocity = -abs(velocity);
			}
			else if (map->collisionMoveLeft(position, aux_size))
			{
				stopTime = 2000;
				position.x += ceil(position.x) - position.x;
				velocity = abs(velocity);
			}
			position.x += deltaTime * velocity;
		}
	}
}

void BallSpike::render(ShaderProgram& program, const glm::vec3& posPlayer, glm::mat4& viewMatrix)
{

	if (state != State::OUT)
	{
		glm::mat4 modelMatrix;
		glm::mat3 normalMatrix;
		modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, -position.y, 0));

		if (state == State::MOVE)
		{
			modelMatrix = glm::translate(modelMatrix, glm::vec3(model->getCenter()));
			modelMatrix = glm::rotate(modelMatrix, (currentTime * 0.01f), axis);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-model->getCenter()));
			normalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
			program.setUniformMatrix3f("normalmatrix", normalMatrix);
		}
		program.setUniformMatrix4f("model", modelMatrix);

		model->render(program);
	}
}

void BallSpike::setPosition(const glm::vec3& pos)
{
	position = pos;
}

void BallSpike::setVelocity(float vel)
{
	velocity = vel;
}

glm::vec3 BallSpike::getPosition()
{
	return position;
}

glm::vec3 BallSpike::getSize()
{
	return size;
}

void BallSpike::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}


bool BallSpike::getOrientation()
{
	return bVertical;
}



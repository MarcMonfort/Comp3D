#include "BallSpike.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


void BallSpike::init(ShaderProgram& shaderProgram, bool bVertical)
{
	this->bVertical = bVertical;	//vertical or horizontal

	model = new AssimpModel();
	model->loadFromFile("models/test.obj", shaderProgram);

	size = model->getSize();
	velocity = 0.005;

	currentTime = 0;

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
	else
	{
		state = State::STATIC;
	}


	if (state != State::OUT)
	{

		if (bVertical)
		{
			glm::vec3 aux_size = glm::vec3(size.x - 0.0001, size.y, size.z);
			if (map->collisionMoveUp(position, aux_size))
			{
				position.y += ceil(position.y) - position.y;
				velocity = abs(velocity);
			}
			else if (map->collisionMoveDown(position, aux_size))
			{
				position.y -= (position.y + size.y) - floor(position.y + size.y);
				velocity = -abs(velocity);
			}
			position.y += deltaTime * velocity;

		}
		else   //horizontal
		{
			glm::vec3 aux_size = glm::vec3(size.x, size.y - 0.0001, size.z);
			if (map->collisionMoveRight(position, aux_size))
			{
				position.x -= (position.x + size.x) - floor(position.x + size.x);
				velocity = -abs(velocity);
			}
			else if (map->collisionMoveLeft(position, aux_size))
			{
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
		modelMatrix = glm::translate(modelMatrix, glm::vec3(model->getCenter()));
		modelMatrix = glm::rotate(modelMatrix, currentTime*0.01f, glm::vec3(0,1,0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-model->getCenter()));
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
		program.setUniformMatrix3f("normalmatrix", normalMatrix);
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



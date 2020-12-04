#include "Wall.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


void Wall::init(ShaderProgram& shaderProgram, bool bVertical, Type type)
{
	this->bVertical = bVertical;	//vertical or horizontal
	model = new AssimpModel();
	if (bVertical)
		model->loadFromFile("models/cube40_v.obj", shaderProgram);	// vertical
	else
		model->loadFromFile("models/cube40_h.obj", shaderProgram);  //horizontal

	size = model->getSize();
	velocity = 0.005;

	this->type = static_cast<int>(type);

	switch (type)
	{
	case Type::EASY:
		followDist = 4;
		break;
	case Type::HARD:
		followDist = 6;
		break;
	case Type::IMPOSSIBLE:
		followDist = 8;
		break;
	default:
		break;
	}

}

void Wall::update(int deltaTime, const glm::vec3& posPlayer, const glm::vec3& sizePlayer)
{
	glm::vec2 centerPlayer = glm::vec2(posPlayer.x + sizePlayer.x / 2, posPlayer.y + sizePlayer.y / 2);
	glm::vec2 centerWall = glm::vec2(position.x + size.x / 2, position.y + size.y / 2);

	int distX = abs(centerPlayer.x - centerWall.x);
	int distY = abs(centerPlayer.y - centerWall.y);

	int distPlayer = (bVertical) ? distX : distY;

	if (distX > map->getMovementCamera().x + 2 || distY > map->getMovementCamera().y + 2)
	{
		state = State::OUT;
	}
	else if (distPlayer > followDist)
	{
		switch (state)
		{
		case State::FOLLOW:
			velocity *= 0.5;
			break;
		case State::OUT:
			velocity = 0.005;
			break;
		}
		state = State::STATIC;
	}
	else
	{
		switch (state)
		{
		case State::STATIC:
			velocity *= 2;
			break;
		case State::OUT:
			velocity = 0.01;
			break;
		}
		state = State::FOLLOW;
	}


	
	if (state != State::OUT)
	{
		followPlayer(centerPlayer);

		if (bVertical)
		{
			glm::vec3 aux_size = glm::vec3(size.x - 0.0001, size.y, size.z);
			if (map->collisionMoveUp(position, aux_size))
			{
				position.y += ceil(position.y)-position.y ;
				if (state == State::STATIC)
					velocity = abs(velocity);
			}
			else if (map->collisionMoveDown(position, aux_size))
			{
				position.y -= (position.y + size.y) - floor(position.y + size.y);
				if (state == State::STATIC)
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
				position.x -= (position.x + size.x) - floor(position.x + size.x);
				if (state == State::STATIC)
					velocity = -abs(velocity);
			}
			else if (map->collisionMoveLeft(position, aux_size))
			{
				position.x += ceil(position.x) - position.x;
				if (state == State::STATIC)
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

	if (state != State::OUT)
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

void Wall::followPlayer(const glm::vec2& centerPlayer)
{

	glm::vec2 centerWall = glm::vec2(position.x + size.x / 2, position.y + size.y / 2);

	if (state == State::FOLLOW)
	{

		if (bVertical)
		{
			if (centerPlayer.y < centerWall.y)
				velocity = -abs(velocity);
			else
				velocity = abs(velocity);
		}
		else
		{
			if (centerPlayer.x < centerWall.x)
				velocity = -abs(velocity);
			else
				velocity = abs(velocity);
		}
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


int Wall::getType()
{
	return type;
}
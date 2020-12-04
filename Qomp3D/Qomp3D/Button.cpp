#include "Button.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


void Button::init(ShaderProgram& shaderProgram, bool press)
{
	model_pressed = new AssimpModel();
	model_pressed->loadFromFile("models/button_up_pressed.obj", shaderProgram);
	size = model_pressed->getSize();

	model_not_pressed = new AssimpModel();
	model_not_pressed->loadFromFile("models/button_up.obj", shaderProgram);

	pressed = press;
}

void Button::update(int deltaTime)
{
}

void Button::render(ShaderProgram& program)
{
	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, -position.y, 0));
	program.setUniformMatrix4f("model", modelMatrix);

	if (pressed)
		model_pressed->render(program);
	else
		model_not_pressed->render(program);
}

void Button::setPosition(const glm::vec3& pos)
{
	position = pos;
}

glm::vec3 Button::getPosition()
{
	return position;
}

glm::vec3 Button::getSize()
{
	return size;
}

void Button::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Button::setPressed(bool press)
{
	pressed = press;
}

bool Button::getPressed()
{
	return pressed;
}

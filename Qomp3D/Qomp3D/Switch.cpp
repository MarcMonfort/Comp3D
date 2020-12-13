#include "Switch.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


void Switch::init(ShaderProgram& shaderProgram, bool act)
{
	activated = act;

	model_yes = new AssimpModel();
	model_yes->loadFromFile("models/switch_yes4.obj", shaderProgram);
	size = model_yes->getSize();

	model_no = new AssimpModel();
	model_no->loadFromFile("models/switch_no.obj", shaderProgram);
}

void Switch::update(int deltaTime)
{
}

void Switch::render(ShaderProgram& program)
{
	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, -position.y, 0));
	program.setUniformMatrix4f("model", modelMatrix);

	if (activated)
		model_yes->render(program);
	else
		model_no->render(program);
}

void Switch::setPosition(const glm::vec3& pos)
{
	position = pos;
}

glm::vec3 Switch::getPosition()
{
	return position;
}

glm::vec3 Switch::getSize()
{
	return size;
}

void Switch::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Switch::toggle() {
	activated = !activated;
}

bool Switch::getActivated()
{
	return activated;
}

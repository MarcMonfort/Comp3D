#include "Switch.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


void Switch::init(ShaderProgram& shaderProgram, bool act, TileMap* tileMap)
{
	map = tileMap;
	activated = act;


	int style = map->getStyle();
	model_yes = new AssimpModel();
	model_no = new AssimpModel();

	switch (style)
	{
	case 0:
		model_yes->loadFromFile("models/switch_yes.obj", shaderProgram);
		model_no->loadFromFile("models/switch_no.obj", shaderProgram);
		break;
	case 1:
		model_yes->loadFromFile("models/water_switch_yes.obj", shaderProgram);
		model_no->loadFromFile("models/water_switch_no.obj", shaderProgram);
		break;
	case 2:
		model_yes->loadFromFile("models/box_switch_yes.obj", shaderProgram);
		model_no->loadFromFile("models/box_switch_no.obj", shaderProgram);
		break;
	case 3:
		model_yes->loadFromFile("models/mario_switch_yes.obj", shaderProgram);
		model_no->loadFromFile("models/mario_switch_no.obj", shaderProgram);
		break;
	case 4:
		model_yes->loadFromFile("models/minecraft_switch_yes.obj", shaderProgram);
		model_no->loadFromFile("models/minecraft_switch_no.obj", shaderProgram);
		break;
	}

	size = model_yes->getSize();
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

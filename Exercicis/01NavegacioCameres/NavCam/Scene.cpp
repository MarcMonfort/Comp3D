#include <iostream>
#include <cmath>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define PI 3.14159f


Scene::Scene()
{
	level = NULL;
}

Scene::~Scene()
{
	if(level != NULL)
		delete level;
}


void Scene::init()
{
	initShaders();
	level = Level::createLevel(glm::vec3(16, 4, 32), texProgram, "images/floor.png", "images/wall.png");

	entity = SimpleObject::createSimpleObject(0, 3, 30, 30, &texProgram);

	projection = glm::perspective(90.f / 180.f * PI, float(CAMERA_WIDTH) / float(CAMERA_HEIGHT), 0.1f, 100.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	if (Game::instance().getKey('1')) {
		eCam = Camera::CAM_FPS;
	}
	else if (Game::instance().getKey('2')) {
		eCam = Camera::CAM_FIX1;
	}
	else if (Game::instance().getKey('3')) {
		eCam = Camera::CAM_FIX2;
	}
	else if (Game::instance().getKey('4')) {
		eCam = Camera::CAM_FIX3;
	}
	else if (Game::instance().getKey('5')) {
		eCam = Camera::CAM_FIX4;
	}

	//glm::vec3 posEntity = entity->getPosition();
	if (Game::instance().getKey('w')) {
		posEntity.z -= 1;
	}
	if (Game::instance().getKey('a')) {
		posEntity.x -= 1;
	}
	if (Game::instance().getKey('s')) {
		posEntity.z += 1;
	}
	if (Game::instance().getKey('d')) {
		posEntity.x += 1;
	}

	entity->setPosition(posEntity);

	currentTime += deltaTime;
}

void Scene::render()
{
	glm::mat4 modelview;

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);

	switch (eCam)
	{
		case Camera::CAM_FPS:
			modelview = glm::lookAt(glm::vec3(posEntity.x, posEntity.y, posEntity.z), glm::vec3(0, 0, -100), glm::vec3(0, 1, 0));
			break;
		case Camera::CAM_FIX1:
			modelview = glm::mat4(1.0f);
			break;
		case Camera::CAM_FIX2:
			modelview = glm::lookAt(glm::vec3(-20, 20, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			break;
		case Camera::CAM_FIX3:
			modelview = glm::lookAt(glm::vec3(6, 10, 20), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			break;
		case Camera::CAM_FIX4:
			modelview = glm::lookAt(glm::vec3(0, 20, 0), glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
			break;
		default:
			modelview = glm::mat4(1.0f);
			break;
	}
	texProgram.setUniformMatrix4f("modelview", modelview);
	level->render();

	modelview = glm::translate(modelview, glm::vec3(posEntity.x, posEntity.y, posEntity.z));
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform4f("color", 1.0f, 0.0f, 0.0f, 0.0f);
	entity->render();
}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if(!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}




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

	lastMousePosition = glm::vec2(CAMERA_WIDTH / 2, CAMERA_HEIGHT / 2);
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

	/*currentMousePosition = Game::instance().getMousePosition();
	yaw -= (currentMousePosition.x - lastMousePosition.x)/100;
	cout << lastMousePosition.x << "   " << currentMousePosition.x << endl;
	lastMousePosition = currentMousePosition;*/

	yaw -= Game::instance().getMouseOffset().x;
	pitch -= Game::instance().getMouseOffset().y;
	
	cout << Game::instance().getMouseOffset().x << endl;

	//glm::vec3 posEntity = entity->getPosition();
	if (Game::instance().getKey('a')) {
		yaw += 5;
	}
	if (Game::instance().getKey('d')) {
		yaw -= 5;
	}

	if (Game::instance().getKey('w')) {
		posEntity.x += (velocity*deltaTime) * cos(glm::radians(yaw));
		posEntity.z -= (velocity*deltaTime) * sin(glm::radians(yaw));
	}
	if (Game::instance().getKey('s')) {
		posEntity.x -= (velocity*deltaTime) * cos(glm::radians(yaw));
		posEntity.z += (velocity*deltaTime) * sin(glm::radians(yaw));
	}
	if (Game::instance().getKey('q')) {
		posEntity.x += (velocity*deltaTime) * cos(glm::radians(yaw + 90));
		posEntity.z -= (velocity*deltaTime) * sin(glm::radians(yaw + 90));
	}
	if (Game::instance().getKey('e')) {
		posEntity.x -= (velocity*deltaTime) * cos(glm::radians(yaw + 90));
		posEntity.z += (velocity*deltaTime) * sin(glm::radians(yaw + 90));
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
		{
			if (bTest) {
				modelview = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), glm::vec3(1, 0, 0));	//  Estamos rotando TODA la escena.
				modelview = glm::rotate(modelview, -glm::radians(yaw - 90), glm::vec3(0, 1, 0));	//el orden en la camara va al reves que el objeto.
				modelview = glm::translate(modelview, glm::vec3(-posEntity.x, -posEntity.y, -posEntity.z));
			}
			else {
				// forma alternativa usando LookAt
				glm::vec3 cameraPos = glm::vec3(posEntity.x, posEntity.y, posEntity.z);
				glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
				glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 direction;
				direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
				direction.y = sin(glm::radians(pitch));
				direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
				cameraFront = glm::normalize(direction);
				modelview = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			}
		}
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
	modelview = glm::rotate(modelview, glm::radians(yaw - 90), glm::vec3(0, 1, 0));
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform4f("color", 1.0f, 0.0f, 0.0f, 0.0f);
	entity->render();
}




void Scene::keyPressed(int key)
{
	if (key == 't')
	{
		bTest = !bTest;
	}
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




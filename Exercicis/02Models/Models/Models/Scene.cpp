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
	model = new AssimpModel();
	model->loadFromFile("models/cube624.obj", texProgram);
	model_2 = new AssimpModel();
	model_2->loadFromFile("models/cube2.obj", texProgram);
	projection = glm::perspective(90.f / 180.f * PI, float(CAMERA_WIDTH) / float(CAMERA_HEIGHT), 0.1f, 100.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;

	camera.yaw += Game::instance().getMouseOffset().x;
	camera.pitch += Game::instance().getMouseOffset().y;
	if (Game::instance().getKey('a')) {
		camera.position.x -= (camera.velocity * deltaTime) * cos(glm::radians(camera.yaw + 90));
		camera.position.z -= (camera.velocity * deltaTime) * sin(glm::radians(camera.yaw + 90));
	}
	if (Game::instance().getKey('d')) {
		camera.position.x += (camera.velocity * deltaTime) * cos(glm::radians(camera.yaw + 90));
		camera.position.z += (camera.velocity * deltaTime) * sin(glm::radians(camera.yaw + 90));
	}
	if (Game::instance().getKey('q')) {
		camera.roll += 5;
	}
	if (Game::instance().getKey('e')) {
		camera.roll -= 5;
	}

	glm::vec3 direction;
	direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
	direction.y = sin(glm::radians(camera.pitch));
	direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
	camera.front = glm::normalize(direction);

	if (Game::instance().getKey('w')) {
		camera.position.x += (camera.velocity * deltaTime) * camera.front.x;
		camera.position.y += (camera.velocity * deltaTime) * camera.front.y;
		camera.position.z += (camera.velocity * deltaTime) * camera.front.z;
	}
	if (Game::instance().getKey('s')) {
		camera.position.x -= (camera.velocity * deltaTime) * camera.front.x;
		camera.position.y -= (camera.velocity * deltaTime) * camera.front.y;
		camera.position.z -= (camera.velocity * deltaTime) * camera.front.z;
	}
}

void Scene::render()
{
	glm::mat4 modelMatrix, viewMatrix;
	glm::mat3 normalMatrix;

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);

	viewMatrix = glm::lookAt(camera.position, camera.position + camera.front, glm::vec3(0.f, 1.f, 0.f));

	// Render level
	modelMatrix = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", viewMatrix * modelMatrix);
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
	texProgram.setUniformMatrix3f("normalmatrix", normalMatrix);
	level->render();

	// Render loaded model
	float scaleFactor = 1.f / model->getHeight();
	glm::vec3 centerModelBase = model->getCenter() - glm::vec3(0.f, -model->getHeight() / 2.f, 0.f);

	modelMatrix = glm::mat4(1.0f);
	/*modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.5f * fabs(sinf(3.f * currentTime / 1000.f)), 0.f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.f, -5.f));
	modelMatrix = glm::rotate(modelMatrix, currentTime / 1000.f, glm::vec3(0.f, 1.f, 0.f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
	modelMatrix = glm::translate(modelMatrix, -centerModelBase);*/

	modelMatrix = glm::translate(modelMatrix, -model->getCenter());
	texProgram.setUniformMatrix4f("modelview", viewMatrix * modelMatrix);

	normalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
	texProgram.setUniformMatrix3f("normalmatrix", normalMatrix);

	model->render(texProgram);

	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, -3.f));
	texProgram.setUniformMatrix4f("modelview", viewMatrix * modelMatrix);
	model_2->render(texProgram);

	cout << "center: " << model->getCenter().x << " " << model->getCenter().y << " " << model->getCenter().z << endl;
	cout << "size:   " << model->getSize().x << " " << model->getSize().y << " " << model->getSize().z << endl;
	//cout << model->getHeight() << "--" << model_2->getHeight() << endl;
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




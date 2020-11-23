#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"


#define PI 3.14159f


Scene::Scene()
{
	level = NULL;
	model = NULL;
	billboard = NULL;
	particles = NULL;
	map = NULL;
}

Scene::~Scene()
{
	if (level != NULL)
		delete level;
	if (model != NULL)
		delete model;
	if (billboard != NULL)
		delete billboard;
	if (particles != NULL)
		delete particles;
	if (map != NULL)
		delete map;
}


void Scene::init()
{
	initShaders();
	level = Level::createLevel(glm::vec3(16, 4, 32), texProgram, "images/floor.png", "images/wall.png");
	model = new AssimpModel();
	model->loadFromFile("models/cube16_border.obj", texProgram);
	billboard = Billboard::createBillboard(glm::vec2(1.f, 1.f), texProgram, "images/mushroom.png");
	billboard->setType(BILLBOARD_Y_AXIS);

	// Initialize particle system
	ParticleSystem::Particle particle;
	particle.lifetime = 1e10f;
	particles = new ParticleSystem();
	particles->init(glm::vec2(0.5f, 0.5f), texProgram, "images/particle.png", 2.f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// Initialize TileMap
	map = TileMap::createTileMap("levels/level01.txt", glm::vec2(0, 0), texProgram);

	projection = glm::perspective(90.f / 180.f * PI, float(CAMERA_WIDTH) / float(CAMERA_HEIGHT), 0.1f, 100.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	int nParticlesToSpawn = 20 * (int((currentTime + deltaTime) / 100.f) - int(currentTime / 100.f));
	ParticleSystem::Particle particle;
	float angle;

	particle.lifetime = 1.4f;
	for (int i = 0; i < nParticlesToSpawn; i++)
	{
		angle = 2.f * PI * (i + float(rand()) / RAND_MAX) / nParticlesToSpawn;
		particle.position = glm::vec3(cos(angle), -1.75f, sin(angle));
		particle.speed = 1.5f * glm::normalize(0.5f * particle.position + glm::vec3(0.f, 3.f, 0.f));
		particles->addParticle(particle);
	}

	currentTime += deltaTime;

	particles->update(deltaTime / 1000.f);
}

void Scene::render()
{
	glm::mat4 modelMatrix, viewMatrix;
	glm::mat3 normalMatrix;

	texProgram.use();
	texProgram.setUniform1b("bLighting", true);
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);

	/*glm::vec3 obs = glm::vec3(8.f * sin(currentTime / 10000.f), 1.f, 8.f * cos(currentTime / 10000.f));
	viewMatrix = glm::lookAt(obs, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));*/

	// Camera position
	viewMatrix = glm::mat4(1.0f);
	viewMatrix = glm::translate(viewMatrix, glm::vec3(0.f, 0.f, -10.f));
	texProgram.setUniformMatrix4f("view", viewMatrix);



	// Render level
	modelMatrix = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("model", modelMatrix);
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
	texProgram.setUniformMatrix3f("normalmatrix", normalMatrix);
	level->render();

	// Render loaded model
	/*float scaleFactor = 2.f / model->getHeight();
	glm::vec3 centerModelBase = model->getCenter() - glm::vec3(0.f, model->getHeight() / 2.f, 0.f);*/

	modelMatrix = glm::mat4(1.0f);
	/*modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.5f * fabs(sinf(3.f * currentTime / 1000.f)), 0.f));
	modelMatrix = glm::rotate(modelMatrix, currentTime / 1000.f, glm::vec3(0.f, 1.f, 0.f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, -2.f, 0.f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
	modelMatrix = glm::translate(modelMatrix, -centerModelBase);*/
	modelMatrix = glm::translate(modelMatrix, model->getCenter());
	texProgram.setUniformMatrix4f("model", modelMatrix);

	normalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
	texProgram.setUniformMatrix3f("normalmatrix", normalMatrix);

	//model->render(texProgram);

	// Render loaded model (second time & third time)
	modelMatrix = glm::translate(modelMatrix, glm::vec3(model->getSize().x,0.f,0.f));
	texProgram.setUniformMatrix4f("model", modelMatrix);
	//model->render(texProgram);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(model->getSize().x, 0.f, 0.f));
	texProgram.setUniformMatrix4f("model", modelMatrix);
	//model->render(texProgram);

	// Render TileMap
	map->render(texProgram);

	// Render billboard
	/*texProgram.setUniform1b("bLighting", false);
	modelMatrix = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", viewMatrix * modelMatrix);
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
	texProgram.setUniformMatrix3f("normalmatrix", normalMatrix);
	billboard->render(glm::vec3(2.f, -1.5f, 0.f), obs);
	billboard->render(glm::vec3(-2.f, -1.5f, 0.f), obs);
	billboard->render(glm::vec3(0.f, -1.5f, 2.f), obs);
	billboard->render(glm::vec3(0.f, -1.5f, -2.f), obs);*/

	// Render particles
	/*glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);

	modelMatrix = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", viewMatrix * modelMatrix);
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
	texProgram.setUniformMatrix3f("normalmatrix", normalMatrix);
	particles->render(obs);

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);*/
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




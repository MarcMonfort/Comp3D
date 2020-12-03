#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define PI 3.14159f


Scene::Scene()
{
	level = NULL;
	model = NULL;
	billboard = NULL;
	particles = NULL;
	map = NULL;
	player = NULL;
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
	if (player != NULL)
		delete player;
}


void Scene::init(int numLevel)
{
	initShaders();
	level = Level::createLevel(glm::vec3(16, 4, 32), texProgram, "images/floor.png", "images/wall.png");
	model = new AssimpModel();
	model->loadFromFile("models/cube10_test.obj", texProgram);
	billboard = Billboard::createBillboard(glm::vec2(1.f, 1.f), texProgram, "images/mushroom.png");
	billboard->setType(BILLBOARD_Y_AXIS);

	// Initialize particle system
	ParticleSystem::Particle particle;
	particle.lifetime = 1e10f;
	particles = new ParticleSystem();
	particles->init(glm::vec2(0.5f, 0.5f), texProgram, "images/particle.png", 2.f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// Initialize TileMap
	string pathLevel = "levels/level0" + to_string(numLevel) + ".txt";
	map = TileMap::createTileMap(pathLevel, glm::vec2(0, 0), texProgram);

	// Init Camera. Depends on the level. Maybe use a map->getStartPosition()...
	camera.position = map->getCenterCamera();
	camera.movement = map->getMovementCamera();

	// Init Player
	player = new Player();
	player->init(texProgram);
	player->setPosition(map->getCheckPointPlayer());
	player->setTileMap(map);

	// Init CheckPoint (player/camera)
	checkpoint.posCamera = map->getCenterCamera();
	checkpoint.posPlayer = map->getCheckPointPlayer();

	// Init Walls
	vector<TileMap::Wall> pos_walls = map->getWalls();	// error wall struct
	for (int i = 0; i < pos_walls.size(); ++i)  // maybe bolean to know if there is any...?
	{ 
		Wall* wall = new Wall();
		wall->init(texProgram, pos_walls[i].bVertical, static_cast<Wall::Type>(pos_walls[i].type));
		wall->setPosition(glm::vec3(pos_walls[i].position,0));
		wall->setTileMap(map);
		walls.push_back(wall);
	}

	// End Inits
	projection = glm::perspective(glm::radians(45.f), float(CAMERA_WIDTH) / float(CAMERA_HEIGHT), 0.1f, 1000.f);
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
	//NEW

	if (map->getNewCheckPoint() && eCamMove == CamMove::STATIC)
	{
		checkpoint.posPlayer = map->getCheckPointPlayer();
		checkpoint.posCamera = camera.position;
		map->setNewCheckPoint(false);
	}

	if (map->getPlayerDead())
	{
		player->setPosition(checkpoint.posPlayer);
		camera.position = checkpoint.posCamera;
		eCamMove = CamMove::STATIC;
		map->setPlayerDead(false);
	}

	glm::vec3 posPlayer = player->getPosition();
	glm::vec3 sizePlayer = player->getSize();
	// Los valores se han puesto a ojo. Si se hace resize del mapa
	// hay que tener en cuenta tanto el tama�o del player, como la
	// posicion inicial de la camara que esta desplazada 0.5, ya 
	// que el centro de un bloque aparece en el centro de la base
	if (posPlayer.x + sizePlayer.x - camera.position.x > 10.5 && eCamMove == CamMove::STATIC)
	{
		timeCamMove = camera.movement.x;
		eCamMove = CamMove::RIGHT;
		//player->setVelocity(glm::vec3(0, 0, 0));
	}
	else if (camera.position.x - posPlayer.x > 9.5 && eCamMove == CamMove::STATIC)
	{
		timeCamMove = camera.movement.x;
		eCamMove = CamMove::LEFT;
		//player->setVelocity(glm::vec3(0, 0, 0));
	}

	if (camera.position.y + posPlayer.y > 7.5 && eCamMove == CamMove::STATIC)
	{
		timeCamMove = camera.movement.y;
		eCamMove = CamMove::DOWN;
		//player->setVelocity(glm::vec3(0, 0, 0));
	}
	else if (- posPlayer.y - camera.position.y > 6.5 && eCamMove == CamMove::STATIC)
	{
		timeCamMove = camera.movement.y;
		eCamMove = CamMove::UP;
		//player->setVelocity(glm::vec3(0, 0, 0));
	}

	switch (eCamMove)
	{
		case Scene::CamMove::STATIC:
			break;
		case Scene::CamMove::RIGHT:
			camera.position.x += camera.velocity * deltaTime;
			timeCamMove -= camera.velocity * deltaTime;
			if (timeCamMove <= 0.f) {
				camera.position.x += timeCamMove;
				eCamMove = CamMove::STATIC;
			}
			break;
		case Scene::CamMove::LEFT:
			camera.position.x -= camera.velocity * deltaTime;
			timeCamMove -= camera.velocity * deltaTime;
			if (timeCamMove <= 0.f) {
				camera.position.x -= timeCamMove;
				eCamMove = CamMove::STATIC;
			}
			break;
		case Scene::CamMove::UP:
			camera.position.y += camera.velocity * deltaTime;
			timeCamMove -= camera.velocity * deltaTime;
			if (timeCamMove <= 0.f) {
				camera.position.y += timeCamMove;
				eCamMove = CamMove::STATIC;
			}
			break;
		case Scene::CamMove::DOWN:
			camera.position.y -= camera.velocity * deltaTime;
			timeCamMove -= camera.velocity * deltaTime;
			if (timeCamMove <= 0.f) {
				camera.position.y -= timeCamMove;
				eCamMove = CamMove::STATIC;
			}
			break;
	}

	player->update(deltaTime, &walls);

	for (int i = 0; i < walls.size(); ++i)
	{
		// mes eficiente si solo hace update de los que s�n visibles!!!!
		walls[i]->update(deltaTime, player->getPosition(), player->getSize());
	}

	map->update(deltaTime);

}

void Scene::render()
{
	glm::mat4 modelMatrix, viewMatrix;
	glm::mat3 normalMatrix;

	texProgram.use();
	texProgram.setUniform1b("bLighting", true);	//si es fals, no se ven sombras
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);


	// Camera position
	viewMatrix = glm::mat4(1.0f);
	viewMatrix = glm::translate(viewMatrix, -camera.position);
	texProgram.setUniformMatrix4f("view", viewMatrix);


	// Init matrix
	modelMatrix = glm::mat4(1.0f);
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
	texProgram.setUniformMatrix3f("normalmatrix", normalMatrix);


	

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

	// Render TileMap
	map->render(texProgram);

	// Render Player
	player->render(texProgram);

	// Render Walls
	for (int i = 0; i < walls.size(); ++i)
	{
		walls[i]->render(texProgram, player->getPosition());
	}
}

void Scene::keyPressed(int key)
{
	if (key == 'd' && eCamMove == CamMove::STATIC)
	{
		timeCamMove = 17;
		eCamMove = CamMove::RIGHT;
	}
	if (key == 's' && eCamMove == CamMove::STATIC)
	{
		timeCamMove = 14;
		eCamMove = CamMove::DOWN;
	}
	if (key == 'a' && eCamMove == CamMove::STATIC)
	{
		timeCamMove = 17;
		eCamMove = CamMove::LEFT;
	}
	if (key == 'w' && eCamMove == CamMove::STATIC)
	{
		timeCamMove = 14;
		eCamMove = CamMove::UP;
	}

	player->keyPressed(key);
}


void Scene::reshape(int width, int height)
{
	float FOV = glm::radians(90.f);
	float rav = float(width) / float(height);
	if (rav < 1.f)
		FOV = 2.0 * atan( tan(glm::radians(90.f/2)) / rav);

	projection = glm::perspective(FOV, rav, 0.1f, 1000.f);
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




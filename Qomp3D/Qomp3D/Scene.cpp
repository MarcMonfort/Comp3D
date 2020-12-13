#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define PI 3.14159f
#define NUM_LEVELS 5


Scene::Scene()
{
	//level = NULL;
	//model = NULL;
	billboard = NULL;
	particles = NULL;
	map = NULL;
	player = NULL;
}

Scene::~Scene()
{
	/*if (level != NULL)
		delete level;
	if (model != NULL)
		delete model;*/
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
	//level = Level::createLevel(glm::vec3(16, 4, 32), texProgram, "images/floor.png", "images/wall.png");
	//model = new AssimpModel();
	//model->loadFromFile("models/cube10_test.obj", texProgram);
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
	roomSize = map->getRoomSize();
	glm::vec3 rgb = map->getColorBackground();
	glClearColor(rgb.x, rgb.y, rgb.z, 1.0f);
	lastLevel = numLevel == NUM_LEVELS + 1;

	//Init Music
	if (lastLevel)
	{
		music = SoundManager::instance().loadSound("sounds/fireworks.mp3", FMOD_LOOP_NORMAL);
	}
	else {
		style = map->getStyle();
		string theme = themes[style];
		music = SoundManager::instance().loadSound(theme, FMOD_LOOP_NORMAL);
	}
	channel = SoundManager::instance().playSound(music);
	channel->setVolume(1.f);

	// Init Camera. Depends on the level. Maybe use a map->getStartPosition()...
	camera.position = map->getCenterCamera();
	camera.movement = map->getMovementCamera();

	// Init Player
	player = new Player();
	player->init(texProgram, map);
	player->setPosition(map->getCheckPointPlayer());
	//player->setTileMap(map);

	// Init CheckPoint (player/camera)
	checkpoint.posCamera = map->getCenterCamera();
	checkpoint.posPlayer = map->getCheckPointPlayer();

	// Init Walls
	vector<TileMap::Wall> pos_walls = map->getWalls();
	for (int i = 0; i < pos_walls.size(); ++i)
	{ 
		Wall* wall = new Wall();
		wall->init(texProgram, pos_walls[i].bVertical, static_cast<Wall::Type>(pos_walls[i].type), map);
		wall->setPosition(glm::vec3(pos_walls[i].position,0));
		//wall->setTileMap(map);
		walls.push_back(wall);
	}

	// Init BallSpikes
	vector<pair<bool, glm::vec2>> pos_ballSpikes = map->getBallSpikes();
	for (int i = 0; i < pos_ballSpikes.size(); ++i)  // maybe bolean to know if there is any...?
	{
		BallSpike* ballSpike = new BallSpike();
		ballSpike->init(texProgram, pos_ballSpikes[i].first, map);
		ballSpike->setPosition(glm::vec3(pos_ballSpikes[i].second, 0));
		//ballSpike->setTileMap(map);
		ballSpikes.push_back(ballSpike);
	}

	// Init Buttons
	vector<tuple<bool, glm::vec2, int>> pos_buttons = map->getButtons();
	for (int i = 0; i < pos_buttons.size(); ++i)  // maybe bolean to know if there is any...?
	{
		Button* button = new Button();
		button->init(texProgram, get<0>(pos_buttons[i]));
		button->setPosition(glm::vec3(get<1>(pos_buttons[i]), 0));
		button->setOrientation(get<2>(pos_buttons[i]));
		button->setTileMap(map);
		buttons.push_back(button);
	}

	// Init Switchs
	vector<pair<bool, glm::vec2>> pos_switchs = map->getSwitchs();
	for (int i = 0; i < pos_switchs.size(); ++i)  // maybe bolean to know if there is any...?
	{
		Switch* switx = new Switch();
		switx->init(texProgram, pos_switchs[i].first, map);
		switx->setPosition(glm::vec3(pos_switchs[i].second, 0));
		//switx->setTileMap(map);
		switchs.push_back(switx);
	}

	// Init God Mode Sprite
	godMode_spritesheet.loadFromFile("images/godmode.png", TEXTURE_PIXEL_FORMAT_RGBA);
	godMode_sprite = Sprite::createSprite(glm::ivec2(128, 16), glm::vec2(1.f, 1.f), &godMode_spritesheet, &texProgram);
	godMode_sprite->setPosition(glm::vec2(50, 690));

	// Init Fade
	fade_spritesheet.loadFromFile("images/fade.png", TEXTURE_PIXEL_FORMAT_RGBA);
	fade_sprite = Sprite::createSprite(glm::ivec2(12800, 12800), glm::vec2(1.f, 1.f), &fade_spritesheet, &texProgram);
	fade_sprite->setPosition(glm::vec2(0, 0));
	totalFadeTime = 750;
	fadeTime = 0;
	fadeIn = true;
	fadeOut = false;

	// End Inits
	projection = glm::perspective(glm::radians(45.f), float(CAMERA_WIDTH) / float(CAMERA_HEIGHT), 0.1f, 1000.f);
	currentTime = 0.0f;

	firstUpdate = true;

	if (lastLevel)
	{
		// Init Crown
		crown = new AssimpModel();
		crown->loadFromFile("models/crown.obj", texProgram);
	}
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;

	if (firstUpdate)
	{
		deltaTime = 0;
		firstUpdate = false;
	}

	if (fadeIn || fadeOut)
	{
		fadeTime += deltaTime;

		if (fadeTime >= totalFadeTime) {
			if (fadeIn)
			{
				fadeIn = false;
				fadeTime = 0;
			}
			else if (fadeOut)
			{
				PlayGameState::instance().finalBlockTaken();
			}
		}
	}

	
	if (!fadeIn && !fadeOut)
	{
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

		if (posPlayer.x + sizePlayer.x - camera.position.x > (roomSize.x / 2) && eCamMove == CamMove::STATIC)
		{
			timeCamMove = camera.movement.x;
			eCamMove = CamMove::RIGHT;
		}
		else if (camera.position.x - posPlayer.x > (roomSize.x / 2) && eCamMove == CamMove::STATIC)
		{
			timeCamMove = camera.movement.x;
			eCamMove = CamMove::LEFT;
		}

		if (camera.position.y + (posPlayer.y + sizePlayer.y) > (roomSize.y / 2) && eCamMove == CamMove::STATIC)
		{
			timeCamMove = camera.movement.y;
			eCamMove = CamMove::DOWN;
		}
		else if (-posPlayer.y - camera.position.y > (roomSize.y / 2) && eCamMove == CamMove::STATIC)
		{
			timeCamMove = camera.movement.y;
			eCamMove = CamMove::UP;
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

		player->update(deltaTime, &walls, &ballSpikes, &buttons, &switchs);

		for (int i = 0; i < walls.size(); ++i)
		{
			walls[i]->update(deltaTime, player->getPosition(), player->getSize());
		}

		//update BallSpikes
		for (int i = 0; i < ballSpikes.size(); ++i)
		{
			ballSpikes[i]->update(deltaTime, player->getPosition());
		}

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



	// Render TileMap
	map->render(texProgram, player->getPosition());

	// Render Player
	if (PlayGameState::instance().getGodMode()) {
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		texProgram.setUniform1f("alpha", 0.3);
	}
	player->render(texProgram, camera.position);
	if (PlayGameState::instance().getGodMode()) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	// Render Walls
	for (int i = 0; i < walls.size(); ++i)
	{
		walls[i]->render(texProgram, player->getPosition());
	}

	// Render BlockSpikes
	for (int i = 0; i < ballSpikes.size(); ++i)
	{
		ballSpikes[i]->render(texProgram, player->getPosition(), viewMatrix);

		//se podria poner al final...
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
		texProgram.setUniformMatrix3f("normalmatrix", normalMatrix);
		//se podria poner al final...
	}

	// Render Buttons
	for (int i = 0; i < buttons.size(); ++i)
	{
		buttons[i]->render(texProgram);
	}

	// Render Switchs
	for (int i = 0; i < switchs.size(); ++i)
	{
		switchs[i]->render(texProgram);
	}

	// Render crown
	if (lastLevel) {
		glm::vec3 playerPos = player->getPosition();
		modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(playerPos.x, -playerPos.y + 1, 0.f));
		texProgram.setUniformMatrix4f("model", modelMatrix);
		crown->render(texProgram);
	}


	// LO ULTIMO (2D)

	if (PlayGameState::instance().getGodMode()) {
		texProgram.setUniformMatrix4f("projection", glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f));
		//texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
		modelMatrix = glm::mat4(1.0f);
		//texProgram.setUniformMatrix4f("model", modelMatrix);
		texProgram.setUniformMatrix4f("view", glm::mat4(1.0f));
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		texProgram.setUniform1b("bLighting", false);
		godMode_sprite->render();
	}

	if (fadeIn)
	{
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		texProgram.setUniformMatrix4f("projection", glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f));
		texProgram.setUniformMatrix4f("view", glm::mat4(1.0f));
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		texProgram.setUniform1b("bLighting", false);
		float alpha = min(1.0f, fadeTime / totalFadeTime);
		texProgram.setUniform1f("alpha", 1 - alpha);
		fade_sprite->render();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	else if (fadeOut)
	{
		player->setVelocity(glm::vec3(0.f, 0.f, 0.f));

		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		texProgram.setUniformMatrix4f("projection", glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f));
		texProgram.setUniformMatrix4f("view", glm::mat4(1.0f));
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		texProgram.setUniform1b("bLighting", false);
		float alpha = min(1.0f, fadeTime / totalFadeTime);
		texProgram.setUniform1f("alpha", alpha);
		fade_sprite->render();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
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


void Scene::setFade(bool b) {
	fadeOut = b;
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




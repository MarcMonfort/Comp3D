#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "Level.h"
#include "AssimpModel.h"
#include "Billboard.h"
#include "ParticleSystem.h"
#include "TileMap.h"
#include "Player.h"
#include "Wall.h"
#include "BallSpike.h"
#include "Button.h"
#include "Switch.h"
#include "Sprite.h"



#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init(int numLevel);
	void update(int deltaTime);
	void render();

	void keyPressed(int key);
	void reshape(int width, int height);

	void setFade(bool b);

private:
	void initShaders();

private:
	Level *level;
	AssimpModel *model;
	Billboard *billboard;
	ParticleSystem *particles;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

	//Extra
	TileMap* map;

	struct Camera
	{
		glm::vec3 position = glm::vec3(0); // 16 (half-map) * 1.6 (size-block)
		float velocity = 0.1;
		glm::vec2 movement;
	} camera;

	enum class CamMove
	{
		STATIC,
		RIGHT,
		LEFT,
		UP,
		DOWN
	};
	CamMove eCamMove = CamMove::STATIC;
	float timeCamMove = 0.f;

	Player* player;
	vector<Wall*> walls;
	vector<BallSpike*> ballSpikes;
	vector<Button*> buttons;
	vector<Switch*> switchs;

	struct CheckPoint
	{
		glm::vec3 posPlayer;
		glm::vec3 posCamera;
	} checkpoint;


	glm::vec2 roomSize;

	Texture godMode_spritesheet;
	Sprite* godMode_sprite;

	Texture fade_spritesheet;
	Sprite* fade_sprite;

	int style;

	FMOD::Sound* music;
	FMOD::Channel* channel;

	vector<string> themes{
		"sounds/main_theme.mp3",
		"sounds/minecraft.mp3",
		"sounds/theme3.mp3",
		"sounds/theme4.mp3",
		"sounds/theme5.mp3"
	};

	float totalFadeTime;
	float fadeTime;

	bool fadeIn;
	bool fadeOut;

	bool firstUpdate = true;

	AssimpModel* crown;

	bool bDead = false;
	int timeDead;

};


#endif // _SCENE_INCLUDE

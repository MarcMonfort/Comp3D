#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
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
	void setEscape(bool b);

private:
	void initShaders();

private:
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
		DOWN,
		FOLLOW
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

	FMOD::Sound* fireworks;
	FMOD::Channel* fireworks_channel;

	vector<string> themes{
		"sounds/sky.mp3",			// original
		"sounds/underwater.mp3",	// water
		"sounds/factory.mp3",		// box
		"sounds/mario.mp3",			// mario
		"sounds/underground.mp3"	// minecraft
	};

	float totalFadeTime;
	float fadeTime;

	bool fadeIn;
	bool fadeOut;

	bool firstUpdate = true;

	bool lastLevel;
	AssimpModel* crown;

	bool bDead = false;
	int timeDead;

	float rotation;

	float victoryTime;

	float maxMusicVolume;

	bool escape;

};


#endif // _SCENE_INCLUDE

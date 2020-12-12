#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE

#include "TileMap.h"
#include "SoundManager.h"
#include "Button.h"
#include "Switch.h"
#include "Wall.h"
#include "BallSpike.h"
#include "ParticleSystem.h"


enum orientation
{
	UP,
	LEFT,
	DOWN,
	RIGHT,
};


class Player
{
public:
	void init(ShaderProgram& shaderProgram);
	void update(int deltaTime, vector<Wall*>* walls, vector<BallSpike*>* ballSpike, vector<Button*>* buttons, vector<Switch*>* switchs);
	void render(ShaderProgram& program, const glm::vec3& eye);

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec3& pos);
	void setVelocity(const glm::vec3& vel);

	glm::vec3 getPosition();
	glm::vec3 getSize();

	void keyPressed(int key);

private:
	bool collideWall(Wall* wall);
	bool collideBallSpike(BallSpike* blockSpike);
	bool collideButton(Button* button);
	bool collideSwitch(Switch* switx);

	void switchAllSwitchs(vector<Switch*>* switchs);
	void unpressAllButtons(vector<Button*>* buttons);

private:
	glm::vec3 posPlayer;
	glm::vec3 size;
	glm::vec3 velocity = glm::vec3(0);
	float currentTime;

	TileMap* map;
	AssimpModel* model;
	ParticleSystem* particles;


	float lastVelocity = 0;

	bool bSpace = false;
	int timeRotate;
	int timeScale;
	orientation eScaleDir;



	FMOD::Sound* wall_sound;
	FMOD::Sound* player_sound;
	FMOD::Sound* button_sound;
	FMOD::Sound* line_sound;
	FMOD::Sound* death_sound;
	FMOD::Sound* basic_sound;

	FMOD::Channel* channel;
	FMOD::Channel* line_channel;
};

#endif

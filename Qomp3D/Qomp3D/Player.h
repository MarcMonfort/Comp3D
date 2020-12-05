#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE

#include "TileMap.h"
#include "SoundManager.h"
#include "Button.h"
#include "Switch.h"
#include "Wall.h"
#include "BallSpike.h"

enum orientation
{
	up,
	left,
	down,
	right,
};

class Wall;

class Player
{
public:
	void init(ShaderProgram& shaderProgram);
	void update(int deltaTime, vector<Wall*>* walls, vector<BallSpike*>* ballSpike, vector<Button*>* buttons, vector<Switch*>* switchs);
	void render(ShaderProgram& program);

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec3& pos);
	void setVelocity(const glm::vec3& vel);

	glm::vec3 getPosition();
	glm::vec3 getSize();

	void keyPressed(int key);


private:
	glm::vec3 posPlayer;
	TileMap* map;

	glm::vec3 velocity = glm::vec3(0);

	const SoundManager* soundManager;
	FMOD::Sound* sound;
	FMOD::Channel* channel;

	AssimpModel* model;

	bool collideWall(Wall* wall);
	bool collideBallSpike(BallSpike* blockSpike);
	bool collideButton(Button* button);
	bool collideSwitch(Switch* switx);

	float lastVelocity = 0;

	void switchAllSwitchs(vector<Switch*>* switchs);
	void unpressAllButtons(vector<Button*>* buttons);

};

#endif

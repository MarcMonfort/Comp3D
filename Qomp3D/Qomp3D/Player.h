#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE

#include "TileMap.h"
#include "SoundManager.h"

class Player
{
public:
	void init(ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render(ShaderProgram& program);

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec3& pos);

	glm::vec2 getPosition();

	void keyPressed(int key);


private:
	glm::vec3 posPlayer;
	TileMap* map;

	glm::vec3 velocity = glm::vec3(0);

	const SoundManager* soundManager;
	FMOD::Sound* sound;
	FMOD::Channel* channel;

	AssimpModel* model;

};

#endif

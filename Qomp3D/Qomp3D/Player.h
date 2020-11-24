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


private:
	glm::vec3 posPlayer;
	TileMap* map;

	glm::vec2 velocity;

	const SoundManager* soundManager;
	FMOD::Sound* sound;
	FMOD::Channel* channel;

	AssimpModel* model;

};

#endif

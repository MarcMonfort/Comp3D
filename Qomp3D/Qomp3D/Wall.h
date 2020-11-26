#ifndef _WALL_INCLUDE
#define _WALL_INCLUDE

#include "TileMap.h"
#include "SoundManager.h"

class Wall
{
public:
	void init(ShaderProgram& shaderProgram, bool bVertical);
	void update(int deltaTime);
	void render(ShaderProgram& program);

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec3& pos);
	void setVelocity(float vel);

	glm::vec3 getPosition();
	glm::vec3 getSize();

	void keyPressed(int key);


private:
	glm::vec3 position;
	TileMap* map;

	float velocity = 0;

	const SoundManager* soundManager;
	FMOD::Sound* sound;
	FMOD::Channel* channel;

	AssimpModel* model;

	bool bVertical;
};

#endif

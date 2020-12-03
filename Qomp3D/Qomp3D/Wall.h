#ifndef _WALL_INCLUDE
#define _WALL_INCLUDE

#include "TileMap.h"
#include "SoundManager.h"

class Wall
{
public:
	void init(ShaderProgram& shaderProgram, bool bVertical);
	void update(int deltaTime, const glm::vec3& posPlayer, const glm::vec3& sizePlayer);
	void render(ShaderProgram& program, const glm::vec3& posPlayer);

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec3& pos);
	void setVelocity(float vel);

	glm::vec3 getPosition();
	glm::vec3 getSize();
	bool getOrientation();

	void keyPressed(int key);


private:
	glm::vec3 position;
	TileMap* map;

	float velocity = 0;

	glm::vec3 size;

	const SoundManager* soundManager;
	FMOD::Sound* sound;
	FMOD::Channel* channel;

	AssimpModel* model;

	bool bVertical;

	void followPlayer(const glm::vec3& posPlayer, const glm::vec3& sizePlayer);
	bool collidePlayer(const glm::vec3& posPlayer, const glm::vec3& sizePlayer);
};

#endif

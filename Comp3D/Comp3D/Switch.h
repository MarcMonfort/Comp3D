#ifndef _SWITCH_INCLUDE
#define _SWITCH_INCLUDE

#include "TileMap.h"
#include "SoundManager.h"

class Switch
{
public:
	Switch();
	~Switch();

	void init(ShaderProgram& shaderProgram, bool act, TileMap* tileMap);
	void update(int deltaTime);
	void render(ShaderProgram& program);

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec3& pos);

	glm::vec3 getPosition();
	glm::vec3 getSize();

	void toggle();

	bool getActivated();

private:
	glm::vec3 position;
	TileMap* map;

	glm::vec3 size;

	const SoundManager* soundManager;
	FMOD::Sound* sound;
	FMOD::Channel* channel;

	AssimpModel* model_yes;
	AssimpModel* model_no;

	bool activated = false;
};

#endif

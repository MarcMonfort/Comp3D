#ifndef _BUTTON_INCLUDE
#define _BUTTON_INCLUDE

#include "TileMap.h"
#include "SoundManager.h"

class Button
{
public:
	void init(ShaderProgram& shaderProgram, bool press);
	void update(int deltaTime);
	void render(ShaderProgram& program);

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec3& pos);

	glm::vec3 getPosition();
	glm::vec3 getSize();

	void setPressed(bool press);
	bool getPressed();

	void setOrientation(int orient);

private:
	glm::vec3 position;
	TileMap* map;

	glm::vec3 size;

	const SoundManager* soundManager;
	FMOD::Sound* sound;
	FMOD::Channel* channel;

	AssimpModel* model_pressed;
	AssimpModel* model_not_pressed;

	bool pressed = false;

	int orientation = 0;
};

#endif

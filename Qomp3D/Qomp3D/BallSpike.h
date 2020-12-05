#ifndef _BALLSPIKE_INCLUDE
#define _BALLSPIKE_INCLUDE

#include "TileMap.h"
#include "SoundManager.h"

class BallSpike
{
public:
	enum class Type
	{
		EASY = 0,
		HARD = 1,
		IMPOSSIBLE = 2
	};


	void init(ShaderProgram& shaderProgram, bool bVertical);
	void update(int deltaTime, const glm::vec3& posPlayer);
	void render(ShaderProgram& program, const glm::vec3& posPlayer, glm::mat4& viewMatrix);

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec3& pos);
	void setVelocity(float vel);

	glm::vec3 getPosition();
	glm::vec3 getSize();
	bool getOrientation();

private:
	glm::vec3 position;
	TileMap* map;

	float currentTime;

	float velocity = 0;

	glm::vec3 size;

	const SoundManager* soundManager;
	FMOD::Sound* sound;
	FMOD::Channel* channel;

	AssimpModel* model;

	bool bVertical;

	enum class State
	{
		STATIC,
		MOVE,
		OUT
	};

	State state = State::STATIC;

	float stopTime;

	glm::vec3 axis;
};

#endif

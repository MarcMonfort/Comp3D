#ifndef _WALL_INCLUDE
#define _WALL_INCLUDE

#include "TileMap.h"
#include "SoundManager.h"
#include "Switch.h"


class Wall
{
public:

	Wall();
	~Wall();

	enum class Type
	{
		EASY = 0,
		HARD = 1,
		IMPOSSIBLE = 2
	};


	void init(ShaderProgram& shaderProgram, bool bVertical, Type type, TileMap* tileMap);
	void update(int deltaTime, const glm::vec3& posPlayer, const glm::vec3& sizePlayer, vector<Switch*>* switchs);
	void render(ShaderProgram& program, const glm::vec3& posPlayer);

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec3& pos);
	void setVelocity(float vel);

	glm::vec3 getPosition();
	glm::vec3 getSize();
	bool getOrientation();

	int getType();

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

	void followPlayer(const glm::vec2& centerPlayer);
	bool collidePlayer(const glm::vec3& posPlayer, const glm::vec3& sizePlayer);
	bool collideSwitch(Switch* switx);

	bool follow = false;
	float followDist = 0.f;

	enum class State
	{
		STATIC,
		FOLLOW,
		OUT
	};

	State state = State::STATIC;

	int type;
};

#endif

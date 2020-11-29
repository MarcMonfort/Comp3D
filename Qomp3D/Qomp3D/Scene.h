#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "Level.h"
#include "AssimpModel.h"
#include "Billboard.h"
#include "ParticleSystem.h"
#include "TileMap.h"
#include "Player.h"
#include "Wall.h"
#include "Button.h"
#include "Switch.h"


#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init(int numLevel);
	void update(int deltaTime);
	void render();

	void keyPressed(int key);
	void reshape(int width, int height);

private:
	void initShaders();

private:
	Level *level;
	AssimpModel *model;
	Billboard *billboard;
	ParticleSystem *particles;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

	//Extra
	TileMap* map;

	struct Camera
	{
		glm::vec3 position = glm::vec3(0); // 16 (half-map) * 1.6 (size-block)
		glm::vec3 front = glm::vec3(0.f, 0.f, -1.f);
		float yaw = -90;
		float pitch = 0;
		float roll = 0;
		float velocity = 0.05;
	} camera;

	enum class CamMove
	{
		STATIC,
		RIGHT,
		LEFT,
		UP,
		DOWN
	};
	CamMove eCamMove = CamMove::STATIC;

	float timeCamMove = 0.f;

	Player* player;

	vector<Wall*> walls;
	vector<Button*> buttons;
	vector<Switch*> switchs;

};


#endif // _SCENE_INCLUDE


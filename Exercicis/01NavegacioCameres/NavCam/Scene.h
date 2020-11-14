#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "Level.h"
#include "SimpleObject.h"


#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void update(int deltaTime);
	void render();

private:
	void initShaders();

private:
	Level *level;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

	enum class Camera
	{
		CAM_FPS,
		CAM_FIX1,
		CAM_FIX2,
		CAM_FIX3,
		CAM_FIX4
	};
	Camera eCam;

	SimpleObject* entity;
	glm::vec3 posEntity = glm::vec3(0.f);
};


#endif // _SCENE_INCLUDE


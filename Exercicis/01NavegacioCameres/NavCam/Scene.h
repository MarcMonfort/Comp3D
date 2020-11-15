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

	void keyPressed(int key);


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
	float yaw = 90;	//start looking at -Z
	float pitch = 0;
	float velocity = 0.1;

	SimpleObject* entity;
	glm::vec3 posEntity = glm::vec3(0.f);

	glm::vec2 lastMousePosition;// = glm::vec2(CAMERA_WIDTH / 2, CAMERA_HEIGHT / 2);
	glm::vec2 currentMousePosition;

	bool bTest = true;
};


#endif // _SCENE_INCLUDE


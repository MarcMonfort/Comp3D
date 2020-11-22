#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "Level.h"
#include "AssimpModel.h"


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
	AssimpModel* model;
	AssimpModel *model_2;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

	struct Camera
	{
		glm::vec3 position = glm::vec3(0);
		glm::vec3 front = glm::vec3(0.f, 0.f, -1.f);
		float yaw = -90;
		float pitch = 0;
		float roll = 0;
		float velocity = 0.05;
	} camera;

};


#endif // _SCENE_INCLUDE


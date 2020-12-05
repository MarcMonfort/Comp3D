#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include <unordered_map>
#include "Texture.h"
#include "ShaderProgram.h"
#include "AssimpModel.h"
#include <tuple>


// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.


class TileMap
{

public:

	struct Wall {
		glm::vec2 position;
		bool bVertical;
		int type;
	};

	// Tile maps can only be created inside an OpenGL context
	static TileMap* createTileMap(const string& levelFile, const glm::vec2& minCoords, ShaderProgram& program);

	TileMap(const string& levelFile, const glm::vec2& minCoords, ShaderProgram& program);
	~TileMap();

	void render(ShaderProgram& program, const glm::ivec3& posPlayer);
	void update(int deltaTime);
	void free();

	int getTileSize() const { return tileSize; }

	bool collisionMoveLeft(const glm::ivec3& pos, const glm::ivec3& size);
	bool collisionMoveRight(const glm::ivec3& pos, const glm::ivec3& size);
	bool collisionMoveDown(const glm::ivec3& pos, const glm::ivec3& size);
	bool collisionMoveUp(const glm::ivec3& pos, const glm::ivec3& size);

	enum block
	{
		basic,
		key,
		door,
		fin,
		line,
		spike,
		checkpoint,
		checkpoint2,
		x_space
	};

	enum orientation
	{
		up,
		left,
		down,
		right
	};

	vector<tuple<bool, glm::vec2, int>> getButtons() const;
	vector<pair<bool, glm::vec2>> getSwitchs() const;
	vector<pair<bool, glm::vec2>> getBallSpikes() const;
	vector<TileMap::Wall> getWalls();

	bool lineCollision(glm::vec3 &pos, glm::vec3 size, bool vertical);

	bool getPlayerDead();
	void setPlayerDead(bool bDead);

	glm::vec3 getCenterCamera();
	glm::vec2 getMovementCamera();

	glm::vec3 getCheckPointPlayer();
	bool getNewCheckPoint();
	void setNewCheckPoint(bool b);

	glm::vec2 getRoomSize();

private:
	bool loadLevel(const string& levelFile, ShaderProgram& program);
	//void prepareArrays(const glm::vec2& minCoords, ShaderProgram& program);
	int checkBlock(int block);
	bool treatCollision(int pos, int type);

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	glm::ivec2 position, mapSize, tilesheetSize;
	int tileSize, blockSize;
	Texture tilesheet;
	glm::vec2 tileTexSize;
	char* map;
	float currentTime;

	glm::vec2 roomSize;

	std::unordered_map<char, AssimpModel*> models = {};

	std::unordered_map<char, string> paths =
	{
		{'1', "models/cube10_yellow_blue.obj"},
		{'f', "models/cube10_test.obj"},
		{'k', "models/key.obj"},
		{'d', "models/door.obj"},
		{'l', "models/hline3.obj"},
		{'m', "models/vline3.obj"},
		{'r', "models/spike_up.obj"},
		{'s', "models/spike_down.obj"},
		{'t', "models/spike_left.obj"},
		{'u', "models/spike_right.obj"},
		{'o', "models/test.obj"},
		{'c', "models/checkpoint.obj"},
		{'C', "models/checkpoint2.obj"},
		{'j', "models/chain.obj"},
		{'q', "models/lock.obj"}
	};


	glm::vec3 centerCamera;
	glm::vec2 movementCamera;

	glm::vec3 checkpointPlayer;  // puede que no se use como checkpoint, sino que lo haga el scene

	

	vector<TileMap::Wall> walls;
	vector<pair<bool, glm::vec2>> ballSpikes;

	vector<int> doors;
  
	vector<tuple<bool, glm::vec2, int>> buttons;
	vector<pair<bool, glm::vec2>> switchs;

	bool bPlayerDead = false;
	bool bNewCheckPoint = false;
};


#endif // _TILE_MAP_INCLUDE

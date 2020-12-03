#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include <unordered_map>
#include "Texture.h"
#include "ShaderProgram.h"
#include "AssimpModel.h"


// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.


class TileMap
{

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap* createTileMap(const string& levelFile, const glm::vec2& minCoords, ShaderProgram& program);

	TileMap(const string& levelFile, const glm::vec2& minCoords, ShaderProgram& program);
	~TileMap();

	void render(ShaderProgram& program);
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
		wall,
		key,
		door,
		fin,
		line,
		spike,
		checkpoint,
		checkpoint2,
		x_space
	};

	vector<pair<bool, glm::vec2>> getWalls() const;
	bool lineCollision(glm::vec3 pos, glm::vec3 size, bool vertical);

	bool getPlayerDead();
	void setPlayerDead(bool bDead);

	glm::vec3 getCenterCamera();
	glm::vec2 getMovementCamera();

	glm::vec3 getCheckPointPlayer();
	bool getNewCheckPoint();
	void setNewCheckPoint(bool b);

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


	std::unordered_map<char, AssimpModel*> models = {};
	glm::vec3 centerCamera;
	glm::vec2 movementCamera;

	glm::vec3 checkpointPlayer;  // puede que no se use como checkpoint, sino que lo haga el scene

	vector<pair<bool, glm::vec2>> walls;
	vector<int> doors;

	bool bPlayerDead = false;
	bool bNewCheckPoint = false;
};


#endif // _TILE_MAP_INCLUDE

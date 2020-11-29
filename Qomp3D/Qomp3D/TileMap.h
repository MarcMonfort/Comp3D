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
	};

	vector<pair<bool, glm::vec2>> getWalls() const;
	bool lineCollision(glm::vec3 pos, glm::vec3 size, bool vertical);

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

	std::unordered_map<char, AssimpModel*> models = {};
	glm::vec3 camCenter;
	glm::vec2 camMovement;

	vector<pair<bool, glm::vec2>> walls;
	vector<int> doors;
};


#endif // _TILE_MAP_INCLUDE

#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
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

	void render(ShaderProgram& program) const;
	void free();

	int getTileSize() const { return tileSize; }

	bool collisionMoveLeft(const glm::ivec3& pos, const glm::ivec3& size);
	bool collisionMoveRight(const glm::ivec3& pos, const glm::ivec3& size);
	bool collisionMoveDown(const glm::ivec3& pos, const glm::ivec3& size);
	bool collisionMoveUp(const glm::ivec3& pos, const glm::ivec3& size);

	enum block
	{
		basic,
		end,
		wall,
		key,
	};

	vector<pair<bool, glm::vec2>> getWalls() const;

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
	int* map;

	vector<AssimpModel*> models;
	glm::vec3 camCenter;
	glm::vec2 camMovement;

	vector<pair<bool, glm::vec2>> walls;
};


#endif // _TILE_MAP_INCLUDE



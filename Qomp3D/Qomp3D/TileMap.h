#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include <unordered_map>
#include "Texture.h"
#include "ShaderProgram.h"
#include "AssimpModel.h"
#include "SoundManager.h"
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

	bool collisionMoveLeft(const glm::ivec3& pos, const glm::ivec3& size, int type = 0);
	bool collisionMoveRight(const glm::ivec3& pos, const glm::ivec3& size, int type = 0);
	bool collisionMoveDown(const glm::ivec3& pos, const glm::ivec3& size, int type = 0);
	bool collisionMoveUp(const glm::ivec3& pos, const glm::ivec3& size, int type = 0);

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
		x_space,
		broken_chain,
		none
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

	glm::vec3 getColorBackground();

	int getStyle();

private:
	bool loadLevel(const string& levelFile, ShaderProgram& program);
	//void prepareArrays(const glm::vec2& minCoords, ShaderProgram& program);
	int checkBlock(int block);
	bool treatCollision(int pos, int type);
	void loadModels(const unordered_map<char, string>& paths, ShaderProgram& program);

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


	std::unordered_map<char, string> original =
	{
		{'1', "models/cube10.obj"},
		{'f', "models/final.obj"},
		{'k', "models/key.obj"},
		{'d', "models/door.obj"},
		{'l', "models/hline3.obj"},
		{'m', "models/vline3.obj"},
		{'r', "models/spike_up.obj"},
		{'s', "models/spike_down.obj"},
		{'t', "models/spike_left.obj"},
		{'u', "models/spike_right.obj"},
		{'c', "models/checkpoint.obj"},
		{'C', "models/checkpoint2.obj"},
		{'j', "models/chain.obj"},
		{'q', "models/lock.obj"},
		{'2', "models/chain.obj"},
		{'3', "models/chain.obj"},
		{'4', "models/broken_chain.obj"},
		{'5', "models/broken_chain.obj"},
		{'7', "models/chain.obj"},
		{'8', "models/lock.obj"},
		{'6', "models/chain.obj"},
		{'9', "models/chain.obj"},
		{'(', "models/broken_chain.obj"},
		{')', "models/broken_chain.obj"},
	};

	std::unordered_map<char, string> water =
	{
		{'1', "models/water.obj"},
		{'f', "models/final.obj"},
		{'k', "models/key.obj"},
		{'d', "models/door.obj"},
		{'l', "models/hline3.obj"},
		{'m', "models/vline3.obj"},
		{'r', "models/water_spike_up.obj"},
		{'s', "models/water_spike_down.obj"},
		{'t', "models/water_spike_left.obj"},
		{'u', "models/water_spike_right.obj"},
		{'c', "models/water_checkpoint.obj"},
		{'C', "models/water_checkpoint2.obj"},
		{'j', "models/water_chain.obj"},
		{'q', "models/water_lock.obj"},
		{'2', "models/water_chain.obj"},
		{'3', "models/water_chain.obj"},
		{'4', "models/water_broken_chain.obj"},
		{'5', "models/water_broken_chain.obj"},
		{'7', "models/water_chain.obj"},
		{'8', "models/water_lock.obj"},
		{'6', "models/water_chain.obj"},
		{'9', "models/water_chain.obj"},
		{'(', "models/water_broken_chain.obj"},
		{')', "models/water_broken_chain.obj"},
	};

	std::unordered_map<char, string> mario =
	{
		{'1', "models/mario.obj"},
		{'f', "models/final.obj"},
		{'k', "models/key.obj"},
		{'d', "models/door.obj"},
		{'l', "models/hline3.obj"},
		{'m', "models/vline3.obj"},
		{'r', "models/mario_spike_up.obj"},
		{'s', "models/mario_spike_down.obj"},
		{'t', "models/mario_spike_left.obj"},
		{'u', "models/mario_spike_right.obj"},
		{'c', "models/mario_checkpoint.obj"},
		{'C', "models/mario_checkpoint2.obj"},
		{'j', "models/mario_chain.obj"},
		{'q', "models/mario_lock.obj"},
		{'2', "models/mario_chain.obj"},
		{'3', "models/mario_chain.obj"},
		{'4', "models/mario_broken_chain.obj"},
		{'5', "models/mario_broken_chain.obj"},
		{'7', "models/mario_chain.obj"},
		{'8', "models/mario_lock.obj"},
		{'6', "models/mario_chain.obj"},
		{'9', "models/mario_chain.obj"},
		{'(', "models/mario_broken_chain.obj"},
		{')', "models/mario_broken_chain.obj"},
	};

	std::unordered_map<char, string> minecraft =
	{
		{'1', "models/minecraft.obj"},
		{'f', "models/final.obj"},
		{'k', "models/key.obj"},
		{'d', "models/door.obj"},
		{'l', "models/hline3.obj"},
		{'m', "models/vline3.obj"},
		{'r', "models/minecraft_spike_up.obj"},
		{'s', "models/minecraft_spike_down.obj"},
		{'t', "models/minecraft_spike_left.obj"},
		{'u', "models/minecraft_spike_right.obj"},
		{'c', "models/minecraft_checkpoint.obj"},
		{'C', "models/minecraft_checkpoint2.obj"},
		{'j', "models/minecraft_chain.obj"},
		{'q', "models/minecraft_lock.obj"},
		{'2', "models/minecraft_chain.obj"},
		{'3', "models/minecraft_chain.obj"},
		{'4', "models/minecraft_broken_chain.obj"},
		{'5', "models/minecraft_broken_chain.obj"},
		{'7', "models/minecraft_chain.obj"},
		{'8', "models/minecraft_lock.obj"},
		{'6', "models/minecraft_chain.obj"},
		{'9', "models/minecraft_chain.obj"},
		{'(', "models/minecraft_broken_chain.obj"},
		{')', "models/minecraft_broken_chain.obj"},
	};

	std::unordered_map<char, string> box =
	{
		{'1', "models/box.obj"},
		{'f', "models/final.obj"},
		{'k', "models/key.obj"},
		{'d', "models/door.obj"},
		{'l', "models/hline3.obj"},
		{'m', "models/vline3.obj"},
		{'r', "models/box_spike_up.obj"},
		{'s', "models/box_spike_down.obj"},
		{'t', "models/box_spike_left.obj"},
		{'u', "models/box_spike_right.obj"},
		{'c', "models/box_checkpoint.obj"},
		{'C', "models/box_checkpoint2.obj"},
		{'j', "models/box_chain.obj"},
		{'q', "models/box_lock.obj"},
		{'2', "models/box_chain.obj"},
		{'3', "models/box_chain.obj"},
		{'4', "models/box_broken_chain.obj"},
		{'5', "models/box_broken_chain.obj"},
		{'7', "models/box_chain.obj"},
		{'8', "models/box_lock.obj"},
		{'6', "models/box_chain.obj"},
		{'9', "models/box_chain.obj"},
		{'(', "models/box_broken_chain.obj"},
		{')', "models/box_broken_chain.obj"},
	};

	std::unordered_map<char, string>* paths;// = &water;

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

	glm::vec3 colorBackground;


	FMOD::Sound* basic_sound;
	FMOD::Sound* death_sound;
	FMOD::Sound* checkpoint_sound;
	FMOD::Sound* chain_sound;
	FMOD::Sound* key_sound;

	FMOD::Channel* channel;

	int style;

	float miau;
};


#endif // _TILE_MAP_INCLUDE

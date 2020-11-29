#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"
#include <glm/gtc/matrix_transform.hpp>
#include "PlayGameState.h"


using namespace std;


TileMap* TileMap::createTileMap(const string& levelFile, const glm::vec2& minCoords, ShaderProgram& program)
{
	TileMap* map = new TileMap(levelFile, minCoords, program);

	return map;
}


TileMap::TileMap(const string& levelFile, const glm::vec2& minCoords, ShaderProgram& program)
{
	loadLevel(levelFile, program);
	//prepareArrays(minCoords, program); No hace falta
}

TileMap::~TileMap()
{
	if (map != NULL)
		delete map;
}


void TileMap::render(ShaderProgram& program) const
{
	/*glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * mapSize.x * mapSize.y);
	glDisable(GL_TEXTURE_2D);*/

	glm::mat4 modelMatrix;
	modelMatrix = glm::mat4(1.0f);


	int tile;

	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			tile = map[j * mapSize.x + i];
			if (tile != 0)
			{
				modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(i,-j,0.f) );
				program.setUniformMatrix4f("model", modelMatrix);
				models[tile-1]->render(program);
			}
		}
	}
}

void TileMap::free()
{
	glDeleteBuffers(1, &vbo);
}

bool TileMap::loadLevel(const string& levelFile, ShaderProgram& program)
{
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	char tile;

	fin.open(levelFile.c_str());
	if (!fin.is_open())
		return false;
	getline(fin, line);
	if (line.compare(0, 7, "TILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> mapSize.x >> mapSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> camCenter.x >> camCenter.y >> camCenter.z;
	getline(fin, line);
	sstream.str(line);
	sstream >> camMovement.x >> camMovement.y;

	int numOfModels;
	getline(fin, line);
	sstream.str(line);
	sstream >> numOfModels;

	for (int i = 0; i < numOfModels; ++i)
	{
		getline(fin, line);
		sstream.str(line);
		sstream >> tilesheetFile;

		AssimpModel* new_model = new AssimpModel();
		new_model->loadFromFile(tilesheetFile, program);
		models.push_back(new_model);
	}
	
	map = new int[mapSize.x * mapSize.y];
	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			fin.get(tile);
			if (tile == ' ')
				map[j * mapSize.x + i] = 0;
			else if (tile == 'v')	// v for vertica wall
			{
				walls.push_back({ true, glm::vec2(i, j) });
				map[j * mapSize.x + i] = 0;
			}
			else if (tile == 'h')	// h for horizonatl wall
			{
				walls.push_back({ false, glm::vec2(i, j) });
				map[j * mapSize.x + i] = 0;
			}
			else
				map[j * mapSize.x + i] = tile - int('0');
		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();

	return true;
}

//void TileMap::prepareArrays(const glm::vec2& minCoords, ShaderProgram& program)
//{
//	int tile, nTiles = 0;
//	glm::vec2 posTile, texCoordTile[2], halfTexel;
//	vector<float> vertices;
//
//	halfTexel = glm::vec2(0.5f / tilesheet.width(), 0.5f / tilesheet.height());
//	for (int j = 0; j < mapSize.y; j++)
//	{
//		for (int i = 0; i < mapSize.x; i++)
//		{
//			tile = map[j * mapSize.x + i];
//			if (tile != 0)
//			{
//				// Non-empty tile
//				nTiles++;
//				posTile = glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);
//				texCoordTile[0] = glm::vec2(float((tile - 1) % 2) / tilesheetSize.x, float((tile - 1) / 2) / tilesheetSize.y);
//				texCoordTile[1] = texCoordTile[0] + tileTexSize;
//				//texCoordTile[0] += halfTexel;
//				texCoordTile[1] -= halfTexel;
//				// First triangle
//				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
//				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
//				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y);
//				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
//				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
//				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
//				// Second triangle
//				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
//				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
//				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
//				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
//				vertices.push_back(posTile.x); vertices.push_back(posTile.y + blockSize);
//				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
//			}
//		}
//	}
//
//	glGenVertexArrays(1, &vao);
//	glBindVertexArray(vao);
//	glGenBuffers(1, &vbo);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
//	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
//	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//}

// Collision tests for axis aligned bounding boxes.
// Method collisionMoveDown also corrects Y coordinate if the box is
// already intersecting a tile below.

bool TileMap::collisionMoveLeft(const glm::ivec3& pos, const glm::ivec3& size)
{
	int x, y0, y1;

	x = pos.x;
	y0 = pos.y;
	y1 = (pos.y + size.y);
	for (int y = y0; y <= y1; y++)
	{
		if (map[y * mapSize.x + x] != 0)
			//return true;
			return treatCollision(y * mapSize.x + x, 0);
	}

	return false;
}

bool TileMap::collisionMoveRight(const glm::ivec3& pos, const glm::ivec3& size)
{
	int x, y0, y1;

	// tileSize  == 1
	x = (pos.x + size.x);
	y0 = pos.y;
	y1 = (pos.y + size.y);
	for (int y = y0; y <= y1; y++)
	{
		if (map[y * mapSize.x + x] != 0)
			//return true;
			return treatCollision(y * mapSize.x + x, 0);
	}

	return false;
}


bool TileMap::collisionMoveDown(const glm::ivec3& pos, const glm::ivec3& size)
{
	int x0, x1, y;

	x0 = pos.x;
	x1 = (pos.x + size.x);
	y = (pos.y + size.y);
	for (int x = x0; x <= x1; x++)
	{
		if (map[y * mapSize.x + x] != 0)
			//return true;
			return treatCollision(y * mapSize.x + x, 0);
	}

	return false;
}

bool TileMap::collisionMoveUp(const glm::ivec3& pos, const glm::ivec3& size)
{
	int x0, x1, y;

	x0 = pos.x;
	x1 = (pos.x + size.x);
	y = pos.y;
	for (int x = x0; x <= x1; x++)
	{
		if (map[y * mapSize.x + x] != 0)
			//return true;
			return treatCollision(y * mapSize.x + x, 0);
	}

	return false;
}

int TileMap::checkBlock(int block)
{
	if (block == 1)
		return basic;
	else if (block == 2)
		return end;
	else if (block == 'v' || block == 'h')
		return wall;
	else if (block == 'k')
		return key;
}

bool TileMap::treatCollision(int pos, int type)
{
	int block = checkBlock(map[pos]);

	if (block == basic)
	{
		return true;
		/*if (map[pos] % 2 == 0)
			map[pos - 1] = 0;
		else
			map[pos + 1] = 0;
		map[pos] = 0;*/

		/*if (type == 0)
			channel = soundManager->playSound(music_basic_block);
		else {
			channel = soundManager->playSound(drop_block2);
			channel->setVolume(0.3f);
		}*/

		//PlayGameState::instance().addPoints(100);
	}
	else if (block == wall) {
		/*if (type == 0)
			channel = soundManager->playSound(music_wall_coin);
		else {
			channel = soundManager->playSound(drop_block2);
			channel->setVolume(0.3f);
		}*/
		return true;
	}
	else if (block == key)
	{
		/*if (map[pos] == 23) {
			map[pos + 1] = 0;
			map[pos + mapSize.x] = 0;
			map[pos + mapSize.x + 1] = 0;
		}
		else if (map[pos] == 24) {
			map[pos - 1] = 0;
			map[pos + mapSize.x] = 0;
			map[pos + mapSize.x - 1] = 0;
		}
		else if (map[pos] == 39) {
			map[pos + 1] = 0;
			map[pos - mapSize.x] = 0;
			map[pos - mapSize.x + 1] = 0;
		}
		else if (map[pos] == 40) {
			map[pos - 1] = 0;
			map[pos - mapSize.x] = 0;
			map[pos - mapSize.x - 1] = 0;
		}
		map[pos] = 0;
		map[8] = 21;
		map[9] = 22;
		map[10] = 21;
		map[11] = 22;
		map[12] = 21;
		map[13] = 22;
		map[14] = 21;
		map[15] = 22;
		map[pos] = 0;*/

		/*if (type == 0)
			channel = soundManager->playSound(music_wall_coin);
		else {
			channel = soundManager->playSound(drop_block2);
			channel->setVolume(0.3f);
		}*/
		return false;
	}
	else if (block == end)
	{
		PlayGameState::instance().finalBlockTaken();
		return false;
	}
	//prepareArrays(a, b);
	//return true;
}


/// END COLLISION

vector<pair<bool, glm::vec2>> TileMap::getWalls() const
{
	return walls;
}

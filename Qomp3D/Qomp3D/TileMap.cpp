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


void TileMap::render(ShaderProgram& program)
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


	char tile;

	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			tile = map[j * mapSize.x + i];
			if (tile != '0')
			{
				unordered_map<char, AssimpModel*>::const_iterator it = models.find(tile);
				if (it != models.end()) {
					modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(i, -j, 0.f));
					program.setUniformMatrix4f("model", modelMatrix);
					it->second->render(program);
				}
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

	char num;
	for (int i = 0; i < numOfModels; ++i)
	{
		getline(fin, line);
		sstream.str(line);
		sstream >> num >> tilesheetFile;

		AssimpModel* new_model = new AssimpModel();
		new_model->loadFromFile(tilesheetFile, program);
		models.insert(pair<int, AssimpModel*>(num, new_model));
	}
	
	map = new char[mapSize.x * mapSize.y];
	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			fin.get(tile);
			if (tile == ' ')
				map[j * mapSize.x + i] = '0';
			else if (tile == 'v')	// v for vertical wall
			{
				walls.push_back({ true, glm::vec2(i, j) });
				map[j * mapSize.x + i] = '0';
			}
			else if (tile == 'h')	// h for horizontal wall
			{
				walls.push_back({ false, glm::vec2(i, j) });
				map[j * mapSize.x + i] = '0';
			}
			else if (tile == 'd')	// d for door
			{
				map[j * mapSize.x + i] = tile;
				doors.push_back(j * mapSize.x + i);
			}
			else if (tile == 'b')	// button
			{
				buttons.push_back({ false, glm::vec2(i, j) });
				map[j * mapSize.x + i] = '0';
			}
			else if (tile == 'y')	// yes swicth
			{
				switchs.push_back({ true, glm::vec2(i, j) });
				map[j * mapSize.x + i] = '0';
			}
			else if (tile == 'n')	// no switch
			{
				switchs.push_back({ false, glm::vec2(i, j) });
				map[j * mapSize.x + i] = '0';
			}
			else
				map[j * mapSize.x + i] = tile;
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
		if (map[y * mapSize.x + x] != '0')
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
		if (map[y * mapSize.x + x] != '0')
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
		if (map[y * mapSize.x + x] != '0')
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
		if (map[y * mapSize.x + x] != '0')
			return treatCollision(y * mapSize.x + x, 0);
	}

	return false;
}

int TileMap::checkBlock(int block)
{
	if (block == 1)
		return basic;
	else if (block == 'f')
		return fin;
	else if (block == 'k')
		return key;
	else if (block == 'd')
		return door;
	else if (block == 'l' || block == 'm')
		return line;
}

bool TileMap::treatCollision(int pos, int type)
{
	int block = checkBlock(map[pos]);

	if (block == basic)
	{
		return true;
	}
	else if (block == key)
	{
		map[pos] = '0';
		for (int i = 0; i < doors.size(); ++i) {
			map[doors[i]] = '0';
		}
		return false;
	}
	else if (block == fin)
	{
		PlayGameState::instance().finalBlockTaken();
		return false;
	}
	else if (block == door)
	{
		return true;
	}
	else if (block == line)
	{
		return false;
	}
	return true;
}

bool TileMap::lineCollision(glm::vec3 pos, glm::vec3 size, bool vertical)
{
	int x0 = pos.x, x1 = pos.x + size.x - 0.05, y0 = pos.y, y1 = pos.y + size.y - 0.05;
	bool c1, c2;

	if (vertical) {
		c1 = map[y0 * mapSize.x + x0] == 'm' && map[y0 * mapSize.x + x1] == 'm';
		c2 = map[y1 * mapSize.x + x0] == 'm' && map[y1 * mapSize.x + x1] == 'm';
	}
	else {
		c1 = map[y0 * mapSize.x + x0] == 'l' && map[y1 * mapSize.x + x0] == 'l';
		c2 = map[y0 * mapSize.x + x1] == 'l' && map[y1 * mapSize.x + x1] == 'l';
	}

	return c1 || c2;
}

/// END COLLISION

vector<pair<bool, glm::vec2>> TileMap::getWalls() const
{
	return walls;
}

vector<pair<bool, glm::vec2>> TileMap::getButtons() const
{
	return buttons;
}

vector<pair<bool, glm::vec2>> TileMap::getSwitchs() const
{
	return switchs;
}

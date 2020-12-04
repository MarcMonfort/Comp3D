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
	currentTime = 0.0f;
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
	//glm::mat3 normalMatrix;
	modelMatrix = glm::mat4(1.0f);


	char tile;

	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			tile = map[j * mapSize.x + i];
			if (tile != '0' && tile != 'x')   // x for activating close thing
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

void TileMap::update(int deltaTime)
{
	currentTime += deltaTime;

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
	sstream >> roomSize.x >> roomSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> centerCamera.x >> centerCamera.y >> centerCamera.z;
	getline(fin, line);
	sstream.str(line);
	sstream >> movementCamera.x >> movementCamera.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> checkpointPlayer.x >> checkpointPlayer.y >> checkpointPlayer.z;


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
			else if (tile == 'v')	// v for easy vertical wall
			{
				Wall wall;
				wall.position = glm::vec2(i, j);
				wall.bVertical = true;
				wall.type = 0;
				walls.push_back(wall);
				map[j * mapSize.x + i] = '0';
			}
			else if (tile == 'V')	// v for hard vertical wall
			{
				Wall wall;
				wall.position = glm::vec2(i, j);
				wall.bVertical = true;
				wall.type = 1;
				walls.push_back(wall);
				map[j * mapSize.x + i] = '0';
			}
			else if (tile == '|')	// v for impossible vertical wall
			{
				Wall wall;
				wall.position = glm::vec2(i, j);
				wall.bVertical = true;
				wall.type = 2;
				walls.push_back(wall);
				map[j * mapSize.x + i] = '0';
			}
			else if (tile == 'h')	// h for easy horizontal wall
			{
				Wall wall;
				wall.position = glm::vec2(i, j);
				wall.bVertical = false;
				wall.type = 0;
				walls.push_back(wall);
				map[j * mapSize.x + i] = '0';
			}
			else if (tile == 'H')	// h for hard horizontal wall
			{
				Wall wall;
				wall.position = glm::vec2(i, j);
				wall.bVertical = false;
				wall.type = 1;
				walls.push_back(wall);
				map[j * mapSize.x + i] = '0';
			}
			else if (tile == '-')	// h for impossible horizontal wall
			{
				Wall wall;
				wall.position = glm::vec2(i, j);
				wall.bVertical = false;
				wall.type = 2;
				walls.push_back(wall);
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
	else if (block == 's')
		return spike;
	else if (block == 'c')
		return checkpoint;
	else if (block == 'C')
		return checkpoint2;
	else if (block == 'x')
		return x_space;
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
		//sonido
		return true;
	}
	else if (block == line)
	{
		return false;
	}
	else if (block == spike)
	{
		if (!PlayGameState::instance().getGodMode())
		{
			bPlayerDead = true;
			return false;
		}	
	}
	else if (block == checkpoint)
	{
		bNewCheckPoint = true;
		for (int j = 0; j < mapSize.y; j++)
			for (int i = 0; i < mapSize.x; i++)
				if (map[j * mapSize.x + i] == 'C')
					map[j * mapSize.x + i] = '0';

		map[pos] = 'C';

		checkpointPlayer.y = pos / mapSize.x;
		checkpointPlayer.x = pos % mapSize.x;

		return false;
	}
	else if (block == checkpoint2)
	{
		return false;
	}
	else if (block == x_space)
	{
		if (map[pos + 1] == 'c')
			return treatCollision(pos + 1, type);
		else if (map[pos - 1] == 'c')
			return treatCollision(pos - 1, type);

		if (map[pos + mapSize.x] == 'c')
			return treatCollision(pos + mapSize.x, type);
		else if (map[pos + mapSize.x + 1] == 'c')
			return treatCollision(pos + mapSize.x + 1, type);
		else if (map[pos + mapSize.x - 1] == 'c')
			return treatCollision(pos + mapSize.x - 1, type);

		else if (map[pos - mapSize.x] == 'c')
			return treatCollision(pos - mapSize.x, type);
		else if (map[pos - mapSize.x + 1] == 'c')
			return treatCollision(pos - mapSize.x + 1, type);
		else if (map[pos - mapSize.x - 1] == 'c')
			return treatCollision(pos - mapSize.x - 1, type);

		else
			map[pos] = '0';
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

vector<TileMap::Wall> TileMap::getWalls()
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

bool TileMap::getPlayerDead()
{
	return bPlayerDead;
}

void TileMap::setPlayerDead(bool bDead)
{
	bPlayerDead = bDead;
}


glm::vec3 TileMap::getCenterCamera()
{
	return centerCamera;
}

glm::vec2 TileMap::getMovementCamera()
{
	return movementCamera;
}

glm::vec3 TileMap::getCheckPointPlayer()
{
	return checkpointPlayer;
}

bool TileMap::getNewCheckPoint()
{
	return bNewCheckPoint;
}

void TileMap::setNewCheckPoint(bool b)
{
	bNewCheckPoint = b;
}


glm::vec2 TileMap::getRoomSize()
{
	return roomSize;
}

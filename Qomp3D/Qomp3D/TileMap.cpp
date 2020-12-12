#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"
#include <glm/gtc/matrix_transform.hpp>
#include "PlayGameState.h"
#include <math.h>


using namespace std;

# define M_PI           3.14159265358979323846  /* pi */


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


void TileMap::render(ShaderProgram& program, const glm::ivec3& posPlayer)
{

	glm::mat4 modelMatrix;
	//glm::mat3 normalMatrix;
	modelMatrix = glm::mat4(1.0f);


	char tile;

	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			if (abs(i - posPlayer.x) <= movementCamera.x+2 && abs(j - posPlayer.y) <= movementCamera.y+2)
			{

				tile = map[j * mapSize.x + i];
				if (tile != ' ' && tile != 'x')
				{
					unordered_map<char, AssimpModel*>::const_iterator it = models.find(tile);

					// Si el model encara no ha estat creat (millor fer-ho abans)
					if (it == models.end()) {
						AssimpModel* new_model = new AssimpModel();
						string path = (paths->find(tile))->second;
						new_model->loadFromFile(path, program);
						models.insert(pair<char, AssimpModel*>(tile, new_model));
						it = models.find(tile);
					}

					// Es renderitza el model a la posició corresponent
					modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(i, -j, 0.f));
					if (it->first == 'j' || it->first == 'q')
						modelMatrix = glm::rotate(modelMatrix, float((M_PI / 4.0f)), glm::vec3(-1, 0, 0));
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
	getline(fin, line);

	int style;
	sstream.str(line);
	sstream >> style;
	switch (style)
	{
	case 0:
		paths = &original;
		colorBackground = glm::vec3(0);
		break;
	case 1:
		paths = &water;
		colorBackground = glm::vec3(0,0,0.2);
		break;
	case 2:
		paths = &box;
		colorBackground = glm::vec3(0.74, 0.60, 0.47);
		break;
	case 3:
		paths = &mario;
		colorBackground = glm::vec3(0, 0.54, 0.78);
		break;
	case 4:
		paths = &minecraft;
		colorBackground = glm::vec3(0);
		break;
	}
	
	map = new char[mapSize.x * mapSize.y];
	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			fin.get(tile);

			switch (tile) {

				case('0'):
					map[j * mapSize.x + i] = ' ';
					break;

				case('v'): {		// easy vertical wall
					Wall wall;
					wall.position = glm::vec2(i, j);
					wall.bVertical = true;
					wall.type = 0;
					walls.push_back(wall);
					map[j * mapSize.x + i] = ' ';
					break; }

				case('V'): {		// hard vertical wall
					Wall wall;
					wall.position = glm::vec2(i, j);
					wall.bVertical = true;
					wall.type = 1;
					walls.push_back(wall);
					map[j * mapSize.x + i] = ' ';
					break; }

				case('|'): {		// impossible vertical wall
					Wall wall;
					wall.position = glm::vec2(i, j);
					wall.bVertical = true;
					wall.type = 2;
					walls.push_back(wall);
					map[j * mapSize.x + i] = ' ';
					break; }

				case('h'): {		// easy horizontal wall
					Wall wall;
					wall.position = glm::vec2(i, j);
					wall.bVertical = false;
					wall.type = 0;
					walls.push_back(wall);
					map[j * mapSize.x + i] = ' ';
					break; }

				case('H'): {		// hard horizontal wall
					Wall wall;
					wall.position = glm::vec2(i, j);
					wall.bVertical = false;
					wall.type = 1;
					walls.push_back(wall);
					map[j * mapSize.x + i] = ' ';
					break; }

				case('-'): {		// impossible horizontal wall
					Wall wall;
					wall.position = glm::vec2(i, j);
					wall.bVertical = false;
					wall.type = 2;
					walls.push_back(wall);
					map[j * mapSize.x + i] = ' ';
					break; }

				case('o'):			// easy vertical ballSpike
					ballSpikes.push_back({ true, glm::vec2(i, j) });
					map[j * mapSize.x + i] = ' ';
					break;

				case('O'):			// horizontal ballSpike
					ballSpikes.push_back({ false, glm::vec2(i, j) });
					map[j * mapSize.x + i] = ' ';
					break;

				case('d'):		// door
				case('j'):		// chain
				case('q'):		// lock
					map[j * mapSize.x + i] = tile;
					doors.push_back(j * mapSize.x + i);
					break;

				case('U'):		// button up
					buttons.push_back({ false, glm::vec2(i, j), up });
					map[j * mapSize.x + i] = ' ';
					break;

				case('D'):		// button down
					buttons.push_back({ false, glm::vec2(i, j), down });
					map[j * mapSize.x + i] = ' ';
					break;

				case('R'):		// button right
					buttons.push_back({ false, glm::vec2(i, j), right });
					map[j * mapSize.x + i] = ' ';
					break;

				case('L'):		// button left
					buttons.push_back({ false, glm::vec2(i, j), left });
					map[j * mapSize.x + i] = ' ';
					break;

				case('y'):		// yes swicth
					switchs.push_back({ true, glm::vec2(i, j) });
					map[j * mapSize.x + i] = ' ';
					break;

				case('n'):		// no switch
					switchs.push_back({ false, glm::vec2(i, j) });
					map[j * mapSize.x + i] = ' ';
					break;

				default:
					map[j * mapSize.x + i] = tile;
					break;
			}
		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();

	return true;
}


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
		if (map[y * mapSize.x + x] != ' ')
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
		if (map[y * mapSize.x + x] != ' ')
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
		if (map[y * mapSize.x + x] != ' ')
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
		if (map[y * mapSize.x + x] != ' ')
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
	else if (block == 'r' || block == 's' || block == 't' || block == 'u')
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
		map[pos] = ' ';
		for (int i = 0; i < doors.size(); ++i) {
			map[doors[i]] = ' ';
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
					map[j * mapSize.x + i] = ' ';

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
			map[pos] = ' ';
		return false;
	}


	return true;
}

bool TileMap::lineCollision(glm::vec3 &pos, glm::vec3 size, bool vertical)
{
	int x0 = pos.x + 0.1, x1 = pos.x + size.x - 0.1, y0 = pos.y + 0.1, y1 = pos.y + size.y - 0.1;
	bool c1, c2;

	if (vertical) {
		c1 = map[y0 * mapSize.x + x0] == 'm' && map[y0 * mapSize.x + x1] == 'm';
		c2 = map[y1 * mapSize.x + x0] == 'm' && map[y1 * mapSize.x + x1] == 'm';
		if (c1 || c2)
			pos = glm::vec3(floor(x0), pos.y, pos.z);
	}
	else {
		c1 = map[y0 * mapSize.x + x0] == 'l' && map[y1 * mapSize.x + x0] == 'l';
		c2 = map[y0 * mapSize.x + x1] == 'l' && map[y1 * mapSize.x + x1] == 'l';
		if (c1 || c2)
			pos = glm::vec3(pos.x, floor(y0), pos.z);
	}

	return c1 || c2;
}

/// END COLLISION

vector<TileMap::Wall> TileMap::getWalls()
{
	return walls;
}

vector<tuple<bool, glm::vec2, int>> TileMap::getButtons() const
{
	return buttons;
}

vector<pair<bool, glm::vec2>> TileMap::getSwitchs() const
{
	return switchs;
}

vector<pair<bool, glm::vec2>> TileMap::getBallSpikes() const
{
	return ballSpikes;
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


glm::vec3 TileMap::getColorBackground()
{
	return colorBackground;
}
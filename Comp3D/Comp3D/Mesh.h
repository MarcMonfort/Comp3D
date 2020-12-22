#ifndef _MESH_INCLUDE
#define _MESH_INCLUDE


#include <vector>
#include "glm/glm.hpp"


using namespace std;


struct Mesh
{
	int textureIndex;
	vector<glm::vec3> vertices, normals;
	vector<glm::vec2> texCoords;
	vector<unsigned int> triangles;
};


#endif // _MESH_INCLUDE


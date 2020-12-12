#ifndef _ASSIMP_MODEL_INCLUDE
#define _ASSIMP_MODEL_INCLUDE


#include <vector>
#include "glm/glm.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Mesh.h"
#include "Texture.h"
#include "ShaderProgram.h"


using namespace std;


class AssimpModel
{
public:
	AssimpModel();
	~AssimpModel();

	bool loadFromFile(const string &filename, ShaderProgram &program);
	void render(ShaderProgram &program) const;

	glm::vec3 getCenter() const;
	glm::vec3 getSize() const;

private:
	void clear();
	bool initFromScene(const aiScene *pScene, const string &filename);
	void initMesh(int index, const aiMesh *paiMesh);
	bool initMaterials(const aiScene *pScene, const string &filename);
	void computeBoundingBox();
	void prepareArrays(ShaderProgram &program);

private:
	glm::vec3 size;
	glm::vec3 center, bbox[2];
	vector<Mesh *> meshes;
	vector<Texture *> textures;

	vector<GLuint> VAOs;
	vector<GLuint> VBOs;
	vector<GLint> posLocations, normalLocations, texCoordLocations;

	Texture floor;
};


#endif // _ASSIMP_MODEL_INCLUDE


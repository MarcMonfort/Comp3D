#ifndef _SIMPLEOBJECT_INCLUDE
#define _SIMPLEOBJECT_INCLUDE


#include "ShaderProgram.h"


// Given a point (x, y) and a size (width, height) in pixels Quad creates 
// a VBO with two triangles. Vertices in the VBO have only a position
// attribute (no colors or texture coordinates)


class SimpleObject
{

public:
	// Quads can only be created inside an OpenGL context
	static SimpleObject* createSimpleObject(float x, float y, float width, float height, ShaderProgram* program);

	SimpleObject(float x, float y, float width, float height, ShaderProgram* program);

	void render() const;
	void free();

	void setPosition(const glm::vec3& pos);
	glm::vec3 getPosition();

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation;

	ShaderProgram* shaderProgram;
	glm::vec3 position;

};


#endif
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include "SimpleObject.h"
#include <vector>


SimpleObject* SimpleObject::createSimpleObject(float x, float y, float width, float height, ShaderProgram* program)
{
	SimpleObject* quad = new SimpleObject(x, y, width, height, program);

	return quad;
}


SimpleObject::SimpleObject(float x, float y, float width, float height, ShaderProgram* program)
{
	

	vector<float> vertices;

	glm::vec3 size = glm::vec3(8, 4, 16);

	vertices.push_back(-size.x / 2.f); vertices.push_back(-size.y / 2.f); vertices.push_back(-size.z / 2.f);

	vertices.push_back(-size.x / 2.f);  vertices.push_back(-size.y / 2.f); vertices.push_back(size.z / 2.f);

	vertices.push_back(size.x / 2.f);  vertices.push_back(-size.y / 2.f); vertices.push_back(size.z / 2.f);

	vertices.push_back(size.x / 2.f);  vertices.push_back(-size.y / 2.f); vertices.push_back(-size.z / 2.f);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 15 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program->bindVertexAttribute("position", 3, 3 * sizeof(float), 0);

	shaderProgram = program;
	position = glm::vec3(0.f);

}

void SimpleObject::render() const
{
	// problema: no actualiza el lookAt
	/*glm::mat4 modelview = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z));
	shaderProgram->setUniformMatrix4f("modelview", modelview);*/

	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glDrawArrays(GL_QUADS, 0, 4);
}

void SimpleObject::free()
{
	glDeleteBuffers(1, &vbo);
}

void SimpleObject::setPosition(const glm::vec3& pos)
{
	position = pos;
}

glm::vec3 SimpleObject::getPosition()
{
	return position;
}
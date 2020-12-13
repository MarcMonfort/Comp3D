#ifndef _PARTICLE_SYSTEM_INCLUDE
#define _PARTICLE_SYSTEM_INCLUDE


#include <vector>
#include "Billboard.h"


class ParticleSystem
{
public:
	struct Particle
	{
		float lifetime;
		glm::vec3 position;
		glm::vec3 speed;
	};

public:
	ParticleSystem();
	~ParticleSystem();

	void init(const glm::vec2 &billboardQuadSize, ShaderProgram &program, const string &billboardTextureName, float gravity = 0.f, float fadeOut = 0.f);
	void addParticle(Particle &newParticle);

	void update(float deltaTimeInSeconds);
	void render(ShaderProgram& program, const glm::vec3 &eye);

	bool empty();

private:
	int numParticles;
	vector<Particle> particles;
	Billboard *billboard;
	float g;

	float fadeOut;
};


#endif // _PARTICLE_SYSTEM_INCLUDE


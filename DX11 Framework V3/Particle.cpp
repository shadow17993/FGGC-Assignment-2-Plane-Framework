#include "Particle.h"


Particle::Particle(Transform* transform, ParticleModel* particleModel, Appearance* appearance) : _transform(transform), _particlePhysics(particleModel), _drawablePhysics(appearance)
{
	_energy = rand() % 300;
}

Particle::Particle(Transform* transform, ParticleModel* particleModel) : _transform(transform), _particlePhysics(particleModel)
{
	_energy = rand() % 300;
}

Particle::~Particle()
{
}

void Particle::Update(float t)
{
	_transform->Update(t);
	_particlePhysics->UpdateParticle(t);

	// Reduces energy with each frame
	_energy--;
}

void Particle::Draw(ID3D11DeviceContext * pImmediateContext)
{
	_drawablePhysics->Draw(pImmediateContext);
}
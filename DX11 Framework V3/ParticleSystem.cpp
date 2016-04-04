#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(Transform* transform, XMFLOAT3 offsetPos, Appearance* particleApp) : _transform(transform), _offsetPos(offsetPos), _particleApp(particleApp)
{
	_transform->SetPosition(_transform->GetPosition().x + offsetPos.x,
							_transform->GetPosition().y + offsetPos.y,
							_transform->GetPosition().z + offsetPos.z);
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update(float dt)
{
	if (particles.size() != 100)
	{
		/*if (rand() % 10 + 1 > 5)*/
		if (GetAsyncKeyState('P') & 0x8000)
		{
			Transform* t = new Transform(nullptr, _transform->GetPosition());
			ParticleModel* pm = new ParticleModel(t, true, { 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f }, {0.0f, 1.0f, 0.0f});
			
			particles.push_back(new Particle(t, pm, _particleApp));
		}
	}

	for (int i = 0; i < particles.size(); i++)
	{
		particles.at(i)->Update(dt);

		// Erases particle if energy is 0;
		if (particles.at(i)->getEnergy() == 0)
		{
			particles.erase(particles.begin() + i);
		}
	}
}

void ParticleSystem::Draw(ID3D11DeviceContext * pImmediateContext)
{
	for (auto particle : particles)
	{
		particle->Draw(pImmediateContext);
	}
}
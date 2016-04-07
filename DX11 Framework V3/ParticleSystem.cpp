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
	_transform->Update(dt);
	//_transform->SetPosition(_transform->getParent()->GetPosition().x, _transform->getParent()->GetPosition().y, _transform->getParent()->GetPosition().z + 13);
	

	if (particles.size() != 100)
	{
		Transform* t = new Transform(nullptr, _transform->GetPosition(), { 0.0f, 0.0f, 0.0f }, {0.3f, 0.3f, 0.3f});
		ParticleModel* pm = new ParticleModel(t, true, { -2.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2.0 - -2.0)),
			-1.0f,
			-2.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2.0 - -2.0)) }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
			
		particles.push_back(new Particle(t, pm, _particleApp));
		
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
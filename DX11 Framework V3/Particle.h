#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "ParticleModel.h"
#include "Appearance.h"
#include <string>

using namespace DirectX;

class Particle
{
public:
	Particle(Transform* transform, ParticleModel* particleModel, Appearance* appearance);
	Particle(Transform* transform, ParticleModel* particleModel);
	~Particle();

	Transform * GetTransform() const { return _transform; }
	ParticleModel * GetParticleModel() const { return _particlePhysics; }
	Appearance * GetAppearance() const { return _drawablePhysics; }

	void setEnergy(int energy){ _energy = energy; }
	int getEnergy(){ return _energy; }

	void Update(float t);
	void Draw(ID3D11DeviceContext * pImmediateContext);

private:

	Transform* _transform;
	ParticleModel* _particlePhysics;
	Appearance* _drawablePhysics;

	int _energy;
};


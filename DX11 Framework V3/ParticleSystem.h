#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "Particle.h"
#include <vector>
#include "GameObject.h"

using namespace DirectX;
using namespace std;

class ParticleSystem
{
public:
	ParticleSystem(Transform* transform, XMFLOAT3 offsetPos, Appearance* particleApp);
	~ParticleSystem();

	Transform* getTransform(){ return _transform; }
	Appearance* getAppearance(){ return _particleApp; }


	void Update(float t);
	void Draw(ID3D11DeviceContext * pImmediateContext);

private:

	vector<Particle*> particles;

	Transform* _transform;
	XMFLOAT3 _offsetPos;
	Appearance* _particleApp;
};


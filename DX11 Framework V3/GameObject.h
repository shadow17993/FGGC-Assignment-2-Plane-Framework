#pragma once

#include <directxmath.h>
#include <d3d11_1.h>
#include <string>
#include "Transform.h"
#include "ParticleModel.h"
#include "Appearance.h"

using namespace DirectX;
using namespace std;



class GameObject
{
public:
	GameObject(string type, Transform* transform, ParticleModel* particleModel, Appearance* appearance);
	GameObject(string type, Transform* transform, Appearance* appearance);
	~GameObject();

	string GetType() const { return _type; }

	Transform * GetTransform() const { return _transform; }
	ParticleModel * GetParticleModel() const { return _particleModel; }
	Appearance * GetAppearance() const { return _appearance; }

	void Update(float t);
	void Draw(ID3D11DeviceContext * pImmediateContext);

private:

	Transform * _transform;
	ParticleModel * _particleModel;
	Appearance * _appearance;

	string _type;

	bool isMoving;
};


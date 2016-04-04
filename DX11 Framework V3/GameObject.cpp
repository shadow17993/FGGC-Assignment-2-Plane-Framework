#include "GameObject.h"

GameObject::GameObject(string type, Transform* transform, ParticleModel* particleModel, Appearance* appearance) : _type(type), _transform(transform), _particleModel(particleModel), _appearance(appearance)
{
	isMoving = true;
}

GameObject::GameObject(string type, Transform* transform, Appearance* appearance) : _type(type), _transform(transform), _appearance(appearance)
{
	isMoving = false;
	_particleModel = nullptr;
}

GameObject::~GameObject()
{
}

void GameObject::Update(float t)
{
	_transform->Update(t);

	if (isMoving)
	{
		if (_type == "Plane")
		{
			_particleModel->UpdatePlane(t);
		}
	}
}

void GameObject::Draw(ID3D11DeviceContext * pImmediateContext)
{
	_appearance->Draw(pImmediateContext);
}

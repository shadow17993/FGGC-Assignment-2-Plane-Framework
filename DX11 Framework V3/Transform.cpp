#include "Transform.h"


Transform::Transform()
{
	_parent = nullptr;
	_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

Transform::Transform(Transform* parent)
{
	_parent = parent;
	_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

Transform::Transform(Transform* parent, XMFLOAT3 position)
{
	_parent = parent;
	_position = position;
	_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

Transform::Transform(Transform* parent, XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
{
	_parent = parent;
	_position = position;
	_rotation = rotation;
	_scale = scale;
}

Transform::~Transform()
{
}


void Transform::Update(float t)
{
	// Calculate world matrix
	XMMATRIX scale = XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	XMMATRIX rotation = XMMatrixRotationX(_rotation.x) * XMMatrixRotationY(_rotation.y) * XMMatrixRotationZ(_rotation.z);
	XMMATRIX translation = XMMatrixTranslation(_position.x, _position.y, _position.z);

	XMStoreFloat4x4(&_world, scale * rotation * translation);

	if (_parent != nullptr)
	{
		XMStoreFloat4x4(&_world, this->GetWorldMatrix() * _parent->GetWorldMatrix());
	}

	_lPosition = GetPosition();
}
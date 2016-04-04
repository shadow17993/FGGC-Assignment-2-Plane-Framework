#include "ParticleModel.h"

ParticleModel::ParticleModel(Transform* transform, bool useConstVel, XMFLOAT3 initVel, XMFLOAT3 initAccel) : _transform(transform)
{
	_isConstVel = useConstVel;
	_isSpinConstVel = true;

	_velocity = initVel;
	_acceleration = initAccel;
	_spinVelocity = { 0.0f, 0.0f, 0.0f };
	_spinAccel = { 0.0f, 0.0f, 0.0f };

	_thrust = { 0.0f, 0.0f, 0.0f };
	_brake = { 0.0f, 0.0f, 0.0f };
	_friction = { 0.0f, 0.0f, 0.0f };
	_netForce = { 0.0f, 0.0f, 0.0f };

	_mass = 1.0f;
	_gravity = -9.8f;
	_weight = _mass * _gravity;
	_initPos = _transform->GetPosition();

	_radius = 1.0f;
}

ParticleModel::ParticleModel(Transform* transform, bool useConstVel, XMFLOAT3 initVel, XMFLOAT3 initAccel, XMFLOAT3 thrust) : _transform(transform)
{
	_isConstVel = useConstVel;
	_isSpinConstVel = true;

	_velocity = initVel;
	_acceleration = initAccel;
	_spinVelocity = { 0.0f, 0.0f, 0.0f };
	_spinAccel = { 0.0f, 0.0f, 0.0f };

	_thrust = thrust;
	_brake = { 0.0f, 0.0f, 0.0f };
	_friction = { 0.0f, 0.0f, 0.0f };
	_netForce = { 0.0f, 0.0f, 0.0f };

	_mass = 1.0f;
	_gravity = -9.8f;
	_weight = _mass * _gravity;
	_initPos = _transform->GetPosition();

	_radius = 1.0f;
}


ParticleModel::~ParticleModel()
{
}


void ParticleModel::Move(float x, float y, float z)
{
	XMFLOAT3 position = _transform->GetPosition();

	position.x += x;
	position.y += y;
	position.z += z;

	_transform->SetPosition(position);
}


//					Movement				//
void ParticleModel::moveConstVel(float t)
{
	_transform->SetPosition(_transform->GetPosition().x + _velocity.x * t,
							_transform->GetPosition().y + _velocity.y * t,
							_transform->GetPosition().z + _velocity.z * t);
}

void ParticleModel::moveConstAccel(float t)
{

	_transform->SetPosition(_transform->GetPosition().x + _velocity.x * t + 0.5f * _acceleration.x * t * t,
							_transform->GetPosition().y + _velocity.y * t + 0.5f * _acceleration.y * t * t,
							_transform->GetPosition().z + _velocity.z * t + 0.5f * _acceleration.z * t * t);

	setVel(_velocity.x + _acceleration.x * t,
			_velocity.y + _acceleration.y * t,
			_velocity.z + _acceleration.z * t);
}


//					Rotation				 //
void ParticleModel::spinConstVel(float t)
{
	_transform->SetRotation(_transform->GetRotation().x + _spinVelocity.x * t,
							_transform->GetRotation().y + _spinVelocity.y * t,
							_transform->GetRotation().z + _spinVelocity.z * t);
}

void ParticleModel::spinConstAccel(float t)
{
	_transform->SetRotation(_transform->GetRotation().x + _spinVelocity.x * t + 0.5f * _spinAccel.x * t * t,
							_transform->GetRotation().y + _spinVelocity.y * t + 0.5f * _spinAccel.y * t * t,
							_transform->GetRotation().z + _spinVelocity.z * t + 0.5f * _spinAccel.z * t * t);

	setSpinVel(_spinVelocity.x + _spinAccel.x * t,
				_spinVelocity.y + _spinAccel.y * t,
				_spinVelocity.z + _spinAccel.z * t);
}


void ParticleModel::UpdateNetForce()
{
	_netForce.x = _thrust.x + _brake.x + _friction.x;
	_netForce.y = _thrust.y + _brake.y + _friction.x;
	_netForce.z = _thrust.z + _brake.z + _friction.z;
}


void ParticleModel::UpdateAccel()
{
	_acceleration.x = _netForce.x / _mass;
	_acceleration.y = _netForce.y / _mass;
	_acceleration.z = _netForce.z / _mass;
}

void ParticleModel::UpdateThrust()
{

	// If so: apply force

	_transform->SetPosition(_transform->GetPosition().x + _netForce.x,
							_transform->GetPosition().y + _netForce.y,
							_transform->GetPosition().z + _netForce.z);


}

void ParticleModel::slidingForce(float theta, float frCoef)
{
	float forceMag = _mass * _gravity * (sin(theta) - frCoef * cos(theta));

	_sForce.x = forceMag * cos(theta);
	_sForce.y = forceMag * sin(theta);
}

void ParticleModel::slidingMotion(float t)
{
	slidingForce(0.0f, 0.5f);

	updateState(t);
	moveConstVel(t);
}

void ParticleModel::dragForce(XMFLOAT3 vel, float dragFactor)
{
	if (_laminar)
	{
		dragLamFlow(vel, dragFactor);
	}
	else
	{
		dragTurbFlow(vel, dragFactor);
	}
}

void ParticleModel::dragLamFlow(XMFLOAT3 vel, float dragFactor)
{
	_drag.x = -dragFactor * vel.x;
	_drag.y = -dragFactor * vel.y;
	_drag.z = -dragFactor * vel.z;
}

void ParticleModel::dragTurbFlow(XMFLOAT3 vel, float dragFactor)
{
	XMFLOAT3 velMag;
	XMStoreFloat3(&velMag, XMVector3Length(XMLoadFloat3(&vel)));


	XMFLOAT3 unitVel;
	XMStoreFloat3(&unitVel, XMVector3Normalize(XMLoadFloat3(&vel)));

	float dragMag = dragFactor * velMag.x * velMag.x;

	_drag.x = dragMag * unitVel.x;
	_drag.y = dragMag * unitVel.y;
	_drag.z = dragMag * unitVel.z;
}

bool ParticleModel::CollisionCheck(XMFLOAT3 pos, float radius)
{
	XMFLOAT3 v =
	{
		_transform->GetPosition().x - pos.x,
		_transform->GetPosition().y - pos.y,
		_transform->GetPosition().z - pos.z,
	};

	float dist = sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
	
	if (dist < (radius + getRadius()))
	{
		return true;
	}

	return false;
}


void ParticleModel::updateState(float t)
{
	UpdateNetForce();
	UpdateAccel();
	moveConstVel(t);
}

void ParticleModel::UpdatePlane(float t)
{
	_angle.x = XMConvertToDegrees(_transform->GetRotation().x);
	_angle.y = XMConvertToDegrees(-_transform->GetRotation().y);
	_angle.z = XMConvertToDegrees(_transform->GetRotation().z);

	/*if (GetAsyncKeyState('S'))
	{
		_isConstVel = !_isConstVel;
	}

	if (GetAsyncKeyState('R'))
	{
		_isSpinConstVel = !_isSpinConstVel;
	}*/

	// Move Plane Up and Down
	if (GetAsyncKeyState('S'))
	{
		_spinVelocity.x -= 0.0001f;
	}
	else if (GetAsyncKeyState('W'))
	{
		_spinVelocity.x += 0.0001f;
		
	}
	else if (_spinVelocity.x >= 0.0f || _spinVelocity.x <= 0.0f)
	{
		if (_spinVelocity.x >= 0.0f)
		{
			_spinVelocity.x -= 0.001f;
		}
		else
		{
			_spinVelocity.x += 0.001f;
		}
	}
	_thrust.y = _angle.x / 90 * -_thrust.z;

	// Move Plane Left and Right
	if (GetAsyncKeyState('A'))
	{
		_spinVelocity.z -= 0.0001f;
	}
	else if (GetAsyncKeyState('D'))
	{
		_spinVelocity.z += 0.0001f;
	}
	else if (_spinVelocity.z >= 0.0f || _spinVelocity.z <= 0.0f)
	{
		if (_spinVelocity.z >= 0.0f)
		{
			_spinVelocity.z -= 0.0001f;
		}
		else
		{
			_spinVelocity.z += 0.0001f;
		}
	}
	

	if (GetAsyncKeyState('Q'))
	{
		_spinVelocity.y -= 0.00001f;
	}
	else if (GetAsyncKeyState('E'))
	{
		_spinVelocity.y += 0.00001f;
	}
	else if (_spinVelocity.y >= 0.0f || _spinVelocity.y <= 0.0f)
	{
		if (_spinVelocity.y >= 0.0f)
		{
			_spinVelocity.y -= 0.0001f;
		}
		else
		{
			_spinVelocity.y += 0.0001f;
		}
	}
	_thrust.x = _angle.y / 90 * -_thrust.z;

	if (GetAsyncKeyState('Z'))
	{
		_thrust.z -= 0.00002f;
	}

	if (GetAsyncKeyState('C'))
	{
		_thrust.z += 0.00004f;
	}

	/*if (_transform->GetPosition().y > _initPos.y)
	{
		_thrust.y -= 0.00015f;
	}
	
	if (_transform->GetPosition().y < _initPos.y)
	{
		setThrust(0, 0, 0);
		_transform->SetPosition(_initPos);
	}*/

	UpdateThrust();
	slidingMotion(t);

	if (_isConstVel)
	{
		moveConstVel(t);
	}
	else
	{
		moveConstAccel(t);
	}

	if (_isSpinConstVel)
	{
		spinConstVel(t);
	}
	else
	{
		spinConstAccel(t);
	}
}

void ParticleModel::UpdateParticle(float t)
{
	updateState(t);
}
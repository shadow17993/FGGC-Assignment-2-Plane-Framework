#include "Transform.h"
#include <windows.h>

using namespace std;
using namespace DirectX;

#pragma once

class ParticleModel
{
public:
	ParticleModel(Transform* transform, bool useConstVel, XMFLOAT3 initVel, XMFLOAT3 initAccel);
	ParticleModel(Transform* transform, bool useConstVel, XMFLOAT3 initVel, XMFLOAT3 initAccel, XMFLOAT3 thrust);
	~ParticleModel();

	void setAccel(XMFLOAT3 acceleration) { _acceleration = acceleration; }
	void setAccel(float x, float y, float z) { _acceleration = { x, y, z }; }
	XMFLOAT3 getAccel() { return _acceleration; }


	void setVel(XMFLOAT3 velocity) { _velocity = velocity; }
	void setVel(float x, float y, float z) { _velocity = { x, y, z }; }
	XMFLOAT3 getVel() { return _velocity; }


	void setSpinAccel(XMFLOAT3 acceleration) { _spinAccel = acceleration; }
	void setSpinAccel(float x, float y, float z) { _spinAccel = { x, y, z }; }
	XMFLOAT3 getSpinAccel() { return _spinAccel; }


	void setSpinVel(XMFLOAT3 velocity) { _spinVelocity = velocity; }
	void setSpinVel(float x, float y, float z) { _spinVelocity = { x, y, z }; }
	XMFLOAT3 getSpinVel() { return _spinVelocity; }


	void setThrust(XMFLOAT3 thrust) { _thrust = thrust; }
	void setThrust(float x, float y, float z) { _thrust = { x, y, z }; }
	XMFLOAT3 getThrust() { return _thrust; }

	void setFriction(XMFLOAT3 friction) { _friction = friction; }
	void setFriction(float x, float y, float z) { _friction = { x, y, z }; }
	XMFLOAT3 getFriction() { return _friction; }

	void setBrake(XMFLOAT3 brake) { _brake = brake; }
	void setBrake(float x, float y, float z) { _brake = { x, y, z }; }
	XMFLOAT3 getBrake() { return _brake; }


	void setMass(float mass) { _mass = mass; }
	float getMass() { return _mass; }

	void setWeight(float weight) { _weight = weight; }
	float getWeight() { return _weight; }

	void setUpforce(float upforce){ _upforce = upforce; }
	float getUpforce() { return _upforce; }

	void setRadius(float radius){ _radius = radius; }
	float getRadius() { return _radius; }

	

	bool setIsConstVel(bool isConstVel) { _isConstVel = isConstVel; }
	bool setIsSpinConstVel(bool isSpinConstVel) { _isSpinConstVel = isSpinConstVel; }

	void Move(float x, float y, float z);
	void moveConstVel(float t);
	void moveConstAccel(float t);

	void spinConstVel(float t);
	void spinConstAccel(float t);

	void UpdateNetForce();
	void UpdateAccel();
	void UpdateThrust();

	void slidingForce(float theta, float frCoef);
	void slidingMotion(float t);

	void MotionInFluid();
	void dragForce(XMFLOAT3 vel, float dragFactor);
	void dragLamFlow(XMFLOAT3 vel, float dragFactor);
	void dragTurbFlow(XMFLOAT3 vel, float dragFactor);

	bool CollisionCheck(XMFLOAT3 pos, float radius);

	Transform* getTransform() { return _transform; }

	void updateState(float t);

	void UpdatePlane(float t);
	void UpdateParticle(float t);

	

private:
	Transform* _transform;
	XMFLOAT3 _initPos;
	XMFLOAT3 _velocity;
	XMFLOAT3 _acceleration;
	XMFLOAT3 _spinVelocity;
	XMFLOAT3 _spinAccel;

	XMFLOAT3 _netForce;
	XMFLOAT3 _friction;
	XMFLOAT3 _thrust;
	XMFLOAT3 _brake;
	XMFLOAT3 _drag;

	float _mass;
	float _gravity;
	float _weight;
	float _upforce;
	

	float _radius;

	XMFLOAT3 _sForce;

	bool _isConstVel;
	bool _isSpinConstVel;
	bool _laminar;

	XMFLOAT3 _angle;
};


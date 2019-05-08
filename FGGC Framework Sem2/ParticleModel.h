#pragma once
#include "Transform.h"
#include "Common.h"
#include "Quaternion.h"

enum MOVEMENT_TYPE
{
	STANDARD = 0,
	CONSTANT_VELOCITY,
	CONSTANT_ACCELERATION
};

class ParticleModel
{
public:
	ParticleModel(Transform* transform, Vector3 centerOfMass, Rect3 boundingBox, std::vector<Vector3> vertices);
	~ParticleModel();

	virtual void Update(MOVEMENT_TYPE type, float deltaTime);

	void AddForce(Vector3 force);
	Vector3 NetForces();
	void AddConstantForces();

	void MotionInFluid();
	void CalculateDragForce();
	void DragLaminarFlow(float dragFactor);
	void DragTurbulentFlow(float dragFactor);

	//void Move(Transform& transform, DIRECTION dir, float deltaTime);
	void Move(float deltaTime);
	void MoveConstantVel(float deltaTime);
	void MoveConstantAcceleration(float deltaTime);

	void EnableStatic() { _staticObj = true; }
	void DisableStatic() { _staticObj = false; }
	bool IsStatic() { return _staticObj; }

	void SetVelocity(Vector3 velocity) { _velocity = velocity; _velocity.Truncate(_maxSpeed); }
	Vector3 GetVelocity() { return _velocity; }

	void SetAcceleration(Vector3 acceleration) { _acceleration = acceleration; }
	Vector3 GetAcceleration() { return _acceleration; }

	virtual void SetMass(float mass) { _mass = mass; _weight = _mass * _gravity; }
	float GetMass() { return _mass; }

	void SetMovementSpeed(float speed) { _movementSpeed = speed; }

	void SetUpThrust(Vector3 force) { _upThrust = force; }
	void SetGravity(Vector3 force) { _gravity = force; _weight = _mass * _gravity; }
	void InverseGravity() { _gravity = _gravity * -1; _weight = _weight * -1; }
	void SetFriction(Vector3 force) { _friction = force; }

	Rect3 GetRect3World();
	Rect3 GetRect3Local();

	void UpdateAxisAlignedBoundingBox();
	Rect3 GetAxisAlignedRect3World();
	Rect3 GetAxisAlignedRect3Local();

	void SetRadius(float radius) { _radius = radius; }
	float GetRadius() { return _radius; }

	std::vector<Vector3> GetVertices() { return _vertices; }

	std::vector<Vector3> GetCollisionVels() { return _collisionVels; }
	void AddCollisionVel(Vector3 vel) { _collisionVels.push_back(vel); }
	void ClearCollisionVels() { _collisionVels.clear(); }

	void SetTransform(Transform* transform) { _transform = transform; }

	virtual void Reset();

protected:

	bool _staticObj;

	std::vector<Vector3> _collisionVels;
	std::vector<Vector3> _vertices;
	Rect3 _axisAlignedBBox;
	Rect3 _boundingBox;

	Transform* _transform;
	Quaternion* _quaternion;

	std::vector<Vector3> _allForces;
	Vector3 _acceleration;
	Vector3 _velocity;

	Vector3 _centerOfMass;

	float _mass;
	float _movementSpeed;
	float _maxSpeed;

	Vector3 _weight;
	Vector3 _upThrust;
	Vector3 _gravity;
	Vector3 _friction;
	Vector3 _drag;

	//Motion in fluid vars
	bool _laminarFlow;

	//Collision Vars
	float _radius;
};
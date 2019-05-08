#pragma once
#include "Common.h"
#include "ParticleModel.h"

enum GEAR_RATIO
{
	GEAR_1 = 0,
	GEAR_2,
	GEAR_3,
	GEAR_4,
	GEAR_5,
	GEAR_MAX
};

class CarBody : public ParticleModel
{
public:
	CarBody(Transform* transform, Vector3 centerOfMass, Rect3 boundingBox, std::vector<Vector3> vertices);
	~CarBody();
	
	void Update(MOVEMENT_TYPE type, float deltaTime) override;

	void CalculteForwardVel(float deltaTime);
	void CalculateAcceleration(Vector3 increment);
	void CalculateSpeed();

	void CalculateAngularAcceleration();
	void CalculateAngularVelocity(float deltaTime);
	void CalculateAngularTorque(Vector3 angularForce, Vector3 pointOfContact, float deltaTime);
	void CalculateAngularMotion(float deltaTime);

	void ApplyFriction();

	Vector3 GetLinearVel() { return _linearVel; }
	void SetLinearVel(Vector3 newVel) { _linearVel = newVel; }

	Vector3 GetLinearThrust() { return _linearThrust; }
	void SetLinearThrust(Vector3 newThurst) { _linearThrust = newThurst; }

	float GetCoefficientOfFriction() { return _coefficientOfFriction; }
	void SetCoefficientOfFriction(float newCoefficient) { _coefficientOfFriction = newCoefficient; }

	float GetWheelRadius() { return _wheelRadius; }
	void SetWheelRadius(float newRadius) { _wheelRadius = newRadius; }

	void SwitchGear();
	void SwitchGear(int increment);
	float GetGearRatio() { return _gearRatio; }

	void SetMass(float mass) override;

	void Reset() override;

private:
	//velocity
	Vector3 _linearVel;
	Vector3 _linearThrust;

	XMMATRIX _inertiaTensor;
	Vector3 _angularVelocity;
	Vector3 _angularAcceleration;
	Vector3 _torque;
	float _maxAngularSpeed;

	//RPM - acceleration
	Vector3 _engineSpeed;
	
	//Wheel Vars
	GEAR_RATIO _currentGear;
	Vector3 _rotSpeed;
	float _wheelRadius, _gearRatio;
	float _coefficientOfFriction, _steeringAngle;

	const float MAX_STEERING_ANGLE = 35.0f;
};
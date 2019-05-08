#include "CarBody.h"
#include <string>
#include <iostream>

CarBody::CarBody(Transform* transform, Vector3 centerOfMass, Rect3 boundingBox, std::vector<Vector3> vertices) : ParticleModel(transform, centerOfMass, boundingBox, vertices)
{
	_wheelRadius = 0.2f;
	_gearRatio = 3.166f;
	_coefficientOfFriction = 0.5f;
	_linearThrust = { 0, 0, 0.15f };
	_steeringAngle = 0.0f;

	_currentGear = GEAR_1;

	_centerOfMass = centerOfMass;

	_maxAngularSpeed = 0.4f;

	XMFLOAT4X4 temp = XMFLOAT4X4
	(
		((0.08333333333f * _mass) * 2), 0.0f, 0.0f, 0.0f,
		0.0f, ((0.08333333333f * _mass) * 2), 0.0f, 0.0f,
		0.0f, 0.0f, ((0.08333333333f * _mass) * 2), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	_inertiaTensor = XMLoadFloat4x4(&temp);
}

CarBody::~CarBody()
{
}

void CarBody::SetMass(float mass)
{
	_mass = mass; 
	_weight = _mass * _gravity;

	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, _inertiaTensor);

	temp._11 = (0.08333333333f * _mass) * 2;
	temp._12 = (0.08333333333f * _mass) * 2;
	temp._12 = (0.08333333333f * _mass) * 2;

	_inertiaTensor = XMLoadFloat4x4(&temp);
}

void CarBody::Reset()
{
	_linearVel.Zero();
	_engineSpeed.Zero();
	_angularVelocity.Zero();

	_gearRatio = 3.166f;
	_steeringAngle = 0.0f;
}

void CarBody::SwitchGear()
{
	if (_linearVel.z < 1)
	{
		_currentGear = GEAR_1;
		_gearRatio = 3.166f;
	}
	else if (_linearVel.z >= 1 && _linearVel.z < 2)
	{
		_currentGear = GEAR_2;
		_gearRatio = 1.882f;
	}
	else if (_linearVel.z >= 2 && _linearVel.z < 3)
	{
		_currentGear = GEAR_3;
		_gearRatio = 1.296f;
	}
	else if (_linearVel.z >= 3 && _linearVel.z < 4)
	{
		_currentGear = GEAR_4;
		_gearRatio = 0.972f;
	}
	else if (_linearVel.z >= 4 && _linearVel.z < 5)
	{
		_currentGear = GEAR_5;
		_gearRatio = 0.738f;
	}
}

void CarBody::SwitchGear(int increment)
{
	GEAR_RATIO tempGear = (GEAR_RATIO)((int)_currentGear + increment);
	if (tempGear >= GEAR_MAX)
		return;

	_currentGear = tempGear;
	std::cout << _currentGear << std::endl;

	switch (_currentGear)
	{
	case GEAR_1:
		_gearRatio = 3.166f;
		break;
	case GEAR_2:
		_gearRatio = 1.882f;
		break;
	case GEAR_3:
		_gearRatio = 1.296f;
		break;
	case GEAR_4:
		_gearRatio = 0.972f;
		break;
	case GEAR_5:
		_gearRatio = 0.738f;
		break;
	}
}

void CarBody::Update(MOVEMENT_TYPE type, float deltaTime)
{
	bool speedChange = false;
	if (GetAsyncKeyState('W'))
	{
		CalculateAcceleration(_linearThrust);
		speedChange = true;
	}

	if (GetAsyncKeyState('S'))
	{
		CalculateAcceleration(-_linearThrust);
		speedChange = true;
	}

	if (!speedChange)
	{
		_engineSpeed = _engineSpeed * 0.95f;
		if (_engineSpeed.z < 0.003 && _engineSpeed.z > -0.003)
			_engineSpeed.z = 0;
	}

	if (GetAsyncKeyState('L') && 
		_steeringAngle < MAX_STEERING_ANGLE)
	{
		_steeringAngle += 0.5f;
	}

	if (GetAsyncKeyState('J') && 
		_steeringAngle > -MAX_STEERING_ANGLE)
	{
		_steeringAngle -= 0.5f;
	}

	SwitchGear();

	//Debugging
	/*if (GetAsyncKeyState('I'))
	{
		SwitchGear(1);
	}

	if (GetAsyncKeyState('K'))
	{
		SwitchGear(-1);
	}*/

	CalculateSpeed();
	CalculteForwardVel(deltaTime);

	if (!_linearVel.IsZero())
		_allForces.push_back(_linearVel);

	/*OutputDebugStringA(std::to_string(_gearRatio).c_str());
	OutputDebugStringA("\n");

	OutputDebugStringA(" Vel: ");
	OutputDebugStringA(std::to_string(_linearVel.x).c_str());
	OutputDebugStringA(", ");
	OutputDebugStringA(std::to_string(_linearVel.y).c_str());
	OutputDebugStringA(", ");
	OutputDebugStringA(std::to_string(_linearVel.z).c_str());
	OutputDebugStringA("\n");
	OutputDebugStringA("\n");*/

	ApplyFriction();

	/*OutputDebugStringA(" Vel: ");
	OutputDebugStringA(std::to_string(_linearVel.x).c_str());
	OutputDebugStringA(", ");
	OutputDebugStringA(std::to_string(_linearVel.y).c_str());
	OutputDebugStringA(", ");
	OutputDebugStringA(std::to_string(_linearVel.z).c_str());
	OutputDebugStringA("\n");
	OutputDebugStringA("\n");*/

	CalculateAngularMotion(deltaTime);

	ParticleModel::Update(type, deltaTime);
}

void CarBody::ApplyFriction()
{
	_linearVel = _linearVel * _coefficientOfFriction;
}

void CarBody::CalculateSpeed()
{
	_rotSpeed = _engineSpeed / _gearRatio;
	//_rotSpeed = _engineSpeed * _gearRatio;
}

void CarBody::CalculteForwardVel(float deltaTime)
{
	Vector3 arcLength = _rotSpeed * XM_2PI * _wheelRadius;

	float radians = _steeringAngle * (XM_PI / 180.0f); 	// convert degrees to radians

	float x = (arcLength.x * cos(radians)) + (arcLength.z * sin(radians));
	float z = (-arcLength.x * sin(radians)) + (arcLength.z * cos(radians));

	arcLength.x = x;
	arcLength.z = z;
	
	//Quaternion q = _transform->GetRotation();
	//q.addScaledVector(arcLength, 1.0f);
	//_transform->ApplyRotation(q);

	if (!arcLength.IsZero())
	{
		_linearVel += arcLength / deltaTime;
	}
}

void CarBody::CalculateAngularAcceleration()
{
	XMMATRIX it_Inverse = XMMatrixInverse(&XMMatrixDeterminant(_inertiaTensor), _inertiaTensor);
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, it_Inverse);

	_angularAcceleration = _torque * Vector3(temp._11, temp._22, temp._33);
	_torque.Zero();
}

void CarBody::CalculateAngularVelocity(float deltaTime)
{
	_angularVelocity += _angularAcceleration * deltaTime;
	_angularVelocity.Truncate(_maxAngularSpeed);
	_angularVelocity = _angularVelocity * 0.9f;
}

void CarBody::CalculateAngularTorque(Vector3 angularForce, Vector3 pointOfContact, float deltaTime)
{
	Vector3 force = angularForce / deltaTime;
	_torque = Cross(pointOfContact - _transform->GetPosition(), force);
}

void CarBody::CalculateAngularMotion(float deltaTime)
{
	//apply angular rotation
	CalculateAngularAcceleration();
	CalculateAngularVelocity(deltaTime);

	Quaternion _currentRot;
	//_currentRot.SetQuaternion(_angularVelocity);
	_currentRot.addScaledVector(_angularVelocity, deltaTime);
	_currentRot.normalise();

	_transform->ApplyRotation(_currentRot);
}

void CarBody::CalculateAcceleration(Vector3 increment)
{
	_engineSpeed += increment;
}
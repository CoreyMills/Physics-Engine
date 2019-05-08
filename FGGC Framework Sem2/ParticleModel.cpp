#include "ParticleModel.h"

ParticleModel::ParticleModel(Transform* transform, Vector3 centerOfMass, Rect3 boundingBox, std::vector<Vector3> vertices)
{
	_staticObj = true;

	_vertices = vertices;
	_boundingBox = boundingBox;

	_laminarFlow = false;
	_transform = transform;

	_mass = 1.0f;
	_movementSpeed = 1.0f;
	_maxSpeed = 0.5f;

	_upThrust = { 0, 2.0f, 0 };
	_gravity = { 0, -1.0f, 0 };

	_weight = _gravity * _mass;

	_centerOfMass = centerOfMass;

	//_dir = NONE;

	//Setup collision vars
	_radius = 0;

	if (boundingBox.maxPoint.x == 0.0f && boundingBox.minPoint.x == 0.0f)
		boundingBox.minPoint.x = boundingBox.maxPoint.x = 0.1f;
	if (boundingBox.maxPoint.y == 0.0f && boundingBox.minPoint.y == 0.0f)
		boundingBox.minPoint.y = boundingBox.maxPoint.y = 0.1f;
	if (boundingBox.maxPoint.z == 0.0f && boundingBox.minPoint.z == 0.0f)
		boundingBox.minPoint.z = boundingBox.maxPoint.z = 0.1f;

	UpdateAxisAlignedBoundingBox();
}

ParticleModel::~ParticleModel()
{
}

void ParticleModel::Reset()
{
	_allForces.clear();
	_velocity.Zero();
	_acceleration.Zero();
}

void ParticleModel::UpdateAxisAlignedBoundingBox()
{
	float minX, minY, minZ, maxX, maxY, maxZ;
	minX = minY = minZ = INT_MAX;
	maxX = maxY = maxZ = INT_MIN;

	for (auto vertex : _vertices)
	{
		vertex = vertex * _transform->GetRotationMatrix();
		//X's
		if (vertex.x > maxX)
			maxX = vertex.x;

		if (vertex.x < minX)
			minX = vertex.x;

		//Y's
		if (vertex.y > maxY)
			maxY = vertex.y;

		if (vertex.y < minY)
			minY = vertex.y;

		//Z's
		if (vertex.z > maxZ)
			maxZ = vertex.z;

		if (vertex.z < minZ)
			minZ = vertex.z;
	}

	if (maxX == 0.0f && minX == 0.0f)
		minX = 0.1f;
	if (maxY == 0.0f && minY == 0.0f)
		minY = 0.1f;
	if (maxZ == 0.0f && minZ == 0.0f)
		minZ = 0.1f;

	_axisAlignedBBox.maxPoint = Vector3(maxX, maxY, maxZ);
	_axisAlignedBBox.minPoint = Vector3(minX, minY, minZ);

	_transform->SetRotFlag(false);
}

void ParticleModel::Update(MOVEMENT_TYPE type, float deltaTime)
{
	if (_transform->GetRotFlag())
		UpdateAxisAlignedBoundingBox();

	if (!_staticObj)
	{
		//Hard y constraint
		/*if (_transform->GetPosition().y - _transform->GetScale().y < 0.0f)
		{
			_transform->SetPosition(Vector3(_transform->GetPosition().x,
				(0.0f + _transform->GetScale().y),
				_transform->GetPosition().z));
			_velocity = -_velocity;
		}*/

		if (GetAsyncKeyState('M'))
		{
			MotionInFluid();
		}

		if (GetAsyncKeyState('F'))
			AddForce(_upThrust);

		//turn left
		if (GetAsyncKeyState('T'))
			_transform->ApplyRotation(Quaternion(Vector3(0.0f, 2.0f, 0.0f)));

		//turn right
		if (GetAsyncKeyState('R'))
			_transform->ApplyRotation(Quaternion(Vector3(0.0f, -2.0f, 0.0f)));

		switch (type)
		{
		case STANDARD:
			AddConstantForces();
			Move(deltaTime);
			break;
		case CONSTANT_VELOCITY:
			MoveConstantVel(deltaTime);
			break;
		case CONSTANT_ACCELERATION:
			MoveConstantAcceleration(deltaTime);
			break;
		}
	}
}

Vector3 ParticleModel::NetForces()
{
	Vector3 netForce;
	for (unsigned int i = 0; i < _allForces.size(); i++)
	{
		netForce += _allForces.at(i);
	}
	_allForces.clear();
	return netForce;
}

void ParticleModel::AddForce(Vector3 force)
{
	_allForces.push_back(force);
}

void ParticleModel::AddConstantForces()
{
	//add friction if moving
	if (_velocity.Length() > 0)
	{
		_velocity = _velocity * 0.8f;

		float maxAxis = _velocity.AbsMaxAxis();
		if (maxAxis < 0.00003 && maxAxis > -0.00003)
			_velocity.Zero();
	}

	//apply gravit * mass
	_allForces.push_back(_weight);

	//apply upForce
	if (_transform->GetPosition().y - _transform->GetScale().y  <= 0.01f)
	{
		_allForces.push_back(-_weight);
	}
}

void ParticleModel::MotionInFluid()
{
	CalculateDragForce();
}

void ParticleModel::CalculateDragForce()
{
	if (_laminarFlow)
		DragLaminarFlow(0.4f);
	else
		DragTurbulentFlow(0.67f);
}

void ParticleModel::DragLaminarFlow(float dragFactor)
{
	_drag = -dragFactor * _velocity;
}

void ParticleModel::DragTurbulentFlow(float dragFactor)
{
	Vector3 unitVel = Vec3Normalization(_velocity);
	float velMag = _velocity.Length();

	float dragMag = dragFactor * (velMag * velMag);

	_drag = -dragMag * unitVel;
}

void ParticleModel::Move(float deltaTime)
{
	_acceleration = NetForces() / _mass;
	
	/*Vector3 pos = _transform->GetPosition();
	
	_velocity = _velocity + _acceleration * deltaTime;
	_velocity.Truncate(_maxSpeed);

	pos += _velocity * deltaTime;

	_transform->SetOldPosition(_transform->GetPosition());
	_transform->SetPosition(pos);*/

	Vector3 pos = _transform->GetPosition() + _velocity * deltaTime +
		((_acceleration * 0.5f) * (deltaTime * deltaTime));

	_transform->SetOldPosition(_transform->GetPosition());
	_transform->SetPosition(pos);

	_velocity = _velocity + _acceleration * deltaTime;
}

void ParticleModel::MoveConstantVel(float deltaTime)
{
	Vector3 pos = _transform->GetPosition() + _velocity * deltaTime;
	_transform->SetPosition(pos);
}

void ParticleModel::MoveConstantAcceleration(float deltaTime)
{
	Vector3 pos = _transform->GetPosition() + _velocity * deltaTime +
		((_acceleration * 0.5f) * (deltaTime * deltaTime));
	_transform->SetPosition(pos);

	_velocity += _acceleration * deltaTime;
}

Rect3 ParticleModel::GetRect3World()
{
	Rect3 temp = Rect3();
	temp.maxPoint = _transform->GetPosition() + (_boundingBox.maxPoint * _transform->GetScale());
	temp.minPoint = _transform->GetPosition() + (_boundingBox.minPoint * _transform->GetScale());
	return temp;
}

Rect3 ParticleModel::GetRect3Local()
{
	return _boundingBox;
}

Rect3 ParticleModel::GetAxisAlignedRect3World()
{
	Rect3 temp = Rect3();
	temp.maxPoint = _transform->GetPosition() + (_axisAlignedBBox.maxPoint * _transform->GetScale());
	temp.minPoint = _transform->GetPosition() + (_axisAlignedBBox.minPoint * _transform->GetScale());
	return temp;
}

Rect3 ParticleModel::GetAxisAlignedRect3Local()
{
	return _axisAlignedBBox;
}

#include "Spring.h"

Spring::Spring(GameObject* aObj, GameObject* bObj, float minBuffer, float maxBuffer, float forceStrength)
{
	this->_aObj = aObj;
	this->_bObj = bObj;
	this->_minBuffer = minBuffer;
	this->_maxBuffer = maxBuffer;
	this->_forceStrength = forceStrength;
}

Spring::~Spring()
{
	_aObj = nullptr;
	delete _aObj;

	_bObj = nullptr;
	delete _bObj;
}

void Spring::Update(float deltaTime)
{
	Vector3 ba = (_aObj->GetTransform()->GetPosition() - _bObj->GetTransform()->GetPosition());
	float dist = ba.Length();

	Vector3 force = ba * _forceStrength;

	if (dist >= _maxBuffer)
	{
		_bObj->GetParticleModel()->AddForce(force);
		_aObj->GetParticleModel()->AddForce(-force);
	}
	else if (dist <= _minBuffer)
	{
		_bObj->GetParticleModel()->AddForce(-force);
		_aObj->GetParticleModel()->AddForce(force);
	}
}

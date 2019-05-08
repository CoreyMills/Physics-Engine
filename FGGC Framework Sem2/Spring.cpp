#include "Spring.h"

Spring::Spring(GameObject* aObj, GameObject* bObj, float minBuffer, float maxBuffer)
{
	this->_aObj = aObj;
	this->_bObj = bObj;
	this->minBuffer = minBuffer;
	this->maxBuffer = maxBuffer;

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

	if (dist >= maxBuffer)
	{
		Vector3 force = ba.Normalise() * (dist / 3);

		_bObj->GetParticleModel()->AddForce(force);
		_aObj->GetParticleModel()->AddForce(-force);
	}
	if (dist <= minBuffer)
	{
		Vector3 force = -ba.Normalise() * (dist / 3);

		_bObj->GetParticleModel()->AddForce(force);
		_aObj->GetParticleModel()->AddForce(-force);
	}
}

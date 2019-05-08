#include "Pole.h"

Pole::Pole(GameObject* aObj, GameObject* bObj)
{
	this->_aObj = aObj;
	this->_bObj = bObj;

	_ab = _bObj->GetTransform()->GetPosition() - _aObj->GetTransform()->GetPosition();
	_aPrevious4X4 = _aObj->GetTransform()->GetWorldMatrix4X4();
	_bPrevious4X4 = _bObj->GetTransform()->GetWorldMatrix4X4();
}

Pole::~Pole()
{
	_aObj = nullptr;
	delete _aObj;

	_bObj = nullptr;
	delete _bObj;
}

void Pole::Update(float deltaTime)
{
	bool aMoved = false;
	bool bMoved = false;

	if (_aObj->GetTransform()->GetPosition() != _aObj->GetTransform()->GetOldPosition())
		aMoved = true;

	if (_bObj->GetTransform()->GetPosition() != _bObj->GetTransform()->GetOldPosition())
		bMoved = true;

	if (aMoved && bMoved)
	{

	}
	else if (aMoved)
	{
		Vector3 distMoved = _aObj->GetTransform()->GetOldPosition() - _aObj->GetTransform()->GetPosition();

		_bObj->GetTransform()->SetPosition(_bObj->GetTransform()->GetPosition() + distMoved);
	}
	else if (bMoved)
	{
		Vector3 distMoved = _bObj->GetTransform()->GetOldPosition() - _bObj->GetTransform()->GetPosition();
			
		_aObj->GetTransform()->SetPosition(_aObj->GetTransform()->GetPosition() + distMoved);
	}
}
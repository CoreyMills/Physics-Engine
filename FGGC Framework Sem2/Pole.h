#pragma once
#include "Common.h"
#include "GameObject.h"

class Pole
{
public:
	Pole(GameObject* aObj, GameObject* bObj);
	~Pole();

	void Update(float deltaTime);
private:
	GameObject* _aObj;
	GameObject* _bObj;

	XMFLOAT4X4 _aPrevious4X4, _bPrevious4X4;
	Vector3 _ab;
};
#pragma once
#include "Common.h"
#include "GameObject.h"

class Spring
{
public:
	Spring(GameObject* aObj, GameObject* bObj, float minBuffer, float maxBuffer);
	~Spring();

	void Update(float deltaTime);

private:
	GameObject* _aObj;
	GameObject* _bObj;

	float minBuffer;
	float maxBuffer;
};
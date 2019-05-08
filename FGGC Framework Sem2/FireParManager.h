#pragma once

#include "BaseParManager.h"

class FireParManager : public BaseParManager
{
public:
	FireParManager(std::string managerName, GameObject * gameObjectfloat, float managerLifeSpan, float particleLifeSpan, int numOfPars, bool active);
	~FireParManager();

	void UpdateParticles(float deltaTime) override;

private:
	unsigned int _maxSparks;
	float _sparkChance;
	float _sparkTimeWaited;
};
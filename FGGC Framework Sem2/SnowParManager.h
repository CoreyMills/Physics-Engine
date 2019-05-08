#pragma once

#include "BaseParManager.h"

class SnowParManager : public BaseParManager
{
public:
	SnowParManager(std::string managerName, GameObject * gameObjectfloat, float managerLifeSpan, float particleLifeSpan, int numOfPars, bool active);
	~SnowParManager();

	void UpdateParticles(float deltaTime) override;

private:
};
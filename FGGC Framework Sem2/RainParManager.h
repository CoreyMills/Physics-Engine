#pragma once

#include "BaseParManager.h"

class RainParManager : public BaseParManager
{
public:
	RainParManager(std::string managerName, GameObject * gameObjectfloat, float managerLifeSpan, float particleLifeSpan, int numOfPars, bool active);
	~RainParManager();

	void UpdateParticles(float deltaTime) override;

private:
};
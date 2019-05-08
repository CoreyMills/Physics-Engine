#pragma once
#include "Common.h"
#include "BaseParManager.h"
#include "FireParManager.h"
#include "SnowParManager.h"
#include "RainParManager.h"

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	void UpdateParManagers(float deltaTime);
	void DrawParManagers(ID3D11DeviceContext* pImmediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb);

	void AddParticleManager(BaseParManager* parManager);
	void RemoveParticleManager(std::string name);

	void ApplyForceToSet(std::string name, Vector3 force);

	std::vector<BaseParManager*> GetManagers() { return _parManagerArray; }
	BaseParManager* GetManager(std::string name);

private:
	std::vector<BaseParManager*> _parManagerArray;
};
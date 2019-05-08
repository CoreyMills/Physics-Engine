#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
	_parManagerArray.erase(_parManagerArray.begin(), _parManagerArray.end());
}

void ParticleSystem::UpdateParManagers(float deltaTime)
{
	for (unsigned int i = 0; i < _parManagerArray.size(); i++)
	{
		if (_parManagerArray.at(i)->GetTimeSinceSpawn() >= _parManagerArray.at(i)->GetLifeSpan())
		{
			_parManagerArray.erase(_parManagerArray.begin() + i);
			_parManagerArray.shrink_to_fit();
			i--;
		}
	}

	for (auto manager : _parManagerArray)
	{
		if (!manager->IsActive())
			continue;

		switch (manager->GetType())
		{
		case 0:
			manager->UpdateParticles(deltaTime);
			break;
		case 1:
			((FireParManager*)manager)->UpdateParticles(deltaTime);
			break;
		case 2:
			((SnowParManager*)manager)->UpdateParticles(deltaTime);
			break;
		case 3:
			((RainParManager*)manager)->UpdateParticles(deltaTime);
			break;
		}
	}
}

void ParticleSystem::DrawParManagers(ID3D11DeviceContext * pImmediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb)
{
	for (auto manager : _parManagerArray)
	{
		if (manager->IsActive())
			manager->DrawParticles(pImmediateContext, constantBuffer, cb);
	}
}

void ParticleSystem::AddParticleManager(BaseParManager* parManager)
{
	_parManagerArray.push_back(parManager);
}

void ParticleSystem::RemoveParticleManager(std::string name)
{
	for (unsigned int i = 0; i < _parManagerArray.size(); i++)
	{
		if (_parManagerArray.at(i)->GetName() == name)
		{
			_parManagerArray.erase(_parManagerArray.begin() + i);
			_parManagerArray.shrink_to_fit();
		}
	}
}

void ParticleSystem::ApplyForceToSet(std::string name, Vector3 force)
{
	for (auto manager : _parManagerArray)
	{
		if (manager->GetName() == name)
		{
			manager->ApplyForceToParticles(force);
			return;
		}
	}
}

BaseParManager * ParticleSystem::GetManager(std::string name)
{
	for (auto manager : _parManagerArray)
	{
		if (manager->GetName() == name)
			return manager;
	}
	return nullptr;
}

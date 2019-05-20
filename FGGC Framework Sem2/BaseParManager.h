#pragma once

#include "Common.h"
#include "GameObject.h"

enum ParManagerType
{
	TYPE_BASE = 0,
	TYPE_FIRE,
	TYPE_SNOW,
	TYPE_RAIN,
	TYPE_MAX
};

enum ParState
{
	STATE_ACTIVE = 0,
	STATE_DEACTIVE,
	STATE_MAX
};

struct Particle
{
	GameObject* object;
	float lifeSpan;
	bool justActivated;
	ParState state;

	Particle()
	{
		object = nullptr;
		lifeSpan = 0.0f;
		justActivated = true;
		state = ParState::STATE_DEACTIVE;
	}

	Particle(GameObject* object, float lifeSpan, ParState state)
	{
		this->object = object;
		this->lifeSpan = lifeSpan;
		this->state = state;
	}

	~Particle()
	{
		object->~GameObject();
		object = nullptr;
		delete object;
	}
};

class BaseParManager
{
public:
	BaseParManager(std::string managerName, GameObject * gameObjectfloat, 
		float managerLifeSpan, float particleLifeSpan, int numOfPars, bool active);
	~BaseParManager();

	virtual void UpdateParticles(float deltaTime);
	void DrawParticles(ID3D11DeviceContext * pImmediateContext, 
		ID3D11Buffer* constantBuffer, ConstantBuffer cb);

	virtual Particle* CreateParticle();
	void RemoveParticles(int numOfPars);

	Particle* FindDeactiveParticle();
	void ApplyForceToParticles(Vector3 force);

	std::string GetName() { return _name; }

	Vector3 GetEmitterPos() { return _emitterPos; }
	void SetEmitterPos(Vector3 newPos) { _emitterPos = newPos; }

	ParManagerType GetType() { return _managerType; }

	std::vector<Particle*> GetCollidableParticles() { return _collidingParArray; }

	const float GetLifeSpan() { return _managerLifeSpan; }
	const float GetTimeSinceSpawn() { return _timeSinceSpawn; }

	void EnableManager() { _active = true; }
	void DisableManager() { _active = false; }
	bool IsActive() { return _active; }

	void Reset();

protected:
	GameObject* _baseCopyObj;

	ParManagerType _managerType;

	std::string _name;
	std::vector<Particle*> _particleArray;
	std::vector<Particle*> _collidingParArray;
	Vector3 _emitterPos;

	float _managerLifeSpan;
	float _parLifeSpan;
	float _timeSinceActivated;
	float _timeSinceSpawn;

	bool _active;
};
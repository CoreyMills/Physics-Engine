#include "FireParManager.h"

FireParManager::FireParManager(std::string managerName, GameObject * gameObjectfloat, float managerLifeSpan, float particleLifeSpan, int numOfPars, bool active) :
	BaseParManager(managerName, gameObjectfloat, managerLifeSpan, particleLifeSpan, numOfPars, active)
{
	_managerType = TYPE_FIRE;

	_maxSparks = 4;
	_sparkChance = 80.0f;
	_sparkTimeWaited = 0.0f;

	//change gravity so it goes in opposite direction with less force
	for (Particle* par :_particleArray)
	{
		par->object->GetParticleModel()->SetGravity({ 0, 0.5f, 0 });
	}
}

FireParManager::~FireParManager()
{
}

void FireParManager::UpdateParticles(float deltaTime)
{
	for (Particle* par : _particleArray)
	{
		if (par->state == ParState::STATE_DEACTIVE)
			continue;

	}

	float transparencyIncrement = 0.01f;

	if (!_particleArray.empty())
	{
		for (Particle* par : _particleArray)
		{
			if (par->state == ParState::STATE_DEACTIVE)
				continue;

			if (par->justActivated)
				par->justActivated = false;

			//apply forces
			par->object->GetParticleModel()->AddForce(Vector3(RandomClamped() / 15, 0.0f, RandomClamped() / 15));

			//apply fire colours
			XMFLOAT4 ambient = par->object->GetAppearance()->GetMaterial()->ambient;

			if (par->lifeSpan >= _parLifeSpan * 0.8f)
				ambient = XMFLOAT4(0.99f, 0.99f, 0.33f, ambient.w);
			else if (par->lifeSpan >= _parLifeSpan * 0.6f)
				ambient = XMFLOAT4(0.85f, 0.81f, 0.34f, ambient.w);
			else if (par->lifeSpan >= _parLifeSpan * 0.4f)
				ambient = XMFLOAT4(0.94f, 0.49f, 0.5f, ambient.w);
			else if (par->lifeSpan >= _parLifeSpan * 0.35f)
				ambient = XMFLOAT4(0.50f, 0.035f, 0.035f, ambient.w);
			else
			{
				ambient = XMFLOAT4(0.45f, 0.46f, 0.46f, ambient.w);
				if (par->object->GetAppearance()->IsTransparent())
					par->object->GetAppearance()->SetBlendFactor(
						ClampVec3(par->object->GetAppearance()->GetBlendFactor() + Vector3(transparencyIncrement), 1.0f, 0.0f));
			}
			par->object->GetAppearance()->GetMaterial()->ambient = ambient;
		}
		
		if (GetAsyncKeyState('B'))
		{
			for (Particle* par : _particleArray)
			{
				if (par->object->GetAppearance()->IsTransparent())
				{
					par->object->GetAppearance()->SetBlendFactor(
						ClampVec3(par->object->GetAppearance()->GetBlendFactor() + Vector3(transparencyIncrement), 1.0f, 0.0f));
				}
			}
		}
	}

	if (_collidingParArray.size() < _maxSparks)
	{
		//Creates Sparks
		_sparkTimeWaited += deltaTime;
		float randSpark = (rand() % 80) + _sparkTimeWaited;
		if (randSpark > _sparkChance)
		{
			_sparkTimeWaited = 0;

			Particle* spark = CreateParticle();
			spark->state = ParState::STATE_ACTIVE;
			spark->lifeSpan *= 1.5f;
			
			XMFLOAT4 ambient = spark->object->GetAppearance()->GetMaterial()->ambient;
			ambient = XMFLOAT4(1.0f, 1.0f, 0.66f, ambient.w);
			spark->object->GetAppearance()->GetMaterial()->ambient = ambient;

			spark->object->GetParticleModel()->SetMass(0.1f);
			spark->object->GetParticleModel()->AddForce(Vector3((RandomClamped() * 1.5f), 2.0f, (RandomClamped() * 1.5f)));
			_collidingParArray.push_back(spark);
		}
	}

	BaseParManager::UpdateParticles(deltaTime);
}
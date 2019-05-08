#include "RainParManager.h"

RainParManager::RainParManager(std::string managerName, GameObject * gameObjectfloat, float managerLifeSpan, float particleLifeSpan, int numOfPars, bool active) :
	BaseParManager(managerName, gameObjectfloat, managerLifeSpan, particleLifeSpan, numOfPars, active)
{
	_managerType = TYPE_RAIN;

	//Setup Colour
	for (Particle* par : _particleArray)
	{
		XMFLOAT4 ambient = par->object->GetAppearance()->GetMaterial()->ambient;
		ambient = XMFLOAT4(0.0f, 0.2f, 1.0f, ambient.w);
		par->object->GetAppearance()->GetMaterial()->ambient = ambient;
	}
}

RainParManager::~RainParManager()
{
}

void RainParManager::UpdateParticles(float deltaTime)
{
	//apply forces
	for (Particle* par : _particleArray)
	{
		if (par->state == ParState::STATE_DEACTIVE)
			continue;

		if (par->justActivated)
		{
			par->justActivated = false;

			Vector3 newPos = par->object->GetTransform()->GetPosition();
			newPos += Vector3((RandomClamped() * 15), 0.0f, (RandomClamped() * 15));
			par->object->GetTransform()->SetPosition(newPos);
		}
		else
		{
			par->object->GetParticleModel()->AddForce(Vector3(0.0f, -5.0f, 0.0f));
		}

		if (par->object->GetTransform()->GetPosition().y >= par->object->GetTransform()->GetScale().y * 2)
			par->object->GetParticleModel()->AddForce(Vector3(RandomClamped() / 50, -0.6f, RandomClamped() / 50));
		else
		{
			Vector3 newPos = par->object->GetTransform()->GetPosition();
			newPos.y = par->object->GetTransform()->GetScale().y;
			par->object->GetTransform()->SetPosition(newPos);
			par->object->GetParticleModel()->SetVelocity(Vector3());
		}
	}

	BaseParManager::UpdateParticles(deltaTime);
}
#include "SnowParManager.h"

SnowParManager::SnowParManager(std::string managerName, GameObject * gameObjectfloat, float managerLifeSpan, float particleLifeSpan, int numOfPars, bool active) :
	BaseParManager(managerName, gameObjectfloat, managerLifeSpan, particleLifeSpan, numOfPars, active)
{
	_managerType = TYPE_SNOW;

	//Setup Colour
	for (Particle* par : _particleArray)
	{
		XMFLOAT4 ambient = par->object->GetAppearance()->GetMaterial()->ambient;
		ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, ambient.w);
		par->object->GetAppearance()->GetMaterial()->ambient = ambient;
	}
}

SnowParManager::~SnowParManager()
{
}

void SnowParManager::UpdateParticles(float deltaTime)
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
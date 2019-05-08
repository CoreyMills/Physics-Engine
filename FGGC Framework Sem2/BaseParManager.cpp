#include "BaseParManager.h"
#include <string>
#include <iostream>

BaseParManager::BaseParManager(std::string managerName, GameObject * gameObject, float managerLifeSpan, float particleLifeSpan, int numOfPars, bool active)
{
	_name = managerName;

	_baseCopyObj = gameObject;

	_active = active;

	_emitterPos = gameObject->GetTransform()->GetPosition();
	_parLifeSpan = particleLifeSpan;

	_managerLifeSpan = managerLifeSpan;

	_timePassed = 0.0f;
	_timeSinceSpawn = 0.0f;

	if (_particleArray.empty())
	{
		for (int i = 0; i < numOfPars; i++)
		{
			_particleArray.push_back(CreateParticle());
		}
	}
}

BaseParManager::~BaseParManager()
{
	if (!_particleArray.empty()) _particleArray.erase(_particleArray.begin(), _particleArray.end());
	if (!_collidingParArray.empty()) _collidingParArray.erase(_collidingParArray.begin(), _collidingParArray.end());
}

void BaseParManager::UpdateParticles(float deltaTime)
{
	_timeSinceSpawn += deltaTime;

	_timePassed -= deltaTime;
	if (_timePassed <= 0.0f)
	{
		_timePassed = _parLifeSpan / _particleArray.size();
		
		int numToActive = (int)(deltaTime / _timePassed);
		int numActivated = 0;

		for (Particle* par : _particleArray)
		{
			if (par->state == ParState::STATE_DEACTIVE)
			{
				numActivated++;
				par->state = ParState::STATE_ACTIVE;
				par->justActivated = true;
			}

			if (numActivated == numToActive)
				break;
		}
	}

	for (Particle* par : _particleArray)
	{
		if (par->state == ParState::STATE_ACTIVE)
		{
			par->lifeSpan -= deltaTime;

			if (par->lifeSpan <= 0)
			{
				par->state = ParState::STATE_DEACTIVE;
				par->lifeSpan = _parLifeSpan;
				par->object->Reset();
				par->object->GetTransform()->SetPosition(_emitterPos);

				par->object->GetAppearance()->SetBlendFactor(_baseCopyObj->GetAppearance()->GetBlendFactor());
				par->object->GetAppearance()->GetMaterial()->ambient = _baseCopyObj->GetAppearance()->GetMaterial()->ambient;
			}
			else
				par->object->Update(deltaTime);
		}
	}

	for (unsigned int i = 0; i < _collidingParArray.size(); i++)
	{
		_collidingParArray.at(i)->lifeSpan -= deltaTime;

		if (_collidingParArray.at(i)->lifeSpan <= 0)
		{
			if (!_collidingParArray.empty()) _collidingParArray.erase(_collidingParArray.begin() + i);
			_collidingParArray.shrink_to_fit();
		}

		if (!_collidingParArray.empty())
		{
			_collidingParArray.at(i)->object->Update(deltaTime);
		}
	}
}

void BaseParManager::DrawParticles(ID3D11DeviceContext * pImmediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb)
{
	//draw normal particles
	for (Particle* par : _particleArray)
	{
		if (par->state == ParState::STATE_DEACTIVE)
			continue;

		// Get render material
		Material* material = par->object->GetAppearance()->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material->ambient;
		cb.surface.DiffuseMtrl = material->diffuse;
		cb.surface.SpecularMtrl = material->specular;

		// Set world matrix
		cb.World = XMMatrixTranspose(par->object->GetTransform()->GetWorldMatrix());

		// Set texture
		if (par->object->GetAppearance()->HasTexture())
		{
			ID3D11ShaderResourceView * textureRV = par->object->GetAppearance()->GetTextureRV();
			pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
			cb.HasTexture = 1.0f;
		}
		else
		{
			cb.HasTexture = 0.0f;
		}

		// Update constant buffer
		pImmediateContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
	
		par->object->Draw(pImmediateContext);
	}

	//draw collidable particles
	for (Particle* par : _collidingParArray)
	{
		if (par->state == ParState::STATE_DEACTIVE)
			continue;

		// Get render material
		Material* material = par->object->GetAppearance()->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material->ambient;
		cb.surface.DiffuseMtrl = material->diffuse;
		cb.surface.SpecularMtrl = material->specular;

		// Set world matrix
		cb.World = XMMatrixTranspose(par->object->GetTransform()->GetWorldMatrix());

		// Set texture
		if (par->object->GetAppearance()->HasTexture())
		{
			ID3D11ShaderResourceView * textureRV = par->object->GetAppearance()->GetTextureRV();
			pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
			cb.HasTexture = 1.0f;
		}
		else
		{
			cb.HasTexture = 0.0f;
		}

		// Update constant buffer
		pImmediateContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);

		par->object->Draw(pImmediateContext);
	}
}

Particle* BaseParManager::CreateParticle()
{
	GameObject* temp = new GameObject(_baseCopyObj);
	temp->GetTransform()->SetPosition(_emitterPos);
	temp->SetName(temp->GetName() + std::to_string(_particleArray.size()));
	temp->SetAppearance(new Appearance(*temp->GetAppearance()));
	temp->GetAppearance()->SetMaterial(new Material(*temp->GetAppearance()->GetMaterial()));

	return new Particle(temp, _parLifeSpan, ParState::STATE_DEACTIVE);
}

void BaseParManager::RemoveParticles(int numOfPars)
{
	if (!_particleArray.empty()) _particleArray.erase(_particleArray.end() - numOfPars, _particleArray.end());
	_particleArray.shrink_to_fit();
}

Particle * BaseParManager::FindDeactiveParticle()
{
	for (Particle* par : _particleArray)
	{
		if (par->state == ParState::STATE_DEACTIVE)
			return par;
	}
	return nullptr;
}

void BaseParManager::ApplyForceToParticles(Vector3 force)
{
	for (Particle* par : _particleArray)
		par->object->GetParticleModel()->AddForce(force);
}

void BaseParManager::Reset()
{
	for (Particle* par : _particleArray)
	{
		par->state = ParState::STATE_DEACTIVE;
		par->lifeSpan = _parLifeSpan;
		par->object->Reset();
		par->object->GetTransform()->SetPosition(_emitterPos);

		par->object->GetAppearance()->SetBlendFactor(_baseCopyObj->GetAppearance()->GetBlendFactor());
		par->object->GetAppearance()->GetMaterial()->ambient = _baseCopyObj->GetAppearance()->GetMaterial()->ambient;
	}
}

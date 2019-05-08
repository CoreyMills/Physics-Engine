#include "GameObject.h"

GameObject::GameObject(std::string name, Appearance* appearance, Transform* transform, ParticleModel* particleModel) :
	_name(name), _appearance(appearance), _transform(transform), _particleModel(particleModel)
{
	_parent = nullptr;
}

GameObject::GameObject(GameObject* objCopy)
{
	_name = objCopy->GetName();

	if (objCopy->GetAppearance())
		_appearance = new Appearance(*objCopy->GetAppearance());
	else
		_appearance = nullptr;

	if (objCopy->GetTransform())
		_transform = new Transform(*objCopy->GetTransform());
	else
		_transform = nullptr;

	if (objCopy->GetParticleModel())
	{
		_particleModel = new ParticleModel(*objCopy->GetParticleModel());
		_particleModel->SetTransform(_transform);
	}
	else
		_particleModel = nullptr;

	_parent = nullptr;
}

GameObject::~GameObject()
{
	_transform = nullptr;
	delete _transform;

	_particleModel = nullptr;
	delete _particleModel;

	_appearance = nullptr;
	delete _appearance;

	_parent = nullptr;
	delete _parent;

	if (!_quads.empty()) _quads.clear();
}

void GameObject::Reset()
{
	_particleModel->Reset();

	CarBody* temp = (CarBody*)_particleModel;
	if (temp)
	{
		temp->Reset();
	}
}

void GameObject::Update(float deltaTime)
{
	if (_particleModel)
	{
		_particleModel->Update(STANDARD, deltaTime);
	}

	// Calculate world matrix
	_transform->Update();

	if (_parent != nullptr)
	{
		XMStoreFloat4x4(&_transform->GetWorldMatrix4X4(), _transform->GetWorldMatrix() * _parent->GetTransform()->GetWorldMatrix());
	}
}

void GameObject::Draw(ID3D11DeviceContext * pImmediateContext)
{
	if (!_appearance->IsTransparent())
		pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
	else
	{
		Vector3 factor = _appearance->GetBlendFactor();
		
		if (factor.IsMaxed(1.0f))
			return;
		
		float tempBF[] = { factor.x, factor.y, factor.z, 1.0f };
		pImmediateContext->OMSetBlendState(_appearance->GetBlendState(), tempBF, 0xffffffff);
	}

	// NOTE: We are assuming that the constant buffers and all other draw setup has already taken place
	// Set vertex and index buffers
	pImmediateContext->IASetVertexBuffers(0, 1, &_appearance->GetGeometryData()->vertexBuffer,
												&_appearance->GetGeometryData()->vertexBufferStride, 
												&_appearance->GetGeometryData()->vertexBufferOffset);

	pImmediateContext->IASetIndexBuffer(_appearance->GetGeometryData()->indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	pImmediateContext->DrawIndexed(_appearance->GetGeometryData()->numberOfIndices, 0, 0);
}
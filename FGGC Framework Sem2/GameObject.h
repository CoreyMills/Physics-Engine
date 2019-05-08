#pragma once
#include "Transform.h"
#include "ParticleModel.h"
#include "Appearance.h"
#include "CarBody.h"

#include "OctaTree.h"
//class OctaTree;

class GameObject
{
public:
	GameObject(std::string name, Appearance* appearance, Transform* transform, ParticleModel* particleModel);
	GameObject(GameObject* objCopy);
	~GameObject();

	std::string GetName() const { return _name; }
	void SetName(std::string name) { _name = name; }

	Transform* GetTransform() { return _transform; }
	void SetTransform(Transform* transform) { _transform = transform; }

	ParticleModel* GetParticleModel() { return _particleModel; }
	void SetParticleModel(ParticleModel* particleModel) { _particleModel = particleModel; }

	Appearance* GetAppearance() const { return _appearance; }
	void SetAppearance(Appearance* appearance) { _appearance = appearance; }

	std::vector<Node*> GetQuads() { return _quads; }
	void SetQuads(std::vector<Node*> quads) { _quads = quads; }
	//void AddQuad(Node& quad) { _quads.push_back(quad); }

	void SetParent(GameObject * parent) { _parent = parent; }

	virtual void Update(float deltaTime);
	virtual void Draw(ID3D11DeviceContext * pImmediateContext);

	void Reset();

private:
	Transform* _transform;

	ParticleModel* _particleModel;

	Appearance* _appearance;

	std::string _name;

	GameObject* _parent; 

	//octaTree quads this obj is inside 
	std::vector<Node*> _quads;
};
#include "OctaTree.h"
#include "GameObject.h"

OctaTree::OctaTree(Node* head, bool onlyStatic, int maxObjectsPerQuad)
{
	_head = head;
	_maxObjectsPerQuad = maxObjectsPerQuad;
	_onlyStatic = onlyStatic;

	debugCount = 0;
}

OctaTree::~OctaTree()
{
	Reset();

	_head = nullptr;
	delete _head;
}

void OctaTree::CreateTree(std::vector<GameObject*> objects)
{
	if (objects.empty())
		return;

	if (!_head->children.empty())
		Reset();

	for (auto obj : objects)
	{
		if (obj->GetParticleModel()->IsStatic())
			_head->objectsInside.push_back(obj);
	}

	CheckSubDivisions(_head);
}

void OctaTree::AddGameObject(GameObject * object)
{
	if (object->GetParticleModel()->IsStatic())
	{
		std::vector<Node*> temp = FindAllQuadsForGameObject(object, _head);

		for (auto node : temp)
		{
			node->objectsInside.push_back(object);
			CheckSubDivisions(node);
		}
	}
}

void OctaTree::CheckSubDivisions(Node* node)
{
	if (node->objectsInside.size() > _maxObjectsPerQuad)
	{
		//if(debugCount < 4)
			AddSubDivisions(node);
		
		//debugCount++;
		
		for (auto child : node->children)
		{
			CheckSubDivisions(child);
		}
	}
}

//debugging the tree
void OctaTree::DrawOctaTree(ID3D11DeviceContext* pImmediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb, Node* node, float deltaTime)
{
	node->debugCube->Update(deltaTime);

	GameObject* gameObject = node->debugCube;

	// Get render material
	Material* material = gameObject->GetAppearance()->GetMaterial();

	// Copy material to shader
	cb.surface.AmbientMtrl = material->ambient;
	cb.surface.DiffuseMtrl = material->diffuse;
	cb.surface.SpecularMtrl = material->specular;

	// Set world matrix
	cb.World = XMMatrixTranspose(gameObject->GetTransform()->GetWorldMatrix());

	// Set texture
	if (gameObject->GetAppearance()->HasTexture())
	{
		ID3D11ShaderResourceView * textureRV = gameObject->GetAppearance()->GetTextureRV();
		pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
		cb.HasTexture = 1.0f;
	}
	else
	{
		cb.HasTexture = 0.0f;
	}

	// Update constant buffer
	pImmediateContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);

	gameObject->Draw(pImmediateContext);

	if (!node->children.empty())
	{
		for (Node* child : node->children)
		{
			DrawOctaTree(pImmediateContext, constantBuffer, cb, child, deltaTime);
		}
	}
}

void OctaTree::DrawOctaTreeLeafs(ID3D11DeviceContext * pImmediateContext, ID3D11Buffer * constantBuffer, ConstantBuffer cb, Node * node, float deltaTime, int height)
{
	bool bypass = false;
	if (!node->children.empty())
	{
		for (Node* child : node->children)
		{
			if (child->height == height)
				bypass = true;

			DrawOctaTreeLeafs(pImmediateContext, constantBuffer, cb, child, deltaTime, height);
		}
	}

	//if (false || bypass)
	if (node->height == height || bypass)
	{
		node->debugCube->Update(deltaTime);

		GameObject* gameObject = node->debugCube;

		// Get render material
		Material* material = gameObject->GetAppearance()->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material->ambient;
		cb.surface.DiffuseMtrl = material->diffuse;
		cb.surface.SpecularMtrl = material->specular;

		// Set world matrix
		cb.World = XMMatrixTranspose(gameObject->GetTransform()->GetWorldMatrix());

		// Set texture
		if (gameObject->GetAppearance()->HasTexture())
		{
			ID3D11ShaderResourceView * textureRV = gameObject->GetAppearance()->GetTextureRV();
			pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
			cb.HasTexture = 1.0f;
		}
		else
		{
			cb.HasTexture = 0.0f;
		}

		// Update constant buffer
		pImmediateContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);

		gameObject->Draw(pImmediateContext);
	}
}

void OctaTree::AddSubDivisions(Node * node)
{
	GameObject* tempObj = new GameObject(node->debugCube);
	Transform* tempTransform = new Transform(*tempObj->GetTransform());
	tempTransform->SetScale(node->debugCube->GetTransform()->GetScale().x / 2);
	
	tempObj->SetTransform(tempTransform);
	tempObj->GetParticleModel()->SetTransform(tempTransform);

	Vector3 boxPos = node->debugCube->GetTransform()->GetPosition();
	float halfSize = tempTransform->GetScale().x;

	int tempHeight = node->height + 1;

	tempObj->GetTransform()->SetPosition(Vector3(boxPos.x + halfSize, boxPos.y + halfSize, boxPos.z + halfSize));
	node->children.push_back(new Node(tempHeight, new GameObject(tempObj)));

	tempObj->GetTransform()->SetPosition(Vector3(boxPos.x - halfSize, boxPos.y + halfSize, boxPos.z + halfSize));
	node->children.push_back(new Node(tempHeight, new GameObject(tempObj)));
	
	tempObj->GetTransform()->SetPosition(Vector3(boxPos.x + halfSize, boxPos.y + halfSize, boxPos.z - halfSize));
	node->children.push_back(new Node(tempHeight, new GameObject(tempObj)));

	tempObj->GetTransform()->SetPosition(Vector3(boxPos.x - halfSize, boxPos.y + halfSize, boxPos.z - halfSize));
	node->children.push_back(new Node(tempHeight, new GameObject(tempObj)));

	tempObj->GetTransform()->SetPosition(Vector3(boxPos.x + halfSize, boxPos.y - halfSize, boxPos.z + halfSize));
	node->children.push_back(new Node(tempHeight, new GameObject(tempObj)));

	tempObj->GetTransform()->SetPosition(Vector3(boxPos.x - halfSize, boxPos.y - halfSize, boxPos.z + halfSize));
	node->children.push_back(new Node(tempHeight, new GameObject(tempObj)));
	
	tempObj->GetTransform()->SetPosition(Vector3(boxPos.x + halfSize, boxPos.y - halfSize, boxPos.z - halfSize));
	node->children.push_back(new Node(tempHeight, new GameObject(tempObj)));
	
	tempObj->GetTransform()->SetPosition(Vector3(boxPos.x - halfSize, boxPos.y - halfSize, boxPos.z - halfSize));
	node->children.push_back(new Node(tempHeight, new GameObject(tempObj)));
	
	tempObj = nullptr;
	delete tempObj;

	tempTransform = nullptr;
	delete tempTransform;
	
	for (auto child : node->children)
	{
		for (auto obj : node->objectsInside)
		{
			if (BoundingBoxCollision(obj->GetParticleModel()->GetAxisAlignedRect3World(), child->debugCube->GetParticleModel()->GetAxisAlignedRect3World()))
			{
				child->objectsInside.push_back(obj);
			}
		}
	}

	node->objectsInside.clear();
}

std::vector<Node*> OctaTree::FindAllQuadsForGameObject(GameObject * object, Node* node)
{
	std::vector<Node*> quads;

	Node* temp = new Node();
	bool continueChecking = true;

	while (continueChecking)
	{
		temp = FindQuadForGameObject(object, node, quads);
		
		if (temp)
		{
			quads.push_back(temp);
		}
		else
			continueChecking = false;
	}

	return quads;
}

Node* OctaTree::FindQuadForGameObject(GameObject * object, Node* node, std::vector<Node*> quadsToIgnore)
{
	//if(PointInBoxCollision(object->GetTransform()->GetPosition(), node->boundingBox))
	if(BoundingBoxCollision(object->GetParticleModel()->GetAxisAlignedRect3World(), node->debugCube->GetParticleModel()->GetAxisAlignedRect3World()))
	{
		Node* returnNode = node;
		if (!node->children.empty())
		{
			for (unsigned int i = 0; i < node->children.size(); i++)
			{
				Node* temp = FindQuadForGameObject(object, node->children.at(i), quadsToIgnore);
				if (temp)
				{
					returnNode = temp;
					bool found = false;

					for (Node* quad : quadsToIgnore)
					{
						if (quad == returnNode)
						{
							found = true;
						}
					}

					if(!found)
						return returnNode;
				}
			}
		}

		for (Node* quad : quadsToIgnore)
		{
			if (quad == returnNode)
			{
				return nullptr;
			}
		}
		return returnNode;
	}
	return nullptr;
}

std::vector<CollisionPair*> OctaTree::GetCollisionPairs(GameObject * movingObj)
{
	std::vector<CollisionPair*> pairs;
	std::vector<Node*> nodes = FindAllQuadsForGameObject(movingObj, _head);

	for (auto node : nodes)
	{
		for (auto staticObj : node->objectsInside)
		{
			CollisionPair* pair = new CollisionPair(movingObj, staticObj);
			pairs.push_back(pair);
		}
	}
	
	return RemoveDuplicatePairs(pairs);
}

std::vector<CollisionPair*> OctaTree::RemoveDuplicatePairs(std::vector<CollisionPair*> _collisionPairs)
{
	if (_collisionPairs.empty())
		return _collisionPairs;

	for (unsigned int i = 0; i < _collisionPairs.size() - 1; i++)
	{ 
		bool duplicateFound = false;
		for (unsigned int j = i + 1; j < _collisionPairs.size(); j++)
		{
			if (_collisionPairs.at(i)->firstObj->GetName() == _collisionPairs.at(j)->firstObj->GetName() &&
				_collisionPairs.at(i)->secondObj->GetName() == _collisionPairs.at(j)->secondObj->GetName())
			{
				_collisionPairs.erase(_collisionPairs.begin() + j);
				j--;
				duplicateFound = true;
			}
		}

		if (duplicateFound)
			i--;
	}

	return _collisionPairs;
}

int OctaTree::GetHeight(Node* node)
{
	if (!node)
		return 0;

	int maxHeight = node->height;
	for (auto child : node->children)
	{
		int height = GetHeight(child);
		if(height > maxHeight)
			maxHeight = height;
	}
	return maxHeight;
}

void OctaTree::Reset()
{
	if (!_head)
		return;

	Node* headCopy = _head;

	DeleteContents(_head);
	
	_head->debugCube = headCopy->debugCube;
}

void OctaTree::DeleteContents(Node* node)
{
	for (auto child : node->children)
	{
		DeleteContents(child);
	}

	if (!node->children.empty())
		node->children.clear();

	node->objectsInside.clear();
	
	node->debugCube = nullptr;
	delete node->debugCube;
}
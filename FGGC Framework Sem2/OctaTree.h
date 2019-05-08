#pragma once

#include "Common.h"
#include "Collision.h"

class GameObject;

struct Node
{
	int height;

	std::vector<GameObject*> objectsInside;

	//Order of children: tlb, trb, tlf, trf, blb, brb, blf, brf
	std::vector<Node*> children;

	//bounding box inside debugCube
	GameObject* debugCube;

	Node() 
	{
	}

	Node(int height, GameObject* cube)
	{
		this->height = height;
		this->debugCube = cube;
	}
};

struct CollisionPair
{
	GameObject* firstObj;
	GameObject* secondObj;

	CollisionPair(GameObject* a, GameObject* b)
	{
		firstObj = a;
		secondObj = b;
	}
};

class OctaTree
{
public:
	OctaTree(Node* head, bool onlyStatic, int maxObjectsPerQuad);
	~OctaTree();

	void DrawOctaTree(ID3D11DeviceContext * pImmediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb, Node* node, float deltaTime);
	void DrawOctaTreeLeafs(ID3D11DeviceContext * pImmediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb, Node* node, float deltaTime, int height);
	
	void OctaTree::CreateTree(std::vector<GameObject*> objects);
	void AddGameObject(GameObject* object);

	void CheckSubDivisions(Node* node);
	void AddSubDivisions(Node* node);

	std::vector<Node*> FindAllQuadsForGameObject(GameObject* object, Node* node);
	Node* FindQuadForGameObject(GameObject* object, Node* node, std::vector<Node*> quadsToIgnore);

	//returns array of collision pairs - no need to check for duplicates
	std::vector<CollisionPair*> GetCollisionPairs(GameObject* movingObj);
	std::vector<CollisionPair*> RemoveDuplicatePairs(std::vector<CollisionPair*> _collisionPairs);

	int GetHeight(Node* node);

	void Reset();
	void DeleteContents(Node* node);

	Node* GetHead() { return _head; }
	void SetHead(Node* newHead) { _head = newHead; }

private:
	Node* _head;

	unsigned int _maxObjectsPerQuad;
	bool _onlyStatic;

	int debugCount;
};
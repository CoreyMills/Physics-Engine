#pragma once
#include "Common.h"
#include "Transform.h"
#include "Quaternion.h"

struct CollisionResults
{
	bool collided;
	float pen;
	Vector3 pointOfContact;
	Vector3 dirOfReflection;

	CollisionResults()
	{
		collided = true;
	}

	CollisionResults(bool collided, float penetration, Vector3 pointOfContact, Vector3 dirOfReflection)
	{
		this->collided = collided;
		this->pen = penetration;
		this->pointOfContact = pointOfContact;
		this->dirOfReflection = dirOfReflection;
	}
};

inline bool SphereCollision(Vector3 aPos, float aRadius, Vector3 bPos, float bRadius)
{
	return (Vec3Dist(aPos, bPos) < (aRadius + bRadius));
}

inline bool BoundingBoxCollision(Rect3 a, Rect3 b)
{
	return (a.maxPoint.x >= b.minPoint.x && a.minPoint.x <= b.maxPoint.x &&
		a.maxPoint.y >= b.minPoint.y && a.minPoint.y <= b.maxPoint.y &&
		a.maxPoint.z >= b.minPoint.z && a.minPoint.z <= b.maxPoint.z);
}

inline bool PointInBoxCollision(Vector3 point, Rect3 box)
{
	return (point.x > box.minPoint.x && point.x < box.maxPoint.x &&
		point.y > box.minPoint.y && point.y < box.maxPoint.y &&
		point.z > box.minPoint.z && point.z < box.maxPoint.z);
}

inline bool LineCollision(Vector2 aStart, Vector2 aEnd, Vector2 bStart, Vector2 bEnd)
{
	return (aStart.x <= bEnd.x && bStart.x <= aEnd.x &&
		aStart.y <= bEnd.y && bStart.y <= aEnd.y);
}

//Ian Millington Cyclone Physics code- Used as reference for code below.
static inline Vector3 GetContactPoint(Vector3 &pOne,Vector3 &dOne,int oneSize,
						Vector3 &pTwo,Vector3 &dTwo,int twoSize,bool useOne)
{
	Vector3 toSt, cOne, cTwo;
	int dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
	int denom, mua, mub;

	smOne = (int)dOne.LengthSq();
	smTwo = (int)dTwo.LengthSq();
	dpOneTwo = (int)Dot(dTwo, dOne);

	toSt = pOne - pTwo;
	dpStaOne = (int)Dot(dOne, toSt);
	dpStaTwo = (int)Dot(dTwo, toSt);

	denom = smOne * smTwo - dpOneTwo * dpOneTwo;

	// Zero denominator indicates parrallel lines
	if (abs(denom) < 0.0001f) 
		return useOne ? pOne : pTwo;

	mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
	mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

	// If either of the edges has the nearest point out
	// of bounds, then the edges aren't crossed, we have
	// an edge-face contact. Our point is on the edge, which
	// we know from the useOne parameter.
	if (mua > oneSize || mua < -oneSize ||
		mub > twoSize || mub < -twoSize)
		return useOne ? pOne : pTwo;
	else
	{
		cOne = (pOne + dOne) * mua;
		cTwo = (pTwo + dTwo) * mub;
		return cOne * 0.5f + cTwo * 0.5f;
	}
}

inline CollisionResults RotatedBoundingBoxCollision(Rect3 a, Transform aTransform, Rect3 b, Transform bTransform)
{
	CollisionResults results;
	std::vector<Vector3> axisArray;
	std::vector<Vector3> aAxisArray;
	std::vector<Vector3> bAxisArray;

	std::vector<Vector3> aPoints;
	//each vertex of cube
	aPoints.push_back(a.maxPoint);
	aPoints.push_back(Vector3(a.maxPoint.x, a.maxPoint.y, a.minPoint.z));
	aPoints.push_back(Vector3(a.minPoint.x, a.maxPoint.y, a.minPoint.z));
	aPoints.push_back(Vector3(a.minPoint.x, a.maxPoint.y, a.maxPoint.z));
	aPoints.push_back(Vector3(a.minPoint.x, a.minPoint.y, a.maxPoint.z));
	aPoints.push_back(Vector3(a.maxPoint.x, a.minPoint.y, a.maxPoint.z));
	aPoints.push_back(Vector3(a.maxPoint.x, a.minPoint.y, a.minPoint.z));
	aPoints.push_back(a.minPoint);

	std::vector<Vector3> bPoints;
	bPoints.push_back(b.maxPoint);
	bPoints.push_back(Vector3(b.maxPoint.x, b.maxPoint.y, b.minPoint.z));
	bPoints.push_back(Vector3(b.minPoint.x, b.maxPoint.y, b.minPoint.z));
	bPoints.push_back(Vector3(b.minPoint.x, b.maxPoint.y, b.maxPoint.z));
	bPoints.push_back(Vector3(b.minPoint.x, b.minPoint.y, b.maxPoint.z));
	bPoints.push_back(Vector3(b.maxPoint.x, b.minPoint.y, b.maxPoint.z));
	bPoints.push_back(Vector3(b.maxPoint.x, b.minPoint.y, b.minPoint.z));
	bPoints.push_back(b.minPoint);

	XMMATRIX tempA = aTransform.GetRotationMatrix();
	aAxisArray.push_back(Vec3Normalization(Vector3(0, 1, 0) * tempA));
	aAxisArray.push_back(Vec3Normalization(Vector3(1, 0, 0) * tempA));
	aAxisArray.push_back(Vec3Normalization(Vector3(0, 0, 1) * tempA));

	XMMATRIX tempB = bTransform.GetRotationMatrix();
	bAxisArray.push_back(Vec3Normalization(Vector3(0, 1, 0) * tempB));
	bAxisArray.push_back(Vec3Normalization(Vector3(1, 0, 0) * tempB));
	bAxisArray.push_back(Vec3Normalization(Vector3(0, 0, 1) * tempB));

	//points
	axisArray.push_back(aAxisArray.at(0));
	axisArray.push_back(aAxisArray.at(1));
	axisArray.push_back(aAxisArray.at(2));

	axisArray.push_back(bAxisArray.at(0));
	axisArray.push_back(bAxisArray.at(1));
	axisArray.push_back(bAxisArray.at(2));

	//edges
	for (Vector3 aAxis : aAxisArray)
	{
		for (Vector3 bAxis : bAxisArray)
		{
			axisArray.push_back(Vec3Normalization(Cross(aAxis, bAxis)));
		}
	}

	Vector3 aCenter = a.minPoint + (a.minPoint - a.maxPoint * 0.5f);
	Vector3 bCenter = b.minPoint + (b.minPoint - b.maxPoint * 0.5f);

	int bestAxis = 0;
	int bestPointAxis = 0;
	float smallestOverlap = INT_MAX;
	Vector3 aClosestPoint, bClosestPoint;
	std::vector<Vector3> currentClosestPoints;
	for (int i = 0; i < 4; i++)
	{
		currentClosestPoints.push_back(Vector3());
	}

	for (unsigned int i = 0; i < axisArray.size(); i++)
	{
		// 01 obj1, 23 obj2

		if (axisArray.at(i).IsZero())
			continue;

		float obj1Min = INT_MAX;
		float obj1Max = -INT_MAX;

		float obj2Min = INT_MAX;
		float obj2Max = -INT_MAX;

		for (unsigned int j = 0; j < aPoints.size(); j++)
		{
			float scalarProjection = Dot(axisArray.at(i), aPoints.at(j) * aTransform.GetWorldMatrix());

			if (scalarProjection < obj1Min)
			{
				currentClosestPoints.at(0) = aPoints.at(j) * aTransform.GetWorldMatrix();
				obj1Min = scalarProjection;
			}

			if (scalarProjection > obj1Max)
			{
				currentClosestPoints.at(1) = aPoints.at(j) * aTransform.GetWorldMatrix();
				obj1Max = scalarProjection;
			}
		}

		for (unsigned int j = 0; j < bPoints.size(); j++)
		{
			float scalarProjection = Dot(axisArray.at(i), bPoints.at(j) * bTransform.GetWorldMatrix());

			if (scalarProjection < obj2Min)
			{
				currentClosestPoints.at(2) = bPoints.at(j) * bTransform.GetWorldMatrix();
				obj2Min = scalarProjection;
			}

			if (scalarProjection > obj2Max)
			{
				currentClosestPoints.at(3) = aPoints.at(j) * bTransform.GetWorldMatrix();
				obj2Max = scalarProjection;
			}
		}

		if (!(obj1Min <= obj2Max && obj1Max >= obj2Min))
		{
			results.collided = false;
			return results;
		}

		float overlap = fminf(obj1Max, obj2Max) - fmaxf(obj1Min, obj2Min);
		//overlap = fmaxf(fminf(obj1Max, obj2Max) - fmaxf(obj1Min, obj2Min), 0.01f);
		if (overlap < smallestOverlap)
		{
			aClosestPoint = currentClosestPoints.at(0);
			bClosestPoint = currentClosestPoints.at(2);
			if ((currentClosestPoints.at(0) - bTransform.GetPosition()).Length() > (currentClosestPoints.at(1) - bTransform.GetPosition()).Length())
			{
				aClosestPoint = currentClosestPoints.at(1);
			}

			if ((currentClosestPoints.at(2) - aTransform.GetPosition()).Length() < (currentClosestPoints.at(3) - aTransform.GetPosition()).Length())
			{
				bClosestPoint = currentClosestPoints.at(3);
			}

			smallestOverlap = overlap;
			bestAxis = i;

			if (i < 6)
				bestPointAxis = i;
		}
	}

	Vector3 toCenter = bTransform.GetPosition() - aTransform.GetPosition();
	Vector3 normal = axisArray.at(bestAxis);
	if (Dot(normal, toCenter) > 0)
		normal = normal * -1.0f;

	results.dirOfReflection = normal;
	results.pen = smallestOverlap;

	//minAxis on obj1
	if (bestAxis < 3)
	{
		for (unsigned int i = 0; i < 3; i++)
		{
			if (Vec3Normalization(normal) == axisArray.at(i + 3) ||
				Vec3Normalization(normal) == axisArray.at(i + 3) * 0.1f)
			{
				results.pointOfContact = aTransform.GetPosition() + (normal * aTransform.GetScale());
				return results;
			}
		}

		results.pointOfContact = bClosestPoint - (normal * smallestOverlap);
	}
	//minAxis on obj2
	else if (bestAxis < 6)
	{
		for (unsigned int i = 0; i < 3; i++)
		{
			if (Vec3Normalization(normal) == axisArray.at(i) ||
				Vec3Normalization(normal) == axisArray.at(i) * 0.1f)
			{
				results.pointOfContact = aTransform.GetPosition() + (normal * aTransform.GetScale());
				return results;
			}
		}

		results.pointOfContact = aClosestPoint - (normal * smallestOverlap);
	}
	//edge collision
	else
	{
		bestAxis -= 6;
		unsigned int aIndex = bestAxis / 3;
		unsigned int bIndex = bestAxis % 3;

		Vector3 aAxis = aAxisArray.at(aIndex);
		Vector3 bAxis = bAxisArray.at(bIndex);

		Vector3 axis = Cross(aAxis, bAxis);
		axis.Normalise();

		if (Dot(axis, toCenter) > 0)
			axis = axis * -1.0f;

		Vector3 ptOnOneEdge = a.maxPoint;
		Vector3 ptOnTwoEdge = b.maxPoint;

		float aAxisHalfSize = 0, bAxisHalfSize = 0;

		//ptOneEdge
		if (0 == aIndex)
		{
			aAxisHalfSize = a.maxPoint.x;
			ptOnOneEdge.x = 0;
		}
		else if (Dot(aAxisArray.at(0), axis) > 0)
			ptOnOneEdge.x = -ptOnOneEdge.x;

		if (1 == aIndex)
		{
			aAxisHalfSize = a.maxPoint.y;
			ptOnOneEdge.y = 0;
		}
		else if (Dot(aAxisArray.at(1), axis) > 0)
			ptOnOneEdge.y = -ptOnOneEdge.y;

		if (2 == aIndex)
		{
			aAxisHalfSize = a.maxPoint.x;
			ptOnOneEdge.z = 0;
		}
		else if (Dot(aAxisArray.at(2), axis) > 0)
			ptOnOneEdge.z = -ptOnOneEdge.z;

		//ptTwoEdge
		if (0 == aIndex)
		{
			bAxisHalfSize = b.maxPoint.x;
			ptOnTwoEdge.x = 0;
		}
		else if (Dot(bAxisArray.at(0), axis) > 0)
			ptOnTwoEdge.x = -ptOnTwoEdge.x;

		if (1 == aIndex)
		{
			bAxisHalfSize = b.maxPoint.y;
			ptOnTwoEdge.y = 0;
		}
		else if (Dot(bAxisArray.at(1), axis) > 0)
			ptOnTwoEdge.y = -ptOnTwoEdge.y;

		if (2 == aIndex)
		{
			bAxisHalfSize = b.maxPoint.z;
			ptOnTwoEdge.z = 0;
		}
		else if (Dot(bAxisArray.at(2), axis) > 0)
			ptOnTwoEdge.z = -ptOnTwoEdge.z;

		ptOnOneEdge = ptOnOneEdge * aTransform.GetWorldMatrix();
		ptOnTwoEdge = ptOnTwoEdge * bTransform.GetWorldMatrix();

		//set results
		results.dirOfReflection = axis;
		results.pointOfContact = GetContactPoint(ptOnOneEdge, aAxis, (int)aAxisHalfSize, 
									ptOnTwoEdge, bAxis, (int)bAxisHalfSize, bestPointAxis > 2);
	}

	return results;
}
#pragma once
#include "transform.h"
#include "Collider.h"
#include "Math.h"
#include <cmath>

class AABBCollider;
class PlaneCollider;

class SphereCollider : public Collider
{
	float radius = 1.0f;

public:
	SphereCollider(Transform* tf, float r) : Collider(tf) { radius = r; }

	bool CollidesWith(Collider& other) override { return other.CollidesWith(*this); }
	bool CollidesWith(SphereCollider& other) override;
	bool CollidesWith(AABBCollider& other) override;
	bool CollidesWith(PlaneCollider& other) override;

	void Collide(SphereCollider* other) override;
	void Collide(AABBCollider* other) override;
	void Collide(PlaneCollider* other) override;

	float GetColliderType() override { return 1.0f; }

	float GetRadius() override { return radius; }
};

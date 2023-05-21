#pragma once
#include "transform.h"
#include "Math.h"

class SphereCollider;
class AABBCollider;
class PlaneCollider;

class Collider abstract
{
protected:
	Transform* _tf;
private:
	bool _grounded = false;
public:
	Math _math;

	bool IsGrounded();
	void SetGrounded();
	void SetGrounded(bool a);

	Collider(Transform* tf) { _math = Math(); _tf = tf; _grounded = false; }

	virtual bool CollidesWith(Collider& other) = 0;
	virtual bool CollidesWith(SphereCollider& other) = 0;
	virtual bool CollidesWith(AABBCollider& other) = 0;
	virtual bool CollidesWith(PlaneCollider& other) = 0;

	virtual Vector3 GetMin();
	virtual Vector3 GetMax();

	virtual float GetRadius();

	virtual Vector3 GetNormal();

	float GetDistance();

	virtual float GetColliderType() = 0;

	Vector3 GetPosition() const { return _tf->GetPosition(); }
};
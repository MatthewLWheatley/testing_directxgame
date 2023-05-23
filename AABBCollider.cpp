#include "AABBCollider.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "Math.h"
#include "RigidBody.h"

AABBCollider::AABBCollider(Transform* tf, const Vector3& min, const Vector3& max) : Collider(tf) 
{
    _min = min;  
    _max = max;
}

void AABBCollider::Collide(SphereCollider* other)
{
	other->Collide(this);
}

void AABBCollider::Collide(AABBCollider* other)
{
	//DebugPrintF("cube and cube");
	Vector3 pos1 = GetPosition();
	Vector3 aMin = pos1 + GetMin();
	Vector3 aMax = pos1 + GetMax();
	//DebugPrintF("\ncube 1: pos %f %f %f \nmin  %f %f %f \nmax  %f %f %f", pos1.x, pos1.y, pos1.z, aMin.x, aMin.y, aMin.z, aMax.x, aMax.y, aMax.z);
	Vector3 pos2 = other->GetPosition();
	Vector3 bMin = pos2 + other->GetMin();
	Vector3 bMax = pos2 + other->GetMax();
	//DebugPrintF("\ncube 1: pos %f %f %f\n min  %f %f %f\n max  %f %f %f", pos2.x, pos2.y, pos2.z, bMin.x, bMin.y, bMin.z, bMax.x, bMax.y, bMax.z);
	float xOverlap = min(aMax.x, bMax.x) - max(aMin.x, bMin.x);
	float yOverlap = min(aMax.y, bMax.y) - max(aMin.y, bMin.y);
	float zOverlap = min(aMax.z, bMax.z) - max(aMin.z, bMin.z);

	Vector3 collisionNormal = Vector3(0, 0, 0);

	//DebugPrintF("\ncollision norm %f %f %f", collisionNormal.x, collisionNormal.y, collisionNormal.z);

	if (xOverlap < yOverlap && xOverlap < zOverlap)
	{
		if (aMin.x < bMin.x) collisionNormal = Vector3(-1.0f, 0.0f, 0.0f);
		else collisionNormal = Vector3(1.0f, 0.0f, 0.0f);
	}
	else if (yOverlap < zOverlap)
	{
		if (aMin.y < bMin.y) collisionNormal = Vector3(0.0f, -1.0f, 0.0f);
		else collisionNormal = Vector3(0.0f, 1.0f, 0.0f);
	}
	else
	{
		if (aMin.z < bMin.z) collisionNormal = Vector3(0.0f, 0.0f, -1.0f);
		else collisionNormal = Vector3(0.0f, 0.0f, 1.0f);
	}

	float penetrationDepth = _math.Min(xOverlap, _math.Min(yOverlap, zOverlap));


	//DebugPrintF("\npenetrationDepth %f", penetrationDepth);

	float inverseMass1 = 1 / GetRigidBody()->GetMass();
	float inverseMass2 = 1 / other->GetRigidBody()->GetMass();

	if (penetrationDepth > 0.0f) {
		Vector3 correction = collisionNormal * penetrationDepth;
		correction *= (inverseMass1 / (inverseMass1 + inverseMass2));
		DebugPrintF("\n%f %f %f", correction.x, correction.y, correction.z);
		Vector3 position1 = GetPosition();
		Vector3 position2 = other->GetPosition();

		position1 += correction * inverseMass1;
		position2 -= correction * inverseMass2;

		SetPosition(position1);
		other->SetPosition(position2);
	}

	float restitution = 0.6f;
	Vector3 relativeVelocity = GetRigidBody()->GetVelocity() - other->GetRigidBody()->GetVelocity();
	float relativeSpeedAlongNormal = _math.Dot(relativeVelocity, collisionNormal);
	if (relativeSpeedAlongNormal < 0.0f)
	{
		float totalInverseMass = GetRigidBody()->GetInverseMass() + other->GetRigidBody()->GetInverseMass();
		if (totalInverseMass <= 0.0f) return;
		float impulseMagnitude = -(1.0f + restitution) * relativeSpeedAlongNormal / totalInverseMass;
		Vector3 impulse = impulseMagnitude * collisionNormal;
		GetRigidBody()->ApplyImpulse(impulse);
		other->GetRigidBody()->ApplyImpulse(-impulse);
	}
}

void AABBCollider::Collide(PlaneCollider* other)
{
	other->Collide(this);
}

bool AABBCollider::CollidesWith(AABBCollider& other)
{
    Vector3 aMin = GetPosition() + _min;
    Vector3 aMax = GetPosition() + _max;
    Vector3 bMin = other.GetPosition() + other._min;
    Vector3 bMax = other.GetPosition() + other._max;
	if ((aMin.x <= bMax.x && aMax.x >= bMin.x) &&
		(aMin.y <= bMax.y && aMax.y >= bMin.y) &&
		(aMin.z <= bMax.z && aMax.z >= bMin.z))
	{
		Collide(&other);
	}
    return 
        (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
        (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
        (aMin.z <= bMax.z && aMax.z >= bMin.z);
}

bool AABBCollider::CollidesWith(SphereCollider& other)
{
    Vector3 aMin = GetPosition() + _min;
    Vector3 aMax = GetPosition() + _max;

    Vector3 closestPoint = _math.Clamp(other.GetPosition(), aMin, aMax);
    float distanceSquared = _math.Distance(closestPoint, other.GetPosition())* _math.Distance(closestPoint, other.GetPosition());
	if (distanceSquared < other.GetRadius() * other.GetRadius())
	{
		Collide(&other);
	}
	return distanceSquared < other.GetRadius() * other.GetRadius();
}

bool AABBCollider::CollidesWith(PlaneCollider& other)
{
    return other.CollidesWith(*this);
}

Vector3 AABBCollider::GetMin()
{
    return _min;
}

Vector3 AABBCollider::GetMax()
{
    return _max; 
}

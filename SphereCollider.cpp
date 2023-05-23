#include "SphereCollider.h"
#include "AABBCollider.h"
#include "PlaneCollider.h"
#include "RigidBody.h"

void SphereCollider::Collide(SphereCollider* other)
{
	//DebugPrintF("sphere and sphere");
	Vector3 position1 = GetPosition();
	Vector3 position2 = other->GetPosition();
	float radius1 = GetRadius();
	float radius2 = other->GetRadius();

	Vector3 collisionNormal = position1 - position2;
	float distance = _math.Length(collisionNormal);
	float penetrationDepth = distance - radius1 - radius2;

	float inverseMass1 = 1 / GetRigidBody()->GetMass();
	float inverseMass2 = 1 / other->GetRigidBody()->GetMass();

	if (penetrationDepth < 0.0f) {
		// Objects are overlapping
		collisionNormal.Normalize();

		Vector3 correction = collisionNormal * penetrationDepth;
		correction *= (inverseMass1 / (inverseMass1 + inverseMass2));

		position1 -= correction * inverseMass1;
		position2 += correction * inverseMass2;

		SetPosition(position1);
		other->SetPosition(position2);
	}
	// Now you can apply the impulse as before
	float restitution = 0.8f;
	Vector3 relativeVelocity = GetRigidBody()->GetVelocity() - other->GetRigidBody()->GetVelocity();
	if (_math.Dot(collisionNormal, relativeVelocity) < 0.0f)
	{
		float vj = -(1 + restitution) * _math.Dot(collisionNormal, relativeVelocity);
		float inverseMass1 = 1 / GetRigidBody()->GetMass();
		float inverseMass2 = 1 / other->GetRigidBody()->GetMass();
		float j = vj / (inverseMass1 + inverseMass2);
		Vector3 impulse1 = inverseMass1 * j * collisionNormal;
		Vector3 impulse2 = -inverseMass2 * j * collisionNormal;
		GetRigidBody()->ApplyImpulse(impulse1);
		other->GetRigidBody()->ApplyImpulse(impulse2);
	}
}

void SphereCollider::Collide(AABBCollider* other)
{
	//DebugPrintF("sphere and cube\n");

	Vector3 aMin = other->GetPosition() + other->GetMin();
	Vector3 aMax = other->GetPosition() + other->GetMax();

	Vector3 closestPoint = _math.Clamp(GetPosition(), aMin, aMax);

	Vector3 collisionNormal = GetPosition() - closestPoint;
	collisionNormal = _math.Normalise(collisionNormal);

	Vector3 relativeVelocity = GetRigidBody()->GetVelocity() - other->GetRigidBody()->GetVelocity();

	if (_math.Dot(collisionNormal, relativeVelocity) <= 0.0f)
	{
		float sphereInverseMass = 1 / GetRigidBody()->GetMass();
		float aabbInverseMass = 1 / other->GetRigidBody()->GetMass();

		float penetrationDepth = GetRadius() - _math.Distance(GetPosition(), closestPoint);

		if (penetrationDepth > 0.0f) {
			const float percent = 0.2f;
			const float slop = 0.01f;
			Vector3 correction = max(penetrationDepth - slop, 0.0f) / (sphereInverseMass + aabbInverseMass) * percent * collisionNormal;

			Vector3 position1 = other->GetPosition();
			Vector3 position2 = GetPosition();

			position1 -= correction * sphereInverseMass;
			position2 += correction * aabbInverseMass;

			other->SetPosition(position1);
			SetPosition(position2);
		}

		float restitution = 0.8f;


		float impulseScalar = -(1 + restitution) * _math.Dot(collisionNormal, relativeVelocity) / (sphereInverseMass + aabbInverseMass);

		Vector3 sphereImpulse = impulseScalar * sphereInverseMass * collisionNormal;
		Vector3 aabbImpulse = -impulseScalar * aabbInverseMass * collisionNormal;

		GetRigidBody()->ApplyImpulse(sphereImpulse);
		other->GetRigidBody()->ApplyImpulse(aabbImpulse);
	}
}

void SphereCollider::Collide(PlaneCollider* other)
{
	other->Collide(this);
}


bool SphereCollider::CollidesWith(SphereCollider& other)
{
    // Calculate the distance between the two sphere colliders' positions
    float distance = _math.Distance(GetPosition(), other.GetPosition());

    // Check if the distance is less than the sum of their radii
	if (distance < (GetRadius() + other.GetRadius())) 
	{
		Collide(&other);
	}
    return distance < (GetRadius() + other.GetRadius());
}

bool SphereCollider::CollidesWith(AABBCollider& other)
{
    return other.CollidesWith(*this);
}

bool SphereCollider::CollidesWith(PlaneCollider& other)
{
    return other.CollidesWith(*this);
}


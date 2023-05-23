#include "Collider.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "PlaneCollider.h"


bool Collider::IsGrounded() 
{
    return _grounded;
}

void Collider::SetGrounded()
{}

void Collider::SetGrounded(bool a) 
{
   _grounded = a;
}

Vector3 Collider::GetMin()
{
    return Vector3();
}

Vector3 Collider::GetMax()
{
    return Vector3();
}

float Collider::GetRadius()
{
    return NULL;
}

Vector3 Collider::GetNormal()
{
    return Vector3(0,0,0);
}

float Collider::GetDistance()
{
    return 0.0f;
}

void Collider::SetRigidBody(RigidBody* rb)
{
    _rb = rb;
}

RigidBody* Collider::GetRigidBody()
{
    return _rb;
}

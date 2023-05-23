#include "RigidBody.h"

void RigidBody::AddForce(Vector3 force)
{
    _netforce += force;
}

Collider* RigidBody::GetCollider()
{
     return _collider; 
}

void RigidBody::ApplyImpulse(Vector3 impulse)
{
    _velocity += impulse / _mass;
}

void RigidBody::ApplyGravity()
{
    if (_useGravity)
    {
        //if (IsCollideable())
        //{
        //    if (GetCollider()->GetColliderType() == 2.0f)
        //    {
        //        DebugPrintF("\n gravity");
        //    }
        //}

        Vector3 gravityForce(0, -9.81f * _mass, 0);
        AddForce(gravityForce);

    }
}

void RigidBody::DragForce()
{
    const float rho = 1.2f;
    const float C_d = 0.1f;
    const float A = 1.0f;

    Vector3 velocityDirection = _velocity;
    velocityDirection.Normalize();

    float speedSquared = _velocity.Magnitude() * _velocity.Magnitude();

    Vector3 dragForce = 0.5f * rho * speedSquared * C_d * A * -velocityDirection;

    AddForce(dragForce);
}

void RigidBody::FrictionForce()
{
    const float u_k = .9f;

    float weight = _mass * 9.8f;
    Vector3 velocityDirection = _velocity;
    velocityDirection.Normalize();
    Vector3 frictionForce = u_k * weight * -velocityDirection;

    AddForce(frictionForce);
}

void RigidBody::Update(float deltaTime)
{
    DragForce();
    if (IsCollideable())
    {
        if (GetCollider()->IsGrounded())
        {
            FrictionForce();
        }
    }
    ApplyGravity();

    _acceleration = _netforce / _mass;
    _velocity += _acceleration * deltaTime;

    // Reset netforce for the next update cycle
    _netforce = Vector3(0, 0, 0);
    _acceleration = Vector3(0, 0, 0);

    // Update position
    Vector3 currentPosition = _transform->GetPosition();
    _transform->SetPosition(currentPosition + _velocity * deltaTime);

    PhysicsModel::Update(deltaTime);
}

void RigidBody::SetVelocity(Vector3 velocity)
{
    _velocity = velocity;
}

void RigidBody::SetAcceleration(Vector3 acceleration)
{
}

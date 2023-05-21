#include "PhysicsModel.h"

PhysicsModel::PhysicsModel(Transform* transform)
{
    _transform = transform;
    _mass = 1.0f;
    _velocity = Vector3(0, 0, 0);
    _acceleration = Vector3(0, 0, 0);
    _netforce = Vector3(0, 0, 0);
    _useGravity = false;
}

void PhysicsModel::AddForce(Vector3 force)
{
    _netforce += force;
}

void PhysicsModel::ApplyImpulse(Vector3 impulse)
{
    _velocity += impulse / _mass;
}

void PhysicsModel::ApplyGravity()
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

void PhysicsModel::DragForce()
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

void PhysicsModel::FrictionForce()
{
    const float u_k = .9f;

    float weight = _mass * 9.8f;
    Vector3 velocityDirection = _velocity;
    velocityDirection.Normalize();
    Vector3 frictionForce = u_k * weight * -velocityDirection;

    AddForce(frictionForce);
}

void PhysicsModel::Update(float deltaTime)
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
}

void PhysicsModel::SetVelocity(Vector3 velocity)
{
    _velocity = velocity;
}

#pragma once
#include "Transform.h"
#include <DirectXMath.h>
#include "Debug.h"
#include <sstream>
#include <iomanip>
#include <string>
#include "Collider.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "PlaneCollider.h"


class PhysicsModel
{
public:
    PhysicsModel(Transform* transform);
    virtual void Update(float deltaTime);
    void SetVelocity(Vector3 velocity);
    void SetAcceleration(Vector3 acceleration);

    void ApplyGravity();
    void DragForce();
    void FrictionForce();


    void SetAccelerationType() { _useConstantAcceleration = !_useConstantAcceleration; };
    void AddForce(Vector3 force);
    bool GetAccelerationType() { return _useConstantAcceleration; };
    Vector3 GetVelocity() const { return _velocity; };
    Vector3 GetAcceleration() const { return _acceleration; };
    float GetMass() const { return _mass; };

    float GetInverseMass() const { return 1 / _mass; };

    void UseConstantAcceleration(bool use) { _useConstantAcceleration = use; }
    void ToggleGravity() { _useGravity = !_useGravity; }
    void ToggleGravity(bool a) { _useGravity = a; }
    bool IsGravity() { return _useGravity; }

    bool IsCollideable() const { return _collider != nullptr;  }
    Collider* GetCollider() const { return _collider; }
    void SetCollider(Collider* collider) { _collider = collider; }
     
    void ApplyImpulse(Vector3 impulse);

protected:
    Transform* _transform;

    Vector3 _velocity;
    Vector3 _acceleration;
    Vector3 _netforce;

    float _mass;
    bool _useConstantAcceleration = false;
    bool _useGravity = true;

    Collider* _collider = nullptr;
};

#pragma once
#include "Vector3.h"
#include "Collider.h"
#include "Math.h"


class SphereCollider;
class PlaneCollider;

class AABBCollider : public Collider
{
    Vector3 _min;
    Vector3 _max;


    bool _grounded = false;
public:
    AABBCollider(Transform* tf, const Vector3& min, const Vector3& max);

    bool CollidesWith(Collider& other) override { return other.CollidesWith(*this); }
    bool CollidesWith(AABBCollider& other) override;
    bool CollidesWith(SphereCollider& other) override;
    bool CollidesWith(PlaneCollider& other) override;

    float GetColliderType() override { return 2.0f; }

    void Collide(SphereCollider* other) override;
    void Collide(AABBCollider* other) override;
    void Collide(PlaneCollider* other) override;

    bool IsGrounded() const { return _grounded; }
    void SetGrounded() { _grounded = !_grounded; }
    void SetGrounded(bool a) { _grounded = a; }

    Vector3 GetMin();
    Vector3 GetMax();
};

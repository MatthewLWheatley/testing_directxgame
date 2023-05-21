#pragma once
#include "Vector3.h"
#include "Collider.h"
#include "Math.h"

class SphereCollider;
class AABBCollider;

class PlaneCollider : public Collider
{
    Vector3 _normal;
    float _distance;
    Vector3 _min;
    Vector3 _max;

public:
    PlaneCollider(Transform* tf, const Vector3& normal, float distance, const Vector3& min, const Vector3& max);

    bool CollidesWith(Collider& other) override { return other.CollidesWith(*this); }
    bool CollidesWith(SphereCollider& other) override;
    bool CollidesWith(AABBCollider& other) override;
    bool CollidesWith(PlaneCollider& other) override;

    float GetColliderType() override { return 3.0f; }

    Vector3 GetMin() const { return _min; }
    Vector3 GetMax() const { return _max; }


    Vector3 GetNormal() override { return _normal; }
    float GetDistance() const { return _distance; }
};
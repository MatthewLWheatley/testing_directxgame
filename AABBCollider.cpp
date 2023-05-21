#include "AABBCollider.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "Math.h"

AABBCollider::AABBCollider(Transform* tf, const Vector3& min, const Vector3& max) : Collider(tf) 
{
    _min = min;  
    _max = max;
}

bool AABBCollider::CollidesWith(AABBCollider& other)
{
    Vector3 aMin = GetPosition() + _min;
    Vector3 aMax = GetPosition() + _max;
    Vector3 bMin = other.GetPosition() + other._min;
    Vector3 bMax = other.GetPosition() + other._max;
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

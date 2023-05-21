#include "PlaneCollider.h"
#include "SphereCollider.h"
#include "AABBCollider.h"

PlaneCollider::PlaneCollider(Transform* tf, const Vector3& normal, float distance, const Vector3& min, const Vector3& max) : Collider(tf), _normal(normal), _distance(distance), _min(min), _max(max) { }

bool PlaneCollider::CollidesWith(SphereCollider& other)
{
    float distance = _math.Dot(_normal, other.GetPosition()) - _distance;
    return distance <= other.GetRadius();
}

bool PlaneCollider::CollidesWith(AABBCollider& other)
{
    Vector3 aMin = other.GetPosition() + other.GetMin();
    Vector3 aMax = other.GetPosition() + other.GetMax();

    Vector3 corners[8] = {
        Vector3(aMin.x, aMin.y, aMin.z),
        Vector3(aMin.x, aMin.y, aMax.z),
        Vector3(aMin.x, aMax.y, aMin.z),
        Vector3(aMin.x, aMax.y, aMax.z),
        Vector3(aMax.x, aMin.y, aMin.z),
        Vector3(aMax.x, aMin.y, aMax.z),
        Vector3(aMax.x, aMax.y, aMin.z),
        Vector3(aMax.x, aMax.y, aMax.z)
    };


    for (int i = 0; i < 8; i++)
    {
        float distance = _math.Dot(_normal, corners[i]) - _distance;

        if (distance <= 0.0f)
            return true;
    }

    return false;
}

bool PlaneCollider::CollidesWith(PlaneCollider& other)
{
    return false;
}

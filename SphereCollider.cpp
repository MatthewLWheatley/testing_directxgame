#include "SphereCollider.h"
#include "AABBCollider.h"
#include "PlaneCollider.h"

bool SphereCollider::CollidesWith(SphereCollider& other)
{
    // Calculate the distance between the two sphere colliders' positions
    float distance = _math.Distance(GetPosition(), other.GetPosition());

    // Check if the distance is less than the sum of their radii
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

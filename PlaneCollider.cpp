#include "PlaneCollider.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "RigidBody.h"

PlaneCollider::PlaneCollider(Transform* tf, const Vector3& normal, float distance, const Vector3& min, const Vector3& max) : Collider(tf), _normal(normal), _distance(distance), _min(min), _max(max) { }



void PlaneCollider::Collide(SphereCollider* other)
{
    Vector3 relativeVelocity = other->GetRigidBody()->GetVelocity();

    float dotProduct = _math.Dot(relativeVelocity, GetNormal());
    Vector3 reflection = relativeVelocity - 2.0f * dotProduct * GetNormal();

    float dampingFactor = 0.2f;
    Vector3 newVelocity = dampingFactor * reflection;
    //DebugPrintF("\n%f %f %f",newVelocity.x, newVelocity.y, newVelocity.z);
    Vector3 temp = other->GetRigidBody()->GetVelocity();
    temp.y = newVelocity.y;
    other->GetRigidBody()->SetVelocity(temp);


    float penetrationDepth = -(_math.Dot((other->GetPosition() - GetPosition()), GetNormal()) - other->GetRadius());
    //DebugPrintF("\n%f", penetrationDepth);
    if (penetrationDepth > 0.0f)
    {
        penetrationDepth = std::ceil(penetrationDepth * 100) / 100;
        //DebugPrintF("\n%f", penetrationDepth);
        Vector3 correction = penetrationDepth * GetNormal();
        Vector3 newPosition = other->GetPosition() + correction;
        other->SetPosition(newPosition);
    }

    other->SetGrounded(true);
}

void PlaneCollider::Collide(AABBCollider* other)
{
    Vector3 aMin = other->GetPosition() + other->GetMin();
    Vector3 aMax = other->GetPosition() + other->GetMax();

    Vector3 corners[4] = {
        Vector3(aMin.x, aMin.y, aMin.z),
        Vector3(aMin.x, aMin.y, aMax.z),
        Vector3(aMax.x, aMin.y, aMin.z),
        Vector3(aMax.x, aMin.y, aMax.z)
    };

    float distance = 0.0f;

    for (int i = 0; i < 4; i++)
    {
        distance = -_math.Dot(GetNormal(), corners[i]) - other->GetRadius() - 1;
        if (distance >= 0.00)
        {
            break;
        }
    }

    Vector3 correction = Vector3(0, distance, 0);
    Vector3 newPosition = other->GetPosition() + correction;
    other->SetPosition(newPosition);
    other->SetGrounded(true);
}

void PlaneCollider::Collide(PlaneCollider* other)
{

}

bool PlaneCollider::CollidesWith(SphereCollider& other)
{
    float distance = _math.Dot(_normal, other.GetPosition()) - _distance;
    if (distance <= other.GetRadius())
    {
        Collide(&other);
    }    
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
        {
            Collide(&other);
            return true;
        }
    }

    return false;
}

bool PlaneCollider::CollidesWith(PlaneCollider& other)
{
    return false;
}
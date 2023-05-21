#include "Math.h"
#include <cmath>

float Math::Distance(Vector3& a, Vector3& b)
{
    float deltaX = b.x - a.x;
    float deltaY = b.y - a.y;
    float deltaZ = b.z - a.z;

    return sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
}

float Math::Length(Vector3& a) 
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

Vector3 Math::Clamp(const Vector3& value, const Vector3& min, const Vector3& max)
{
    float x = value.x;
    float y = value.y;
    float z = value.z;

    x = (x > max.x) ? max.x : x;
    x = (x < min.x) ? min.x : x;

    y = (y > max.y) ? max.y : y;
    y = (y < min.y) ? min.y : y;

    z = (z > max.z) ? max.z : z;
    z = (z < min.z) ? min.z : z;

    return Vector3(x, y, z);
}

float Math::Dot(const Vector3& a, const Vector3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Math::Min(const Vector3& a, const Vector3& b)
{
    Vector3 temp;

    temp.x = (a.x < b.x) ? a.x : b.x;
    temp.y = (a.y < b.y) ? a.y : b.y;
    temp.z = (a.z < b.z) ? a.z : b.z;

    return temp;
}

float Math::Min(const float& a, const float& b)
{
    float temp = (a < b) ? a : b;
    return temp;
}

Vector3 Math::Max(const Vector3& a, Vector3& b)
{
    Vector3 temp;

    temp.x = (a.x > b.x) ? a.x : b.x;
    temp.y = (a.y > b.y) ? a.y : b.y;
    temp.z = (a.z > b.z) ? a.z : b.z;

    return temp;
}

float Math::Max(const float& a, const float& b)
{
    float temp = (a > b) ? a : b;
    return temp;
}

Vector3 Math::Normalise(const Vector3& a)
{
    float magnitude = sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z));

    if (magnitude == 0) return Vector3(0, 0, 0);

    return Vector3(a.x / magnitude, a.y / magnitude, a.z / magnitude);
}

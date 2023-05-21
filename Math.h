#pragma once
#include "Vector3.h"

class Math
{
public:
    static float Distance(Vector3& a, Vector3& b);
    float Length(Vector3& a);
    static Vector3 Clamp(const Vector3& value, const Vector3& min, const Vector3& max);
    static float Dot(const Vector3& a, const Vector3& b);
    static Vector3 Min(const Vector3& a, const Vector3& b);
    static float Min(const float& a, const float& b);
    static Vector3 Max(const Vector3& a, Vector3& b);
    static float Max(const float& a, const float& b);
    static Vector3 Normalise(const Vector3& a);
};
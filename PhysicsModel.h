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
    PhysicsModel(Transform* transform)
    {
        _transform = transform;
    };

    virtual void Update(float deltaTime);

protected:
    Transform* _transform;
};

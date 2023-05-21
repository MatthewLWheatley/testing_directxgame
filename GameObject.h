#pragma once

#include <string>
#include "Appearance.h"
#include "Transform.h"
#include "PhysicsModel.h"
#include "ParticleModel.h"

using namespace std;

class GameObject
{
public:
    GameObject(string type, Appearance appearance, Transform* transform);
    ~GameObject();

    string GetType() const { return _type; }

    Appearance GetAppearance() const { return _appearance; }

    Transform* GetTransform() const { return _transform; }

    PhysicsModel* GetPhysicsModel() const { return _physicsModel; }
    
    RigidBody* GetRigidBodyPhysicsModel() const
    {
        // Try to cast to RigidBody*
        RigidBody* rb = dynamic_cast<RigidBody*>(_physicsModel);

        // If cast is successful, rb is not null
        if (rb)
        {
            return rb;
        }
        else
        {
            // Handle error here
            return nullptr;
        }
    }
    PhysicsModel* SetPhysicsModel(PhysicsModel* pM) { _physicsModel = pM; }

    void Update(float deltaTime);
    void Draw(ID3D11DeviceContext* pImmediateContext);

    void setParent(GameObject* parent) { _parent = parent; }
    GameObject* GetParent() { return _parent; }

private:
    string _type;
    Appearance _appearance;
    Transform* _transform;
    PhysicsModel* _physicsModel;

    GameObject* _parent;
};


#include "GameObject.h"

GameObject::GameObject(std::string type, Appearance appearance, Transform* transform): _appearance(appearance), _transform(transform), _type(type)
{
	_physicsModel = new ParticleModel(transform);
}

GameObject::~GameObject()
{
	_transform = nullptr;
}


RigidBody* GameObject::GetRigidBodyPhysicsModel()
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

void GameObject::Update(float deltaTime)
{
    _physicsModel->Update(deltaTime);
    _transform->Update(deltaTime);
	_transform->UpdateWorldMatrix();
}

void GameObject::Draw(ID3D11DeviceContext* pImmediateContext)
{
	_appearance.Draw(pImmediateContext);
}

#include "GameObject.h"

GameObject::GameObject(std::string type, Appearance appearance, Transform* transform): _appearance(appearance), _transform(transform), _type(type)
{
	_physicsModel = new ParticleModel(transform);
}

GameObject::~GameObject()
{
	_transform = nullptr;
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

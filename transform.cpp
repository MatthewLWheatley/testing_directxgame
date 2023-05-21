#include "Transform.h"

Transform::Transform()
{
}

Transform::Transform(Vector3 position, Quaternion orientation, Vector3 scale)
{
	_orientation = orientation;
	_scale = scale;
	_position = position;
}

void Transform::Update(float deltaTime)
{
	// Calculate world matrix
	XMMATRIX scale = XMMatrixScaling(GetScale().x, GetScale().y, GetScale().z);
	XMMATRIX rotation = XMMatrixRotationX(GetRotation().x) * XMMatrixRotationY(GetRotation().y) * XMMatrixRotationZ(GetRotation().z);
	XMMATRIX translation = XMMatrixTranslation(GetPosition().x, GetPosition().y, GetPosition().z);
}
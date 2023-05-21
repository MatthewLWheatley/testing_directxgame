#pragma once

#include <directxmath.h>
#include <d3d11_4.h>
#include <string>
#include "Structures.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Debug.h"

using namespace DirectX;
using namespace std;

class Transform
{
public:
    Transform();
    //Transform(Vector3 position, Vector3 rotation, Vector3 scale);
    Transform(Vector3 position, Quaternion orientation, Vector3 scale);

    void SetPosition(Vector3 position) { _position = position; }
    void SetPosition(float x, float y, float z) { _position.x = x; _position.y = y; _position.z = z; }
    Vector3 GetPosition() const { return _position; }

    void SetScale(Vector3 scale) { _scale = scale; }
    void SetScale(float x, float y, float z) { _scale.x = x; _scale.y = y; _scale.z = z; }
    Vector3 GetScale() const { return _scale; }

    void SetOrientation(Quaternion orientation) { _orientation = orientation; }
    void SetOrientation(float x, float y, float z, float w) { _orientation.n = w; _orientation.v.x = x; _orientation.v.y = y; _orientation.v.z = z; }
    Quaternion GetOrientation() const { return _orientation; }

    void SetRotation(Vector3 rotation) { _orientation = MakeQFromEulerAngles(rotation.x, rotation.y, rotation.z); }
    Vector3 GetRotation() const { return MakeEulerAnglesFromQ(_orientation); }

    XMFLOAT4X4* GetWorld() const { return _world; };

    void UpdateWorldMatrix()
    {
        XMMATRIX world = XMMatrixIdentity();
        world *= XMMatrixScaling(_scale.x, _scale.y, _scale.z);
        XMFLOAT4 temp = XMFLOAT4( _orientation.v.x, _orientation.v.y, _orientation.v.z, _orientation.n);
        XMVECTOR quaternion = XMLoadFloat4(&temp);
        world *= XMMatrixRotationQuaternion(quaternion);
        world *= XMMatrixTranslation(_position.x, _position.y, _position.z);

        //XMStoreFloat4x4(_world, world);
    }

    XMMATRIX GetWorldMatrix()
    {
        XMMATRIX world = XMMatrixIdentity();
        world *= XMMatrixScaling(_scale.x, _scale.y, _scale.z); 
        XMFLOAT4 temp = XMFLOAT4(_orientation.v.x, _orientation.v.y, _orientation.v.z, _orientation.n);
        XMVECTOR quaternion = XMLoadFloat4(&temp);
        world *= XMMatrixRotationQuaternion(quaternion);
        world *= XMMatrixTranslation(_position.x, _position.y, _position.z);
        //XMStoreFloat4x4(_world, world);
        return world;
    }

    void Update(float deltaTime);
private:
    Quaternion _orientation;
    Vector3 _position;
    //Vector3 _rotation;
    Vector3 _scale;

    XMFLOAT4X4* _world;
};

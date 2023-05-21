#pragma once
#include "PhysicsModel.h"

class ParticleModel : public PhysicsModel
{
public:
    // Default constructor
    ParticleModel(Transform* transform) : PhysicsModel(transform) 
    { 
        _transform = transform; 
        _startPosition = Vector3(0,1,0); 
        _resetTime = 5.0f;
    };

    // Overloaded constructor for customization
    ParticleModel(Transform* transform, float resetTime, float pertubation, bool gravityInversion) : PhysicsModel(transform) { _transform = transform; _startPosition = transform->GetPosition(); };;

    void Update(float deltatime) override;

    void Reset();

private:
    Transform* _transform;
    float _timeAlive;
    Vector3 _startPosition;
    float _resetTime;
    float _pertubation;
    bool _gravityInversion;
};

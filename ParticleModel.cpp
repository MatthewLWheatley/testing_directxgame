#include "ParticleModel.h"

void ParticleModel::Update(float deltaTime) 
{
	_timeAlive += deltaTime;

	if (_resetTime <= _timeAlive) 
	{
		Reset();
	}

	PhysicsModel::Update(deltaTime);
}

void ParticleModel::Reset() 
{
	_transform->SetPosition(_startPosition);
	_timeAlive = 0.0f;
}
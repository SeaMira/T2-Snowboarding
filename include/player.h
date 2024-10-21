#pragma once

#include "MonaEngine.hpp"
#include "Rendering/DiffuseFlatMaterial.hpp"

class Player : public Mona::GameObject {
public:
	Player();
	~Player();

    virtual void UserStartUp(Mona::World& world) noexcept;

    virtual void UserUpdate(Mona::World& world, float timeStep) noexcept;

    
private:
    Mona::TransformHandle mTransform;

    friend class Camera;
};
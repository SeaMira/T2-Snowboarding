#pragma once

#include "MonaEngine.hpp"
#include "Rendering/DiffuseFlatMaterial.hpp"

class Player : public Mona::GameObject {
public:
	Player(glm::vec3 initPos);
	~Player();

    virtual void UserStartUp(Mona::World& world) noexcept;

    virtual void UserUpdate(Mona::World& world, float timeStep) noexcept;

    void keyPressed(Mona::World& world, float timeStep);

    void buttonPressed(Mona::World& world, float timeStep);

    
private:
    Mona::TransformHandle mTransform;
    glm::vec3 mInitPos;

    // keyboard
    float mSpeed = 10.0f;

    friend class Camera;
};
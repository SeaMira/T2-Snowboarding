#pragma once

#include "mesh_navigator.h"
#include "MonaEngine.hpp"
#include "Rendering/DiffuseFlatMaterial.hpp"

class Player : public Mona::GameObject {
public:
	Player(glm::vec3 initPos, MeshNavigator* meshNav);
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
    float mTimer = 1.0f;
    MeshNavigator* m_MeshNav;

    Triangle mPosT = Triangle(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f));
    friend class Camera;
};
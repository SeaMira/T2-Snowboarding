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

    void stopPlayer(Mona::World& world);
    void accelleratePlayer(Mona::World& world);

    glm::vec3 getPos();

    
private:
    Mona::TransformHandle mTransform;
    glm::vec3 mInitPos;

    glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);


    glm::vec3 velocity = glm::vec3(0.0f);
    const float mSpeed = 6.5f;
    const float mGlobalSpeed = 5.0f;
    const float slideSpeed = 6.5f;
    const float heightInterpolationSpeed = 1.0f;
    const float groundThreshold = 0.1f;

    const float rotationSpeed = 2.0f;  // Controla la velocidad de giro
    float acceleration = 1.0f;  // Controla qué tan rápido aumenta la velocidad
    const float maxAcceleration = 2.0f;  // Controla qué tan rápido aumenta la velocidad
    const float mbuffAcceleration = 4.0f;  // Controla qué tan rápido aumenta la velocidad
    const float deceleration = 0.9f;   // Controla qué tan rápido disminuye la velocidad al presionar "S"
    float reaccelerate = 1.0f;   // Controla qué tan rápido disminuye la velocidad al presionar "S"

    bool onFloor = false;
    bool stopped = false;

    float mStopTimer = 0.0f;
    float mAccTimer = 1.0f;
    MeshNavigator* m_MeshNav;

    std::shared_ptr<Mona::AudioClip> mAccelerationSound;
    std::shared_ptr<Mona::AudioClip> mSlideSound;
    std::shared_ptr<Mona::AudioClip> mCrashSound;

    friend class Camera;
};
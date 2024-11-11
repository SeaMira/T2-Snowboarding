#pragma once

#include "MonaEngine.hpp"
#include "Rendering/DiffuseFlatMaterial.hpp"
#include "player.h"


class Camera: public Mona::GameObject {
public:
    Camera(Mona::GameObjectHandle<Player> player, float range, float yaw, float pitch);
    ~Camera();

    virtual void UserStartUp(Mona::World& world) noexcept;

    virtual void UserUpdate(Mona::World& world, float timeStep) noexcept;

    void mouseMoved(Mona::World& world);
    void mouseWheelScrolled(Mona::World& world);

    void controllerButtonsPressed(Mona::World& world);
    void rightStickMoved(Mona::World& world);

    glm::vec3 calculateLookUp();
    glm::quat calculateRotation(const glm::vec3& pos, const glm::vec3& lookAt, const glm::vec3& lookUp);
    glm::vec3 calculateCameraPosition(const glm::vec3& lookAt);

    Mona::TransformHandle getTransform();


private:
    Mona::GameObjectHandle<Player> mPlayer;
    Mona::TransformHandle mPlayerTransform;
    Mona::TransformHandle mTransform;
    glm::vec3 mInitLookUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float mYaw, mPitch;
    float mCameraRange;
    float mCameraSpeed = 10.0f;

    float mZoomSensitivity = 1.0f;
    float mJoystickSensitiviy = 1.0f;
    float mMinDistance = 2.0f; 
    float mMaxDistance = 50.0f;

    // mouse
    bool mFirstMouse = true;
    glm::dvec2 mLastMousePosition = glm::dvec2(0.0, 0.0);
    float mSensitivity = 1.0f;

};


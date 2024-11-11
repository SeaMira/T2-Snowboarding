#include "camera.h"
#include <algorithm>

Camera::Camera(Mona::GameObjectHandle<Player> player, float range, float yaw, float pitch) :
    mPlayer(player), mCameraRange(range), mYaw(yaw), mPitch(pitch) {
    mPlayerTransform = mPlayer->mTransform;
}

Camera::~Camera() = default;

Mona::TransformHandle Camera::getTransform() {
    return mTransform;
}

void Camera::UserStartUp(Mona::World& world) noexcept {
    mTransform = world.AddComponent<Mona::TransformComponent>(*this, glm::vec3(0.0f));
    glm::vec3 cameraPos = calculateCameraPosition(mPlayerTransform->GetLocalTranslation());
    glm::vec3 cameraLookUp = calculateLookUp();
    glm::quat cameraRotation = calculateRotation(cameraPos, mPlayerTransform->GetLocalTranslation(), cameraLookUp);

    mTransform->SetTranslation(cameraPos);
    mTransform->SetRotation(cameraRotation);

    world.SetMainCamera(world.AddComponent<Mona::CameraComponent>(*this));
    world.SetAudioListenerTransform(mTransform);
}


void Camera::UserUpdate(Mona::World& world, float timeStep) noexcept {
    mouseMoved(world);
    mouseWheelScrolled(world);
    controllerButtonsPressed(world);
    rightStickMoved(world);

    glm::vec3 cameraPos = calculateCameraPosition(mPlayerTransform->GetLocalTranslation());
    glm::vec3 cameraLookUp = calculateLookUp();
    glm::quat cameraRotation = calculateRotation(cameraPos, mPlayerTransform->GetLocalTranslation(), cameraLookUp);

    mTransform->SetTranslation(cameraPos);
    mTransform->SetRotation(cameraRotation);
}


void Camera::mouseMoved(Mona::World& world) {
    auto& input = world.GetInput();
    glm::dvec2 mousePos = input.GetMousePosition();

    if (mFirstMouse) {
        mLastMousePosition = mousePos;
        mFirstMouse = false;
    }

    // Calcular el movimiento del mouse en píxeles
    glm::dvec2 mouseDelta = mousePos - mLastMousePosition;

    // Actualizar la última posición del mouse para el próximo cálculo
    mLastMousePosition = mousePos;

    // Ajustar el yaw y pitch de la cámara con el movimiento del mouse
    mYaw += static_cast<float>(mouseDelta.x) * mSensitivity;
    mPitch -= static_cast<float>(mouseDelta.y) * mSensitivity;

}

void Camera::mouseWheelScrolled(Mona::World& world) {
    auto& input = world.GetInput();
    glm::dvec2 mouseWheelOffset = input.GetMouseWheelOffset();

    // Ajustar la distancia usando la coordenada y del desplazamiento de la rueda
    mCameraRange += static_cast<float>(mouseWheelOffset.y) * mZoomSensitivity;

    // Clampear la distancia para que no sea demasiado cercana o lejana
    mCameraRange = std::clamp(mCameraRange, mMinDistance, mMaxDistance);
}


void Camera::controllerButtonsPressed(Mona::World& world) {
    auto& input = world.GetInput();

    if (input.GetGamepadAxisValue(MONA_JOYSTICK_1, MONA_GAMEPAD_AXIS_LEFT_TRIGGER) > 0.0f) {
        // Ajustar la distancia usando la coordenada y del desplazamiento de la rueda
        mCameraRange += static_cast<float>(input.GetGamepadAxisValue(MONA_JOYSTICK_1, MONA_GAMEPAD_AXIS_LEFT_TRIGGER)) * mZoomSensitivity;

        // Clampear la distancia para que no sea demasiado cercana o lejana
        mCameraRange = std::clamp(mCameraRange, mMinDistance, mMaxDistance);
    }
    if (input.GetGamepadAxisValue(MONA_JOYSTICK_1, MONA_GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.0f) {
        // Ajustar la distancia usando la coordenada y del desplazamiento de la rueda
        mCameraRange -= static_cast<float>(input.GetGamepadAxisValue(MONA_JOYSTICK_1, MONA_GAMEPAD_AXIS_RIGHT_TRIGGER)) * mZoomSensitivity;

        // Clampear la distancia para que no sea demasiado cercana o lejana
        mCameraRange = std::clamp(mCameraRange, mMinDistance, mMaxDistance);
    }
}


void Camera::rightStickMoved(Mona::World& world) {
    auto& input = world.GetInput();

    // Obtener los valores de los ejes de la palanca derecha
    float rightStickX = input.GetGamepadAxisValue(MONA_JOYSTICK_1, MONA_GAMEPAD_AXIS_RIGHT_X);
    float rightStickY = input.GetGamepadAxisValue(MONA_JOYSTICK_1, MONA_GAMEPAD_AXIS_RIGHT_Y);

    // Ajustar yaw y pitch usando los valores de los ejes de la palanca derecha
    if (std::abs(rightStickX) > 0.2f) mYaw += rightStickX * mJoystickSensitiviy;
    if (std::abs(rightStickY) > 0.2f) mPitch += rightStickY * mJoystickSensitiviy; // Invertido para que mover hacia arriba incremente el pitch

    // Limitar el ángulo de pitch para que la cámara no pase de los polos (-89 a 89 grados)
    mPitch = std::clamp(mPitch, -89.0f, 89.0f);

}


glm::quat Camera::calculateRotation(const glm::vec3& pos, const glm::vec3& lookAt, const glm::vec3& lookUp) {
    // Crear el quaternion para la rotación alrededor del eje Y (yaw)
    glm::quat qYaw = glm::angleAxis(glm::radians(mYaw), mInitLookUp);

    // Crear el quaternion para la rotación alrededor del eje X (pitch)
    glm::quat qPitch = glm::angleAxis(glm::radians(90.0f - mPitch), glm::vec3(-1.0f, 0.0f, 0.0f));

    // Combinar las rotaciones (la multiplicación de quaterniones aplica las rotaciones en orden)
    glm::quat rotation = qYaw * qPitch;
    return rotation;
}


glm::vec3 Camera::calculateLookUp() {
    // Calcular la dirección hacia el objetivo (desde la cámara al objetivo)
    glm::vec3 direction = glm::normalize(mPlayerTransform->GetLocalTranslation() - mTransform->GetLocalTranslation());
    //std::cout << "direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;
    // Calcular el nuevo 'up' como el perpendicular entre la dirección de la cámara y el eje Y
    glm::vec3 right = glm::normalize(glm::cross(mInitLookUp, direction));
    //std::cout << "right: " << right.x << ", " << right.y << ", " << right.z << std::endl;

    glm::vec3 lookUp = glm::normalize(glm::cross(direction, right));
    //std::cout << "lookUp: " << lookUp.x << ", " << lookUp.y << ", " << lookUp.z << std::endl;

    return lookUp;
}


glm::vec3 Camera::calculateCameraPosition(const glm::vec3& lookAt) {
    mPitch = std::clamp(mPitch, -89.0f, 89.0f);
    float yawRad = glm::radians(mYaw);
    float thetaRad = glm::radians(90.0f - mPitch);

    float x = lookAt.x + mCameraRange * std::sin(thetaRad) * std::sin(yawRad);
    float y = lookAt.y - mCameraRange * std::cos(thetaRad);
    float z = lookAt.z + mCameraRange * std::sin(thetaRad) * std::cos(yawRad);
    glm::vec3 cameraPosition(x, y, z);

    return cameraPosition;
}
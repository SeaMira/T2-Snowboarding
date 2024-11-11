#include "player.h"
#include <iostream>

Player::Player(glm::vec3 initPos, MeshNavigator* meshNav) : mInitPos(initPos), m_MeshNav(meshNav) {}

Player::~Player() = default;

glm::vec3 Player::getPos() {
	return mTransform->GetLocalTranslation();
}

void Player::stopPlayer() {
	reaccelerate = 0.0f;
	stopped = true;
	mStopTimer = 3.0f;
	velocity = glm::vec3(0.0f);
}

void Player::accelleratePlayer() {
	acceleration = mbuffAcceleration;
}

void Player::UserStartUp(Mona::World& world) noexcept {
	auto wallMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
	wallMaterial->SetDiffuseColor(glm::vec3(0.9f));

	auto& meshManager = Mona::MeshManager::GetInstance();
	mTransform = world.AddComponent<Mona::TransformComponent>(*this, mInitPos, glm::fquat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
	world.AddComponent<Mona::StaticMeshComponent>(*this, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), wallMaterial);

	auto& config = Mona::Config::GetInstance();
	std::filesystem::path terrain_p = config.getPathOfApplicationAsset("Models/scnd_snow_terrain_quad.obj");

	m_MeshNav->loadMeshToMap(terrain_p.string());
}

void Player::UserUpdate(Mona::World& world, float timeStep) noexcept {
	keyPressed(world, timeStep);
	buttonPressed(world, timeStep);
	
	Quad* q = m_MeshNav->getQuadAtPosition(mTransform->GetLocalTranslation().x, mTransform->GetLocalTranslation().z);
	mAccTimer += timeStep;
	acceleration = std::max(1.0f, acceleration - timeStep);

	mStopTimer -= timeStep;
	if (mStopTimer <= 0.0f) {
		stopped = false;
	}

	if (q != nullptr && !stopped) {
		reaccelerate = std::min(1.0f, reaccelerate + timeStep);
		// 1. Calculate quad normal and sliding force
		glm::vec3 quadNormal = q->calculateQuadNormal();

		// Definir el vector hacia arriba (normal al suelo plano)
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
		float angleRadians = glm::acos(glm::dot(glm::normalize(quadNormal), upVector));
		float angleDegrees = glm::degrees(angleRadians);

		glm::vec3 slideForce = glm::cross(quadNormal, glm::cross(gravity, quadNormal));

		// 2. Apply sliding force to horizontal velocity

		// 3. Enforce maximum horizontal speed
		glm::vec3 horizontalVelocity = glm::vec3(velocity.x, 0.0f, velocity.z);

		if (glm::length(horizontalVelocity) > mSpeed * mGlobalSpeed * acceleration * reaccelerate) {
			horizontalVelocity = glm::normalize(horizontalVelocity) * mSpeed * mGlobalSpeed * acceleration * reaccelerate;
			velocity.x = horizontalVelocity.x;
			velocity.z = horizontalVelocity.z;
		}

		// 4. Apply gravity to vertical velocity if airborne
		float currentY = mTransform->GetLocalTranslation().y;
		float groundY = q->getHeightAt(mTransform->GetLocalTranslation().x, mTransform->GetLocalTranslation().z);

		if (currentY <= groundY + groundThreshold) {
			velocity += slideForce * timeStep * slideSpeed * (angleDegrees / 45.0f) * (angleDegrees / 45.0f);
			onFloor = true;
			// On the ground: snap to ground and apply friction
			mTransform->SetTranslation(glm::vec3(mTransform->GetLocalTranslation().x, groundY, mTransform->GetLocalTranslation().z));
			float angleIn = glm::dot(horizontalVelocity, quadNormal);
			if (angleIn > 0.0f) {
				velocity.y = angleIn*2.0f;
			} else velocity.y = angleIn/90.0f;

			// Apply friction to the horizontal speed if on the ground
			velocity.x *= 0.99f; // Horizontal friction factor (adjust as needed)
			velocity.z *= 0.99f;
		}
		else {
			// In the air: let gravity continue acting on the player
			onFloor = false;
			velocity += gravity * 10.0f * timeStep;
		}

		// 5. Update player position based on the calculated velocity
		mTransform->Translate(velocity * timeStep);
	}

}


void Player::keyPressed(Mona::World& world, float timeStep) {
	auto& input = world.GetInput();
	
	if (onFloor && !stopped) {
		if ((input.IsKeyPressed(MONA_KEY_W) || input.IsKeyPressed(MONA_KEY_UP)) && mAccTimer > 1.0f) {
			acceleration = maxAcceleration;
			velocity *= acceleration ;
			mAccTimer = 0.0f;
		}
		if (input.IsKeyPressed(MONA_KEY_S) || input.IsKeyPressed(MONA_KEY_DOWN)) {
			velocity *= deceleration ;
		}
		if (input.IsKeyPressed(MONA_KEY_D) || input.IsKeyPressed(MONA_KEY_RIGHT)) {
			velocity = glm::rotateY(velocity, -rotationSpeed * timeStep);
		}
		if (input.IsKeyPressed(MONA_KEY_A) || input.IsKeyPressed(MONA_KEY_LEFT)) {
			velocity = glm::rotateY(velocity, rotationSpeed * timeStep);
		}

	}
	//if (moveDir.x != 0.0f || moveDir.y != 0.0f || moveDir.z != 0.0f) moveDir = glm::normalize(moveDir);
	//mTransform->Translate(moveDir * timeStep * mSpeed);

}

void Player::buttonPressed(Mona::World& world, float timeStep) {
	auto& input = world.GetInput();
	glm::vec3 moveDir(0.0f);
	float leftStickX = input.GetGamepadAxisValue(MONA_JOYSTICK_1, MONA_GAMEPAD_AXIS_LEFT_X);
	float leftStickY = input.GetGamepadAxisValue(MONA_JOYSTICK_1, MONA_GAMEPAD_AXIS_LEFT_Y);

	if (std::abs(leftStickX) > 0.2f) moveDir += glm::vec3(leftStickX, 0.0f, 0.0f);
	if (std::abs(leftStickY) > 0.2f) moveDir += glm::vec3(0.0f, 0.0f, leftStickY);

	mTransform->Translate(moveDir * timeStep * mSpeed);
}
#include "player.h"
#include <iostream>

Player::Player(glm::vec3 initPos, MeshNavigator* meshNav, float timer) : mInitPos(initPos), m_MeshNav(meshNav), game_timer(timer) {}

Player::~Player() = default;

glm::vec3 Player::getPos() {
	return mTransform->GetLocalTranslation();
}

void Player::stopPlayer(Mona::World& world) {
	reaccelerate = 0.0f;
	stopped = true;
	mStopTimer = 3.0f;
	velocity = glm::vec3(0.0f);
	world.PlayAudioClip3D(mCrashSound, mTransform->GetLocalTranslation(), 0.3f);
}

void Player::accelleratePlayer(Mona::World& world) {
	acceleration = mbuffAcceleration;
	world.PlayAudioClip3D(mAccelerationSound, mTransform->GetLocalTranslation(), 0.3f);
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

	auto& audioClipManager = Mona::AudioClipManager::GetInstance();
	mAccelerationSound = audioClipManager.LoadAudioClip(config.getPathOfApplicationAsset("Sounds/SFX/accel.wav"));
	mSlideSound = audioClipManager.LoadAudioClip(config.getPathOfApplicationAsset("Sounds/SFX/slide.wav"));
	mCrashSound = audioClipManager.LoadAudioClip(config.getPathOfApplicationAsset("Sounds/SFX/crash.wav"));
	mWinSound = audioClipManager.LoadAudioClip(config.getPathOfApplicationAsset("Sounds/APOGG/LifeIsFullOfJoy.wav"));

	/*ImGui::Begin("Snowboarding Info:");
	ImGui::Text("Speed: %.2f", game_timer); 
	ImGui::End();*/
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
	if (mTransform->GetLocalTranslation().z < -520.698f && !win) {
		win = true;
		world.PlayAudioClip3D(mWinSound, mTransform->GetLocalTranslation(), 0.3f);
	}

	if (q != nullptr && !stopped && !win) {
		reaccelerate = std::min(1.0f, reaccelerate + timeStep);
		// 1. Calculate quad normal and sliding force
		glm::vec3 quadNormal = q->calculateQuadNormal();

		// Definir el vector hacia arriba (normal al suelo plano)
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
		float angleRadians = glm::acos(glm::dot(glm::normalize(quadNormal), upVector));
		float angleDegrees = glm::degrees(angleRadians);

		glm::vec3 slideForce = glm::cross(quadNormal, glm::cross(gravity, quadNormal));

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
			if (!onFloor) world.PlayAudioClip3D(mSlideSound, mTransform->GetLocalTranslation(), 0.3f);
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
			world.PlayAudioClip3D(mAccelerationSound, mTransform->GetLocalTranslation(), 0.3f);
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
	if (input.IsKeyPressed(MONA_KEY_R)) {
		mTransform->SetTranslation(mInitPos);
		velocity = glm::vec3(0.0f);
	}
	//if (moveDir.x != 0.0f || moveDir.y != 0.0f || moveDir.z != 0.0f) moveDir = glm::normalize(moveDir);
	//mTransform->Translate(moveDir * timeStep * mSpeed);

}

void Player::buttonPressed(Mona::World& world, float timeStep) {
	auto& input = world.GetInput();
	if (onFloor) {
		float leftStickX = input.GetGamepadAxisValue(MONA_JOYSTICK_1, MONA_GAMEPAD_AXIS_LEFT_X);

		if (std::abs(leftStickX) > 0.2f) velocity = glm::rotateY(velocity, leftStickX * rotationSpeed * timeStep);

		if (input.IsGamepadButtonPressed(MONA_JOYSTICK_1, MONA_GAMEPAD_BUTTON_CROSS) && mAccTimer > 1.0f) {
			acceleration = maxAcceleration;
			velocity *= acceleration;
			mAccTimer = 0.0f;
			world.PlayAudioClip3D(mAccelerationSound, mTransform->GetLocalTranslation(), 0.3f);
		}
		if (input.IsGamepadButtonPressed(MONA_JOYSTICK_1, MONA_GAMEPAD_BUTTON_CIRCLE)) {
			velocity *= deceleration;
		}

	}
	
}
#include "player.h"

Player::Player(glm::vec3 initPos) : mInitPos(initPos) {}

Player::~Player() = default;

void Player::UserStartUp(Mona::World& world) noexcept {
	auto wallMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
	wallMaterial->SetDiffuseColor(glm::vec3(0.9f));

	auto& meshManager = Mona::MeshManager::GetInstance();
	mTransform = world.AddComponent<Mona::TransformComponent>(*this, mInitPos, glm::fquat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
	world.AddComponent<Mona::StaticMeshComponent>(*this, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), wallMaterial);
}

void Player::UserUpdate(Mona::World& world, float timeStep) noexcept {
	keyPressed(world, timeStep);
	buttonPressed(world, timeStep);
}


void Player::keyPressed(Mona::World& world, float timeStep) {
	auto& input = world.GetInput();
	glm::vec3 moveDir(0.0f, 0.0f, 0.0f);
	if (input.IsKeyPressed(MONA_KEY_W) || input.IsKeyPressed(MONA_KEY_UP)) {
		moveDir += glm::vec3(0.0f, 0.0f, -1.0f);
	}
	if (input.IsKeyPressed(MONA_KEY_S) || input.IsKeyPressed(MONA_KEY_DOWN)) {
		moveDir += glm::vec3(0.0f, 0.0f, 1.0f);
	}
	if (input.IsKeyPressed(MONA_KEY_D) || input.IsKeyPressed(MONA_KEY_RIGHT)) {
		moveDir += glm::vec3(1.0f, 0.0f, 0.0f);
	}
	if (input.IsKeyPressed(MONA_KEY_A) || input.IsKeyPressed(MONA_KEY_LEFT)) {
		moveDir += glm::vec3(-1.0f, 0.0f, 0.0f);
	}
	if (moveDir.x != 0.0f || moveDir.y != 0.0f || moveDir.z != 0.0f) moveDir = glm::normalize(moveDir);
	mTransform->Translate(moveDir * timeStep * mSpeed);

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
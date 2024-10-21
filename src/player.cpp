#include "player.h"

Player::Player() {}

Player::~Player() = default;

void Player::UserStartUp(Mona::World& world) noexcept {
	auto wallMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
	wallMaterial->SetDiffuseColor(glm::vec3(0.9f));

	auto& meshManager = Mona::MeshManager::GetInstance();
	mTransform = world.AddComponent<Mona::TransformComponent>(*this, glm::vec3(0.0f, 0.0f, 0.0f), glm::fquat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
	world.AddComponent<Mona::StaticMeshComponent>(*this, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), wallMaterial);
}

void Player::UserUpdate(Mona::World& world, float timeStep) noexcept {

}
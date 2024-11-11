#include "obstacle.h"


Obstacle::Obstacle(glm::vec3 initPos, Mona::GameObjectHandle<Player> player, float scale) : mInitPos(initPos), mPlayer(player), mScale(scale) {}
Obstacle::~Obstacle() = default;

void Obstacle::UserStartUp(Mona::World& world) noexcept {
	auto& meshManager = Mona::MeshManager::GetInstance();
	auto bodyMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
	bodyMaterial->SetDiffuseColor(glm::vec3(1.0f, 1.0f, 1.0f));

	auto noseMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
	noseMaterial->SetDiffuseColor(glm::vec3(0.8f, 0.0f, 0.0f));

	auto sphere1 = world.CreateGameObject<Mona::GameObject>();
	world.AddComponent<Mona::TransformComponent>(sphere1, mInitPos + glm::vec3(0.0f, 1.0f * mScale, 0.0f), glm::fquat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f * mScale));
	world.AddComponent<Mona::StaticMeshComponent>(sphere1, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Sphere), bodyMaterial);

	auto sphere2 = world.CreateGameObject<Mona::GameObject>();
	world.AddComponent<Mona::TransformComponent>(sphere2, mInitPos + glm::vec3(0.0f, 2.6f * mScale, 0.0f), glm::fquat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.6f * mScale));
	world.AddComponent<Mona::StaticMeshComponent>(sphere2, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Sphere), bodyMaterial);

	auto sphere3 = world.CreateGameObject<Mona::GameObject>();
	world.AddComponent<Mona::TransformComponent>(sphere3, mInitPos + glm::vec3(0.0f, 3.5f * mScale, 0.0f), glm::fquat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.3f * mScale));
	world.AddComponent<Mona::StaticMeshComponent>(sphere3, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Sphere), bodyMaterial);

	auto nose = world.CreateGameObject<Mona::GameObject>();
	world.AddComponent<Mona::TransformComponent>(nose, mInitPos + glm::vec3(0.0f, 3.5f * mScale, 0.3f*mScale), glm::fquat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.05f * mScale));
	world.AddComponent<Mona::StaticMeshComponent>(nose, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), noseMaterial);
}

void Obstacle::UserUpdate(Mona::World& world, float timeStep) noexcept {
	glm::vec3 playerPos = mPlayer->getPos();
	bool inX = (mInitPos.x - 1.0f * mScale <= playerPos.x) && (playerPos.x <= mInitPos.x + 1.0f * mScale);
	bool inZ = (mInitPos.z - 1.0f * mScale <= playerPos.z) && (playerPos.z <= mInitPos.z + 1.0f * mScale);
	bool inY = (mInitPos.y <= playerPos.y) && (playerPos.y <= mInitPos.y + 3.8f * mScale);
	if (inX && inY && inZ && mIsVisible) {
		mPlayer->stopPlayer();
		mIsVisible = false;
	}
}
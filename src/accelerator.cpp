#include "accelerator.h"

Accelerator::Accelerator(glm::vec3 initPos, Mona::GameObjectHandle<Player> player, float scale) : mInitPos(initPos), mPlayer(player), mScale(scale) {}
Accelerator::~Accelerator() = default;

void Accelerator::UserStartUp(Mona::World& world) noexcept {
	auto& meshManager = Mona::MeshManager::GetInstance();
	auto postMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
	postMaterial->SetDiffuseColor(glm::vec3(0.0f, 0.0f, 1.0f));

	auto flagMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
	flagMaterial->SetDiffuseColor(glm::vec3(1.0f, 0.0f, 0.0f));
	

	auto post1 = world.CreateGameObject<Mona::GameObject>();
	world.AddComponent<Mona::TransformComponent>(post1, mInitPos + glm::vec3(-1.0f * mScale, 1.0f * mScale * postL, 0.0f), glm::fquat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(mScale / 10.0f, mScale * postL, mScale / 10.0f));
	world.AddComponent<Mona::StaticMeshComponent>(post1, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), postMaterial);
	
	auto post2 = world.CreateGameObject<Mona::GameObject>();
	world.AddComponent<Mona::TransformComponent>(post2, mInitPos + glm::vec3(1.0f * mScale, 1.0f * mScale * postL, 0.0f), glm::fquat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(mScale/10.0f, mScale * postL, mScale / 10.0f));
	world.AddComponent<Mona::StaticMeshComponent>(post2, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), postMaterial);
	
	auto flag = world.CreateGameObject<Mona::GameObject>();
	world.AddComponent<Mona::TransformComponent>(flag, mInitPos + glm::vec3(0.0f, 2.0f * mScale * postL - 0.3f * mScale, 0.0f), glm::fquat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(mScale, mScale * 0.3f, mScale));
	world.AddComponent<Mona::StaticMeshComponent>(flag, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Plane), flagMaterial);


}

void Accelerator::UserUpdate(Mona::World& world, float timeStep) noexcept {
	glm::vec3 playerPos = mPlayer->getPos();
	bool inX = (mInitPos.x - mScale <= playerPos.x) && (playerPos.x <= mInitPos.x + mScale);
	bool inZ = (mInitPos.z - mScale / 10.0f <= playerPos.z) && (playerPos.z <= mInitPos.z + mScale / 10.0f);
	bool inY = (mInitPos.y <= playerPos.y) && (playerPos.y <= mInitPos.y + 2.0f * mScale * postL - 0.3f * mScale);
	if (inX && inY && inZ && mIsVisible) {
		mPlayer->accelleratePlayer();
		mIsVisible = false;
	}
}
#include "MonaEngine.hpp"
#include "Rendering/DiffuseFlatMaterial.hpp"
#include "player.h"
#include "camera.h"

class Snowboarding : public Mona::Application
{
public:
	Snowboarding() = default;
	~Snowboarding() = default;
	virtual void UserStartUp(Mona::World& world) noexcept override {

		world.SetGravity(glm::vec3(0.0f, 0.0f, 0.0f));
		world.SetAmbientLight(glm::vec3(0.5f));
		auto player = world.CreateGameObject<Player>(glm::vec3(0.0f));
		auto camera = world.CreateGameObject<Camera>(player, 15.0f, 0.0f, 0.0f);

		auto wallMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
		wallMaterial->SetDiffuseColor(glm::vec3(1.0f, 0.0f, 0.0f));

		auto& meshManager = Mona::MeshManager::GetInstance();
		auto cube = world.CreateGameObject<Mona::GameObject>();
		Mona::TransformHandle transform = world.AddComponent<Mona::TransformComponent>(cube, glm::vec3(0.0f, 0.0f, -5.0f), glm::fquat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
		world.AddComponent<Mona::StaticMeshComponent>(cube, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), wallMaterial);
	}

	virtual void UserShutDown(Mona::World& world) noexcept override {
	}
	virtual void UserUpdate(Mona::World& world, float timeStep) noexcept override {
	}
};
int main()
{
	Snowboarding app;
	Mona::Engine engine(app);
	engine.StartMainLoop();

}
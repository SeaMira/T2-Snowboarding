#include "MonaEngine.hpp"
#include "Core/Config.hpp"
#include "Rendering/DiffuseFlatMaterial.hpp"
#include "Rendering/PBRTexturedMaterial.hpp"
#include "Rendering/UnlitTexturedMaterial.hpp"
#include "Rendering/DiffuseTexturedMaterial.hpp"
#include <imgui.h>
#include <iostream>
#include "player.h"
#include "camera.h"
#include "mesh_navigator.h"
#include "obstacle.h"
#include "accelerator.h"


void AddDirectionalLight(Mona::World& world, const glm::vec3& axis, float angle, float lightIntensity)
{
	auto light = world.CreateGameObject<Mona::GameObject>();
	auto transform = world.AddComponent<Mona::TransformComponent>(light);
	transform->Rotate(axis, angle);
	world.AddComponent<Mona::DirectionalLightComponent>(light, lightIntensity * glm::vec3(1.0f));

}

class Snowboarding : public Mona::Application
{
public:
	Snowboarding() = default;
	~Snowboarding() = default;
	virtual void UserStartUp(Mona::World& world) noexcept override {
		//Mona::SourceDirectoryData::SetSourceDirectory("../assets");
		auto& meshManager = Mona::MeshManager::GetInstance();
		auto& config = Mona::Config::GetInstance();
		auto& textureManager = Mona::TextureManager::GetInstance();


		// Setting Map
		world.SetBackgroundColor(0.1f, 0.1f, 1.0f, 1.0f);
		std::shared_ptr<Mona::PBRTexturedMaterial> terr_material = std::static_pointer_cast<Mona::PBRTexturedMaterial>(world.CreateMaterial(Mona::MaterialType::PBRTextured));
		std::shared_ptr<Mona::Texture> albedo = textureManager.LoadTexture(config.getPathOfApplicationAsset("albedo_scnd.png"));
		std::shared_ptr<Mona::Texture> normalMap = textureManager.LoadTexture(config.getPathOfApplicationAsset("normal_scnd.png"));
		std::shared_ptr<Mona::Texture> metallic = textureManager.LoadTexture(config.getPathOfApplicationAsset("metallic_scnd.png"));
		std::shared_ptr<Mona::Texture> roughness = textureManager.LoadTexture(config.getPathOfApplicationAsset("rough_scnd.png"));
		std::shared_ptr<Mona::Texture> ambientOcclusion = textureManager.LoadTexture(config.getPathOfApplicationAsset("AO_scnd.png"));
		terr_material->SetAlbedoTexture(albedo);
		terr_material->SetNormalMapTexture(normalMap);
		terr_material->SetMetallicTexture(normalMap);
		terr_material->SetRoughnessTexture(roughness);
		terr_material->SetAmbientOcclusionTexture(ambientOcclusion);
		auto map = world.CreateGameObject<Mona::GameObject>();
		Mona::TransformHandle mapTransform = world.AddComponent<Mona::TransformComponent>(map);
		float terr_scale = 50.0f;
		mapTransform->Scale(glm::vec3(terr_scale));
		std::filesystem::path terrain_p = config.getPathOfApplicationAsset("scnd_snow_terrain_T.obj");
		world.AddComponent<Mona::StaticMeshComponent>(map, meshManager.LoadMesh(terrain_p, true), terr_material);

		MeshNavigator* meshNav = new MeshNavigator(terrain_p.string(), terr_scale);

		// setting light and gravity
		world.SetGravity(glm::vec3(0.0f, 0.0f, 0.0f));
		glm::vec3 sunAxis(1.0f, 0.0f, 0.0f);
		float sunAngle = 45.0f;
		float sunIntensity = 4.0f;
		AddDirectionalLight(world, sunAxis, sunAngle, sunIntensity);
		world.SetAmbientLight(glm::vec3(0.9f));
		auto player = world.CreateGameObject<Player>(glm::vec3(5.14424, 18.117, -5.95871), meshNav);
		auto camera = world.CreateGameObject<Camera>(player, 15.0f, 0.0f, 0.0f);

		// setting cube for reference
		auto wallMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
		wallMaterial->SetDiffuseColor(glm::vec3(0.0f, 1.0f, 0.0f));
		auto cube = world.CreateGameObject<Mona::GameObject>();
		Mona::TransformHandle transform = world.AddComponent<Mona::TransformComponent>(cube, glm::vec3(0.0f, 0.0f, -5.0f), glm::fquat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
		world.AddComponent<Mona::StaticMeshComponent>(cube, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), wallMaterial);

		float obstacleScale = 2.0f;
		auto snowMan1 = world.CreateGameObject<Obstacle>(terr_scale * glm::vec3(-0.4318f, -0.2439f, -2.3424f), player, obstacleScale);
		auto snowMan2 = world.CreateGameObject<Obstacle>(terr_scale * glm::vec3(0.1363f, -0.459f, -4.0663f), player, obstacleScale);
		auto snowMan3 = world.CreateGameObject<Obstacle>(terr_scale * glm::vec3(-0.02272f, -0.4694f, -6.5291f), player, obstacleScale);
		auto snowMan4 = world.CreateGameObject<Obstacle>(terr_scale * glm::vec3(0.409f, -0.7656f, -6.5291f), player, obstacleScale);
		auto snowMan5 = world.CreateGameObject<Obstacle>(terr_scale * glm::vec3(-0.72727f, -0.8576f, -7.2679f), player, obstacleScale);
		auto snowMan6 = world.CreateGameObject<Obstacle>(terr_scale * glm::vec3(-0.7045f, -1.1168f, -9.3615f), player, obstacleScale);
		auto snowMan7 = world.CreateGameObject<Obstacle>(terr_scale * glm::vec3(0.6818f, -1.14718f, -9.6078f), player, obstacleScale);
		auto snowMan8 = world.CreateGameObject<Obstacle>(terr_scale * glm::vec3(0.01f, -1.22386f, -10.2235f), player, obstacleScale*4.0f);

		float acceleratorScale = 4.0f;
		auto arc1 = world.CreateGameObject<Accelerator>(terr_scale * glm::vec3(-0.28f, -0.4282f, -3.82f), player, acceleratorScale*2.0f);
		auto arc2 = world.CreateGameObject<Accelerator>(terr_scale * glm::vec3(-0.568, -0.704f, -6.0365f), player, acceleratorScale*2.0f);
		auto arc3 = world.CreateGameObject<Accelerator>(terr_scale * glm::vec3(0.568, -0.704f, -6.0365f), player, acceleratorScale*2.0f);
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
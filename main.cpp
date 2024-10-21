#include "MonaEngine.hpp"
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
		auto player = world.CreateGameObject<Player>();
		auto camera = world.CreateGameObject<Camera>(player, 15.0f, 0.0f, 0.0f);
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
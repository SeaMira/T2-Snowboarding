#pragma once

#include "player.h"
#include "MonaEngine.hpp"
#include "Rendering/DiffuseFlatMaterial.hpp"

class Accelerator : public Mona::GameObject {
public:
	Accelerator(glm::vec3 initPos, Mona::GameObjectHandle<Player> player, float scale);
	~Accelerator();

	virtual void UserStartUp(Mona::World& world) noexcept;

	virtual void UserUpdate(Mona::World& world, float timeStep) noexcept;

private:
	Mona::TransformHandle mTransform;
	Mona::GameObjectHandle<Player> mPlayer;
	glm::vec3 mInitPos;
	float mScale;
	float postL = 1.0f;

	bool mIsVisible = true;
};


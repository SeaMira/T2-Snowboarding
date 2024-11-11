#pragma once

#include "player.h"
#include "MonaEngine.hpp"
#include "Rendering/DiffuseFlatMaterial.hpp"

class Obstacle : public Mona::GameObject {
public:
	Obstacle(glm::vec3 initPos, Mona::GameObjectHandle<Player> player, float scale);
	~Obstacle();

	virtual void UserStartUp(Mona::World& world) noexcept;

	virtual void UserUpdate(Mona::World& world, float timeStep) noexcept;

private:
	Mona::TransformHandle mTransform;
	Mona::GameObjectHandle<Player> mPlayer;
	glm::vec3 mInitPos;
	float mScale;

	bool mIsVisible = true;
};


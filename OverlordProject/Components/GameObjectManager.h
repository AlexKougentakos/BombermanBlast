#pragma once

//One idea to remove the includes in the header and hence the templated function is just to split the function into
// multiple functions, one for each type of object. CreateBomb, CreateExplosion, CreatePowerUp, etc.
// This would also remove the need for the enum class ObjectType

//Include all the GameObjects here
#include "Prefabs/BombermanCharacter.h"
#include "Prefabs/Explosion.h"

template <typename T>
concept IsDerivedFromGameObject = std::is_base_of<GameObject, T>::value;

class GameObjectManager : public BaseComponent
{
public: 
	explicit GameObjectManager() = default;
	~GameObjectManager() override = default;

	GameObjectManager(const GameObjectManager& other) = delete;
	GameObjectManager(GameObjectManager&& other) noexcept = delete;
	GameObjectManager& operator=(const GameObjectManager& other) = delete;
	GameObjectManager& operator=(GameObjectManager&& other) noexcept = delete;


	template <IsDerivedFromGameObject ObjectType, typename... Args>
	ObjectType* CreateGameObject(Args&&... args)
	{
		GameObject* pGameObject = new ObjectType(std::forward<Args>(args)...);

		if (!pGameObject) return nullptr;

		m_pGameObjects.push_back(pGameObject);
		GetScene()->AddChild(pGameObject);

		return static_cast<ObjectType*>(pGameObject);
	}

	void RemoveGameObject(GameObject* pGameObject);
public:
	void Initialize(const SceneContext&) override {}
private:
	GameScene* m_pScene{};

	std::vector<GameObject*> m_pGameObjects{};

};
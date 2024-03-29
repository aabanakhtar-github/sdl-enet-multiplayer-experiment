#ifndef ECS_H
#define ECS_H

#include <cstdint> 
#include <string> 
#include <type_traits> 
#include <bitset> 
#include <vector> 
#include <cassert>
#include <queue> 
#include <iostream> 
#include <unordered_map>
#include <memory>

namespace ECS
{
	using EntityID = std::uint32_t;
	using ComponentID = int;

	// end user should not need these internal constants
	namespace __Internal
	{
		extern ComponentID g_next_componentID;
		constexpr EntityID MAX_ENTITIES = 10'000;
		constexpr ComponentID MAX_COMPONENT_PER_ENTITY = 32;
		using ComponentMask = std::bitset<MAX_COMPONENT_PER_ENTITY>;

		struct EntityData
		{
			ComponentMask Components;
			EntityID id;
		};
	}

	struct Component
	{
		Component() = default;
	};

	template<typename T>
	inline ComponentID GetComponentID() 
	{
		static_assert(std::is_base_of_v<Component, T> == true, "Cannot create a component ID for non-components!");
		static ComponentID s_this_ID = __Internal::g_next_componentID++;
		assert(s_this_ID < __Internal::MAX_COMPONENT_PER_ENTITY && "Too many components! Consider modifying ECS::__Internal::MAX_COMPONENT_PER_ENTITY");
		return s_this_ID;
	}

	// handly little class to help with managing all pools
	class IComponentPool
	{
	};

	template<typename T>
	class ComponentPool : public IComponentPool
	{
	public:
		explicit ComponentPool()
		{
			static_assert(std::is_base_of_v<Component, T>, "Cannot create pool of non component!");
		}

		T& AddComponent(EntityID ID)
		{
			m_ComponentPool.emplace_back();
			m_EntityToComponentMap[ID] = m_ComponentPool.size() - 1;
			m_ComponentToEntityMap[m_ComponentPool.size() - 1] = ID;
			return *(m_ComponentPool.end() - 1);
		}

		T& GetComponent(EntityID ID) 
		{
			assert(m_EntityToComponentMap.contains(ID) && "Cannot get non existent component!");
			return m_ComponentPool[m_EntityToComponentMap[ID]];
		}

		void RemoveComponent(EntityID ID)
		{
			assert(m_EntityToComponentMap.contains(ID) && "Cannot erase a component on non existent entity!");
			std::size_t last = m_ComponentPool.size() - 1;
			std::size_t this_deletion_index = m_EntityToComponentMap[ID];

			// move the last element into the spot of the deleted if it isn't the last
			// theoretically this if isn't needed but to save a little time :D
			if (this_deletion_index != last)
			{
				std::swap(m_ComponentPool[this_deletion_index], m_ComponentPool[last]);
				EntityID moved_entity = m_ComponentToEntityMap[last];
				m_EntityToComponentMap[moved_entity] = this_deletion_index;
				m_ComponentToEntityMap[this_deletion_index] = moved_entity;
			}

			m_ComponentPool.erase(m_ComponentPool.begin() + last);
			m_ComponentToEntityMap.erase(last);
			m_EntityToComponentMap.erase(ID);
		}

	private:
		ComponentPool(const ComponentPool&) = delete;
		ComponentPool& operator = (const ComponentPool&) = delete;

		std::unordered_map<EntityID, std::size_t> m_EntityToComponentMap;
		std::unordered_map<std::size_t, EntityID> m_ComponentToEntityMap;
		std::vector<T> m_ComponentPool;
	};

	class ComponentManager
	{
	public:
		explicit ComponentManager() = default;

		template<typename T>
		T& AddComponent(EntityID ID)
		{
			assert(m_ComponentPools.contains(GetComponentID<T>()) && "Unregistered component being added!");
			return	GetComponentPoolOfType<T>()->AddComponent(ID);
		}

		template<typename T>
		void RegisterComponent()
		{
			static_assert(std::is_base_of_v<Component, T>, "Cannot register non component!");
			assert(!m_ComponentPools.contains(GetComponentID<T>()) && "Cannot make pool that exists!");
			// Use std::make_shared for better performance and safety
			std::shared_ptr<IComponentPool> pool = std::make_shared<ComponentPool<T>>();
			m_ComponentPools[GetComponentID<T>()] = pool;
		}


		template<typename T>
		T& GetComponent(EntityID ID) 
		{
			auto selected_pool = GetComponentPoolOfType<T>();
			return selected_pool->GetComponent(ID);
		}

		template<typename T>
		void RemoveComponent(EntityID ID)
		{
			assert(m_ComponentPools.contains(GetComponentID<T>()) && "Cannot remove a component type that doesn't exist!");
			GetComponentPoolOfType<T>()->RemoveComponent(ID);
		}

	private:
		std::unordered_map<ComponentID, std::shared_ptr<IComponentPool>> m_ComponentPools;

		template<typename T>
		std::shared_ptr<ComponentPool<T>> GetComponentPoolOfType()
		{
			ComponentID type_ID = GetComponentID<T>();
			assert(m_ComponentPools.contains(type_ID) && "Component Pool does not exist!");
			return std::static_pointer_cast<ComponentPool<T>, IComponentPool>(m_ComponentPools[type_ID]);
		}
	};

	class Scene
	{
	public:
		explicit Scene() :
			m_FreeEntityIDs(__Internal::MAX_ENTITIES)
		{
			for (EntityID i = 0; i < __Internal::MAX_ENTITIES; ++i)
			{
				m_FreeEntityIDs[i] = i;
			}
		}

		[[nodiscard]] EntityID CreateEntity()
		{
			EntityID e = m_FreeEntityIDs.front();
			assert(e < __Internal::MAX_ENTITIES && "Cannot have more than 10000 entites");
			m_ActiveEntities.insert(std::make_pair(e, __Internal::ComponentMask()));
			m_FreeEntityIDs.pop_front();
			return e;
		}

		bool EntityExists(EntityID ID)
		{
			return m_ActiveEntities.contains(ID);
		}


		void DestroyEntity(EntityID ID)
		{
			assert(m_ActiveEntities.contains(ID) && "Cannot destroy non-existent entity!");
			m_ActiveEntities[ID].reset();
			m_FreeEntityIDs.push_front(ID);
			m_ActiveEntities.erase(ID);
		}

		template<typename T>
		void RegisterComponent()
		{
			GetComponentID<T>();
			return m_ComponentManager.RegisterComponent<T>();
		}

		template<typename T>
		T& AddComponent(EntityID ID)
		{
			assert(m_ActiveEntities[ID].test(GetComponentID<T>()) != true && "Cannot add already existing component!");
			m_ActiveEntities[ID].set(GetComponentID<T>());
			return	m_ComponentManager.AddComponent<T>(ID);
		}

		template<typename T>
		T& GetComponent(EntityID ID)  
		{
			assert(m_ActiveEntities.contains(ID) && "Cannot get non-existent component!");
			return m_ComponentManager.GetComponent<T>(ID);
		}

		template<typename T>
		void RemoveComponent(EntityID ID)
		{
			assert(m_ActiveEntities.contains(ID) && m_ActiveEntities[ID].test(GetComponentID<T>()) && "Cannot get non-existent component!");
			m_ActiveEntities[ID].set(GetComponentID<T>(), false);
			m_ComponentManager.RemoveComponent<T>(ID);
		}


	private:
		std::deque<EntityID> m_FreeEntityIDs;
		std::unordered_map<EntityID, __Internal::ComponentMask> m_ActiveEntities;
		ComponentManager m_ComponentManager;

		template<typename... T>
		friend class SceneView;

		Scene(const Scene&) = delete;
		Scene& operator = (const Scene&) = delete;
	};

	template<typename... T>
	class SceneView
	{
	public:
		explicit SceneView(Scene& scene)
		{
			static_assert((std::is_base_of_v<Component, T> && ...), "SceneView only accepts components deriving from Component!");

			if constexpr (sizeof...(T) > 0)
			{
				std::vector<ComponentID> IDs = { (GetComponentID<T>(), ...) };

				for (ComponentID ID : IDs)
				{
					m_Mask.set(ID);
				}

				for (auto [ID, component_mask] : scene.m_ActiveEntities)
				{
					if ((component_mask & m_Mask) == m_Mask)
					{
						m_ValidEntities.push_back(ID);
					}
				}
			}

			else
			{
				for (auto [ID, _] : scene.m_ActiveEntities)
				{
					m_ValidEntities.push_back(ID);
				}
			}
		}

		const std::vector<EntityID>& GetEntities() const
		{
			return m_ValidEntities;
		}

	private:
		__Internal::ComponentMask m_Mask;
		std::vector<EntityID> m_ValidEntities;

		SceneView(const SceneView&) = delete;
		SceneView& operator = (const SceneView& other) = delete;
	};

	class ISystem 
	{
	public:
		virtual void Update(Scene& scene, float delta) = 0;
		virtual void Init(Scene& scene) = 0;
	};

	class SystemManager 
	{
		SystemManager(const SystemManager&) = delete; 
		SystemManager& operator = (const SystemManager&) = delete; 
		SystemManager() = default; 

	public:
		// cross engine compat
		static SystemManager& Get() 
		{
			static SystemManager manager; 
			return manager;
		}

		void InitAllSystems(ECS::Scene& scene)
		{
			for (auto& system : m_Systems)
			{
				system->Init(scene);
			}
		}

		void RegisterSystem(ISystem* system)
		{
			m_Systems.push_back(std::unique_ptr<ISystem>(system));
		}

		void RegisterSystems(const std::vector<ISystem*>& systems)
		{
			for (auto& system : systems) 
			{
				m_Systems.push_back(std::unique_ptr<ISystem>(system));
			}	
		}

		void UpdateSystems(float delta, ECS::Scene& scene) 
		{
			for (auto& system : m_Systems)
			{
				system->Update(scene, delta);		
			}
		}

	private:
		std::vector<std::unique_ptr<ISystem>> m_Systems;
	};
}

#endif // ECS_H
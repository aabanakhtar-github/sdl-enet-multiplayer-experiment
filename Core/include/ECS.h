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

	// end user should not need these	
	namespace __Internal
	{
		extern ComponentID g_next_componentID;
		constexpr EntityID k_max_entities = 10'000;
		constexpr ComponentID k_max_components_per_entity = 32;
		using ComponentMask = std::bitset<k_max_components_per_entity>;

		struct EntityData
		{
			ComponentMask Components;
			EntityID id;
		};
	}

	class Component {};

	template<typename T>
	inline ComponentID GetComponentID()
	{
		static_assert(std::is_base_of_v<Component, T> == true, "Cannot create a component ID for non-components!");
		static ComponentID s_this_ID = __Internal::g_next_componentID++;
		return s_this_ID;
	}


	class IComponentPool
	{
	};

	template<typename T>
	class ComponentPool : public IComponentPool
	{
	public:
		explicit ComponentPool() : m_ComponentPool(__Internal::k_max_entities)
		{
			static_assert(std::is_base_of_v<Component, T>, "Cannot create pool of non component!");	
		}

		T& GetComponent(EntityID ID)
		{
			return m_ComponentPool[ID];
		}

	private:
		ComponentPool(const ComponentPool&) = delete;
		ComponentPool& operator = (const ComponentPool&) = delete;

		std::vector<T> m_ComponentPool;
	};

	class ComponentManager
	{
	public:
		explicit ComponentManager()
		{

		}
		
		template<typename T>
		void RegisterComponent()
		{
			static_assert(std::is_base_of_v<Component, T>, "Cannot register non-components!");
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
			m_FreeEntityIDs(__Internal::k_max_entities)
		{
			for (EntityID i = 0; i < __Internal::k_max_entities; ++i)
			{
				m_FreeEntityIDs[i] = i;
			}
		}

		[[nodiscard]] EntityID CreateEntity()
		{
			EntityID e = m_FreeEntityIDs.front();
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
			return m_ComponentManager.RegisterComponent<T>();
		}

		template<typename T>
		void AddComponent(EntityID ID)
		{
			assert(m_ActiveEntities[ID].test(GetComponentID<T>()) != true && "Cannot add already existing component!");
			m_ActiveEntities[ID].set(GetComponentID<T>());
			m_ComponentManager.GetComponent<T>(ID) = T();
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
		SceneView(Scene& scene)
		{
			if constexpr (sizeof...(T) > 0)
			{
				std::vector<ComponentID> IDs = { (GetComponentID<T>(), ...) };

				for (ComponentID ID : IDs)
				{
					m_Mask.set(ID);
				}

				for (auto [ID, ComponentMask] : scene.m_ActiveEntities)
				{
					if ((ComponentMask & m_Mask) == true)
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
	
		const std::vector<EntityID>& Get()
		{
			return m_ValidEntities;
		}
		
	private:
		__Internal::ComponentMask m_Mask;
		std::vector<EntityID> m_ValidEntities;
	};
	
}

#endif
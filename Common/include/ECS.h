#ifndef ECS_H
#define ECS_H

#include <deque>
#include <bitset>
#include <cassert>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace ECS {

using EntityID = std::size_t;

// end user should not need these internal constants
namespace Internal {
extern std::size_t next_component_ID;
extern std::size_t next_system_ID;
constexpr EntityID max_entities = 10'000;
constexpr std::size_t max_component_per_entity = 32;
using ComponentMask = std::bitset<max_component_per_entity + 1>;
} // namespace Internal

struct ComponentBase {
  ComponentBase() = default;
};

template <typename T> inline std::size_t getComponentID() {
  static_assert(std::is_base_of_v<ComponentBase, T> == true,
                "Cannot create a component ID for non-components!");
  static std::size_t s_this_ID = Internal::next_component_ID++;
  assert(s_this_ID < Internal::max_component_per_entity &&
         "Too many components! Consider modifying "
         "ECS::_Internal::MAX_COMPONENT_PER_ENTITY");
  return s_this_ID;
}

// handly little class to help with managing all pools
class ComponentPoolBase {};

template <typename T> class ComponentPool : public ComponentPoolBase {
public:
  explicit ComponentPool() {
    static_assert(std::is_base_of_v<ComponentBase, T>,
                  "Cannot create pool of non component!");
  }

  T &addComponent(EntityID ID) {
    component_pool_.emplace_back();
    entity_to_component_map_[ID] = component_pool_.size() - 1;
    component_to_entity_map_[component_pool_.size() - 1] = ID;
    return *(component_pool_.end() - 1);
  }

  [[nodiscard]] T &getComponent(EntityID ID) {
    assert(entity_to_component_map_.contains(ID) &&
           "Cannot get non existent component!");
    return component_pool_[entity_to_component_map_[ID]];
  }

  void removeComponent(EntityID ID) {
    assert(entity_to_component_map_.contains(ID) &&
           "Cannot erase a component on non existent entity!");
    std::size_t last = component_pool_.size() - 1;
    std::size_t this_deletion_index = entity_to_component_map_[ID];

    // swap components
    if (this_deletion_index != last) {
      std::swap(component_pool_[this_deletion_index], component_pool_[last]);
      // the id of the entity that just got swapped
      EntityID moved_entity = component_to_entity_map_[last];
      // point the swapped valid entity to it's component that's in the new
      // position (where the deleted component *WAS*)
      entity_to_component_map_[moved_entity] = this_deletion_index;
      // same thing but point the moved component to the entity
      component_to_entity_map_[this_deletion_index] = moved_entity;
    }

    component_pool_.erase(component_pool_.end());
    component_to_entity_map_.erase(last);
    entity_to_component_map_.erase(ID);
  }

  ComponentPool(const ComponentPool &) = delete;
  ComponentPool &operator=(const ComponentPool &) = delete;

private:
  std::unordered_map<EntityID, std::size_t> entity_to_component_map_;
  std::unordered_map<std::size_t, EntityID> component_to_entity_map_;
  std::vector<T> component_pool_;
};

class ComponentManager {
public:
  explicit ComponentManager() = default;

  template <typename T> T &addComponent(EntityID ID) {
    assert(m_ComponentPools.contains(getComponentID<T>()) &&
           "Unregistered component being added!");
    return getComponentPoolOfType<T>()->addComponent(ID);
  }

  template <typename T> void registerComponent() {
    static_assert(std::is_base_of_v<ComponentBase, T>,
                  "Cannot register non component!");
    assert(!m_ComponentPools.contains(getComponentID<T>()) &&
           "Cannot make pool that exists!");
    // Use std::make_shared for better performance and safety
    std::shared_ptr<ComponentPoolBase> pool =
        std::make_shared<ComponentPool<T>>();
    m_ComponentPools[getComponentID<T>()] = pool;
  }

  template <typename T> [[nodiscard]] T &getComponent(EntityID ID) {
    auto selected_pool = getComponentPoolOfType<T>();
    return selected_pool->getComponent(ID);
  }

  template <typename T> void removeComponent(EntityID ID) {
    assert(m_ComponentPools.contains(getComponentID<T>()) &&
           "Cannot remove a component type that doesn't exist!");
    getComponentPoolOfType<T>()->removeComponent(ID);
  }

private:
  std::unordered_map<std::size_t, std::shared_ptr<ComponentPoolBase>>
      m_ComponentPools;

  template <typename T>
  [[nodiscard]] std::shared_ptr<ComponentPool<T>> getComponentPoolOfType() {
    std::size_t type_ID = getComponentID<T>();
    assert(m_ComponentPools.contains(type_ID) &&
           "Component Pool does not exist!");
    return std::static_pointer_cast<ComponentPool<T>, ComponentPoolBase>(
        m_ComponentPools[type_ID]);
  }
};

class Scene {
public:
  explicit Scene() : free_entity_IDs_(Internal::max_entities) {
    for (EntityID i = 0; i < Internal::max_entities; ++i) {
      free_entity_IDs_[i] = i;
    }
  }

  [[nodiscard]] EntityID createEntity() {
    EntityID e = free_entity_IDs_.front();
    assert(e < Internal::max_entities &&
           "Cannot have more than 10000 entities");

    auto default_mask = Internal::ComponentMask();
    default_mask[0] = true;
    active_entities_.emplace(e, default_mask);
    free_entity_IDs_.pop_front();
    return e;
  }

  void setEntityActive(ECS::EntityID ID, bool active = true) {
    assert(active_entities_.contains(ID) &&
           "Cannot edit activity of a non-existent entity!");
    active_entities_[ID][0] = active;
  }

  bool entityActive(ECS::EntityID ID) {
    return active_entities_.contains(ID) && active_entities_[ID][0];
  }

  // TODO: refactor active_entities; should be "Created_entities" as these
  // entities can potentially be inactive the array holds the entities that have
  // been created but still can be disabled
  bool entityExists(EntityID ID) { return active_entities_.contains(ID); }

  void destroyEntity(EntityID ID) {
    assert(active_entities_.contains(ID) &&
           "Cannot destroy non-existent entity!");
    active_entities_[ID].reset();
    free_entity_IDs_.push_front(ID);
    active_entities_.erase(ID);
  }

  template <typename T> void registerComponent() {
    getComponentID<T>();
    return component_manager_.registerComponent<T>();
  }

  template <typename T> T &addComponent(EntityID ID) {
    assert(active_entities_[ID].test(getComponentID<T>()) != true &&
           "Cannot add already existing component!");
    active_entities_[ID].set(getComponentID<T>());
    return component_manager_.addComponent<T>(ID);
  }

  template <typename T> [[nodiscard]] T &getComponent(EntityID ID) {
    assert(active_entities_.contains(ID) &&
           "Cannot get non-existent component!");
    return component_manager_.getComponent<T>(ID);
  }

  template <typename T> void removeComponent(EntityID ID) {
    assert(active_entities_.contains(ID) &&
           active_entities_[ID].test(getComponentID<T>()) &&
           "Cannot get non-existent component!");
    active_entities_[ID].set(getComponentID<T>(), false);
    component_manager_.removeComponent<T>(ID);
  }

  template <typename T> [[nodiscard]] bool hasComponent(EntityID ID) {
    return active_entities_[ID][getComponentID<T>()];
  }

  Scene(const Scene &) = delete;
  Scene &operator=(const Scene &) = delete;

private:
  std::deque<EntityID> free_entity_IDs_;
  std::unordered_map<EntityID, Internal::ComponentMask> active_entities_;
  ComponentManager component_manager_;

  template <typename... T> friend class SceneView;
};

template <typename... T> class SceneView {
public:
  explicit SceneView(Scene &scene) {
    static_assert((std::is_base_of_v<ComponentBase, T> && ...),
                  "SceneView only accepts components deriving from Component!");
    mask_[0] = true; // it's already checking for active entities

    if constexpr (sizeof...(T) > 0) {
      std::vector<std::size_t> IDs = {(getComponentID<T>(), ...)};

      for (std::size_t ID : IDs) {
        mask_.set(ID);
      }

      for (auto [ID, component_mask] : scene.active_entities_) {
        if ((component_mask & mask_) == mask_) {
          valid_entities_.push_back(ID);
        }
      }
    } else {
      for (auto [ID, _] : scene.active_entities_) {
        valid_entities_.push_back(ID);
      }
    }
  }

  [[nodiscard]] const std::vector<EntityID> &getEntities() const {
    return valid_entities_;
  }

  SceneView(const SceneView &) = delete;
  SceneView &operator=(const SceneView &other) = delete;

private:
  Internal::ComponentMask mask_;
  std::vector<EntityID> valid_entities_;
};

class ISystem {
public:
  virtual void update(Scene &scene, float delta) = 0;
  virtual void init(Scene &scene) = 0;
};

template <typename T> std::size_t getSystemID() {
  static std::size_t ID = Internal::next_system_ID++;
  return ID;
}

class SystemManager {
  SystemManager() = default;

public:
  SystemManager(const SystemManager &) = delete;
  SystemManager &operator=(const SystemManager &) = delete;

  // cross engine compat
  [[nodiscard]] static SystemManager &get() {
    static SystemManager manager;
    return manager;
  }

  void initAllSystems(ECS::Scene &scene) {
    for (auto &system : systems_) {
      system->init(scene);
    }
  }

  template <typename T> void registerSystem(T *sys) {
    static_assert(
        std::is_base_of_v<ECS::ISystem, T>,
        "Registering a system that isn't inherited from ECS::ISystem");
    assert(!system_index_.contains(getSystemID<T>()) &&
           "Cannot add duplicate systems!");

    systems_.emplace_back(static_cast<ISystem *>(sys));
    system_index_[getSystemID<T>()] = *(systems_.end() - 1);
  }

  void updateSystems(float delta, ECS::Scene &scene) {
    for (auto &system : systems_) {
      system->update(scene, delta);
    }
  }

  template <typename T>
  // TODO: weak_ptr?
  [[nodiscard]] std::shared_ptr<T> getSystem() {
    return std::static_pointer_cast<T>(system_index_[getSystemID<T>()]);
  }

private:
  std::vector<std::shared_ptr<ISystem>> systems_;
  std::unordered_map<std::size_t, std::shared_ptr<ISystem>> system_index_;
};
} // namespace ECS

#endif // ECS_H

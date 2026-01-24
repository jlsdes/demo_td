#ifndef DEMO_TD_COMPONENT_MANAGER_HPP
#define DEMO_TD_COMPONENT_MANAGER_HPP

// Based on https://austinmorlan.com/posts/entity_component_system/

#include "component.hpp"
#include "entity/entity.hpp"
#include "utils/log.hpp"

#include <array>
#include <cassert>
#include <memory>
#include <typeindex>
#include <unordered_map>


/** Basic interface of all ComponentStore instances. */
class ComponentStore {
public:
    virtual ~ComponentStore();

    /** Creates a new component using its default (empty) constructor. */
    virtual void insert( Entity entity ) = 0;
    /** Removes the component associated with the given entity. */
    virtual void remove( Entity entity ) = 0;

    virtual Component & get( Entity entity ) = 0 ;

    virtual Component * begin();
    virtual Component * end();

    [[nodiscard]] virtual unsigned int size() const = 0;
    [[nodiscard]] virtual bool empty() const = 0;
    [[nodiscard]] virtual bool contains( Entity entity ) const = 0;
};


/** A collection of components of a specific type. */
template <SubComponent ComponentType>
class ComponentArray : public ComponentStore {
public:
    ComponentArray();
    ~ComponentArray() override = default;

    ComponentArray( ComponentArray const & store ) = delete;
    ComponentArray & operator=( ComponentArray const & store ) = delete;

    ComponentArray( ComponentArray && store ) = default;
    ComponentArray & operator=( ComponentArray && store ) = default;

    void insert( Entity entity ) override;
    void insert( Entity entity, ComponentType const & component );
    void remove( Entity entity ) override;

    ComponentType & get( Entity entity ) override;

    ComponentType * begin() override;
    ComponentType * end() override;

    [[nodiscard]] unsigned int size() const override;
    [[nodiscard]] bool empty() const override;
    [[nodiscard]] bool contains( Entity entity ) const override;

private:
    /// All components of this specific type. There exists at most one per entity, so this can't exceed that.
    std::array<ComponentType, g_max_entities> m_components;
    /// A mapping of component indices to their respective entities.
    std::array<Entity, g_max_entities> m_entities;
    /// A mapping of entities to their respective component indices.
    std::unordered_map<Entity, unsigned int> m_entity_to_component;

    unsigned int m_nr_components { 0 };
};


/** Manages ComponentStore objects for all Component subtypes. */
class ComponentManager {
public:
    ComponentManager();
    ~ComponentManager() = default;

    ComponentManager( ComponentManager const & ) = delete;
    ComponentManager & operator=( ComponentManager const & ) = delete;

    ComponentManager( ComponentManager && ) = default;
    ComponentManager & operator=( ComponentManager && ) = default;

private:
    struct StoredStore {
        std::unique_ptr<ComponentStore> store { nullptr };
        std::unique_ptr<std::type_index> type_id { nullptr };
    };

    /// All registered component stores and their respective type identifiers.
    std::array<StoredStore, g_max_component_types> m_stores;

    /// A bitfield indicating which flags are currently in use (i.e. 1 = used, 0 = unused).
    ComponentFlags m_used_flags;
};


// Template definitions
#include "component_manager.ipp"


#endif //DEMO_TD_COMPONENT_MANAGER_HPP

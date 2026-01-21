#ifndef DEMO_TD_COMPONENT_MANAGER_HPP
#define DEMO_TD_COMPONENT_MANAGER_HPP

// Based on https://austinmorlan.com/posts/entity_component_system/

#include "component.hpp"
#include "entity.hpp"
#include "utils/log.hpp"

#include <array>
#include <cassert>
#include <memory>
#include <typeindex>
#include <unordered_map>


/** Basic interface of all ComponentStore instances. */
class ComponentStore {
public:
    virtual ~ComponentStore() = default;

    /** Creates a new component using its default (empty) constructor. */
    virtual void insert( Entity entity ) = 0;
    /** Removes the component associated with the given entity. */
    virtual void remove( Entity entity ) = 0;
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

    ComponentType & get( Entity entity );

    ComponentType * begin();
    ComponentType * end();

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
    ComponentManager() = default;
    ~ComponentManager() = default;

    ComponentManager( ComponentManager const & ) = delete;
    ComponentManager & operator=( ComponentManager const & ) = delete;

    ComponentManager( ComponentManager && ) = default;
    ComponentManager & operator=( ComponentManager && ) = default;

    /** Adds a component store holding the given component type, unless one is already registered. */
    template <SubComponent ComponentType>
    void create_store();
    /** Removes the component store associated with the given component type, if possible. */
    template <SubComponent ComponentType>
    void remove_store();

    /** Returns all components of the specified type, if possible. */
    template <SubComponent ComponentType>
    ComponentArray<ComponentType> & get_component_array() const;
    /** Returns the component's type flag, if possible. If not, an error is thrown. */
    template <SubComponent ComponentType>
    ComponentFlag get_component_flag() const;

    /** Returns the entity's component of the specified type, if possible. */
    template <SubComponent ComponentType>
    ComponentType & get_component( Entity entity ) const;
    /** Inserts a new component into the appropriate component store. */
    template <SubComponent ComponentType>
    void insert_component( Entity entity );
    template <SubComponent ComponentType>
    void insert_component( Entity entity, ComponentType const & component );
    /** Removes an entity's component from the appropriate component store. */
    template <SubComponent ComponentType>
    void remove_component( Entity entity );

private:
    /** Returns a type ID that is unique per component type. */
    template <SubComponent ComponentType>
    static constexpr std::type_index type_id();

    /// All registered component stores.
    std::unordered_map<std::type_index, std::unique_ptr<ComponentStore>> m_stores;
    /// All registered component types and their respective flags.
    std::unordered_map<std::type_index, ComponentFlag> m_component_types;

    ComponentFlag m_used_flags { 0 };
};


// Template definitions
#include "component_manager.ipp"


#endif //DEMO_TD_COMPONENT_MANAGER_HPP

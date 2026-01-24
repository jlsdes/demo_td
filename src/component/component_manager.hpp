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
#include <map>


class EntityManager;
class SystemManager;


/** Basic interface of all ComponentStore instances. */
class ComponentStore {
public:
    virtual ~ComponentStore();

    /** Creates a new component using its default (empty) constructor. */
    virtual void insert( Entity entity ) = 0;
    /** Removes the component associated with the given entity. */
    virtual void remove( Entity entity ) = 0;

    virtual Component & get( Entity entity ) = 0;

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
    /// All components of this specific type. Every entity can have at most one of a specific type of component.
    std::array<ComponentType, g_max_entities> m_components;
    /// A mapping of component indices to their respective entities.
    std::array<Entity, g_max_entities> m_component_to_entity;
    /// A mapping of entities to their respective component indices.
    std::map<Entity, unsigned int> m_entity_to_component;

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

    template <SubComponent ComponentType>
    [[nodiscard]] ComponentTypeID create_store();
    void remove_store( ComponentTypeID type_id );

    [[nodiscard]] bool has_store( ComponentTypeID type_id ) const;
    [[nodiscard]] ComponentStore const & get_store( ComponentTypeID type_id ) const;

    template <SubComponent ComponentType>
    void insert_component( Entity entity, ComponentType && component );
    void remove_component( Entity entity, ComponentTypeID type_id );

    [[nodiscard]] bool entity_has_component( Entity entity, ComponentTypeID type_id ) const;
    [[nodiscard]] Component & get_component( Entity entity, ComponentTypeID type_id ) const;

private:
    EntityManager * m_entities { nullptr };
    SystemManager * m_systems { nullptr };

    /// All registered component stores and their respective type identifiers.
    std::array<std::unique_ptr<ComponentStore>, g_max_component_types> m_stores;

    std::map<std::type_index, ComponentTypeID> m_types;

    /// A bitfield indicating which flags are currently in use (i.e. 1 = used, 0 = unused).
    ComponentFlags m_used_flags;
};


// Template definitions
#include "component_manager.ipp"


#endif //DEMO_TD_COMPONENT_MANAGER_HPP

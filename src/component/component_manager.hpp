#ifndef DEMO_TD_COMPONENT_MANAGER_HPP
#define DEMO_TD_COMPONENT_MANAGER_HPP

#include "component.hpp"
#include "entity/entity.hpp"
#include "utils/log.hpp"

#include <array>
#include <bit>
#include <cassert>
#include <memory>
#include <typeindex>
#include <map>


struct ECS;
class EntityManager;
class SystemManager;


/** Basic interface of all ComponentStore instances. */
class ComponentStore {
public:
    virtual ~ComponentStore() = default;

    /** Creates a new component using its default (empty) constructor. */
    virtual void insert( EntityID entity ) = 0;
    /** Removes the component associated with the given entity. */
    virtual void remove( EntityID entity ) = 0;

    [[nodiscard]] virtual Component & get( EntityID entity ) = 0;

    // [[nodiscard]] virtual Component * begin() = 0;
    // [[nodiscard]] virtual Component * end() = 0;

    [[nodiscard]] virtual unsigned int size() const = 0;
    [[nodiscard]] virtual bool empty() const = 0;
    [[nodiscard]] virtual bool contains( EntityID entity ) const = 0;
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

    void insert( EntityID entity ) override;
    void insert( EntityID entity, ComponentType const & component );
    void remove( EntityID entity ) override;

    [[nodiscard]] ComponentType & get( EntityID entity ) override;

    [[nodiscard]] unsigned int size() const override;
    [[nodiscard]] bool empty() const override;
    [[nodiscard]] bool contains( EntityID entity ) const override;

    /** Provides iteration over the components in the array. */
    class Iterator {
    public:
        Iterator( ComponentArray & array, unsigned int start_index );

        /** Advances the iterator to the next component. */
        Iterator & operator++();
        /** Returns the component that is currently being pointed at. */
        ComponentType & operator*();
        ComponentType * operator->();
        /** Compares two iterators, which must belong to the exact same component array. */
        bool operator==( Iterator const & other ) const;

        /** Returns the entity the current component belongs to. */
        [[nodiscard]] EntityID get_entity() const;
        /** Returns the currently pointed at component. */
        [[nodiscard]] ComponentType & get_component();

    private:
        /// The array that is being iterated over.
        ComponentArray & m_array;
        /// The currently pointed at component's index.
        unsigned int m_index;
    };

    /** Returns an iterator pointing at the first component in the array. */
    [[nodiscard]] Iterator begin();
    /** Returns an iterator pointing at the first component past the array. */
    [[nodiscard]] Iterator end();

private:
    /// All components of this specific type. Every entity can have at most one of a specific type of component.
    std::array<ComponentType, g_max_entities> m_components;
    /// A mapping of component indices to their respective entities.
    std::array<EntityID, g_max_entities> m_component_to_entity;
    /// A mapping of entities to their respective component indices.
    std::map<EntityID, unsigned int> m_entity_to_component;

    unsigned int m_nr_components { 0 };
};


/** Manages ComponentStore objects for all Component subtypes. */
class ComponentManager {
public:
    explicit ComponentManager( ECS * ecs );
    ~ComponentManager() = default;

    ComponentManager( ComponentManager const & ) = delete;
    ComponentManager & operator=( ComponentManager const & ) = delete;

    ComponentManager( ComponentManager && ) = default;
    ComponentManager & operator=( ComponentManager && ) = delete;

    /** Creates a new component store for the given type of components. Only one store can exist per component type. A
     *  store of the appropriate type must be created before components of that type can be inserted. */
    template <SubComponent ComponentType>
    ComponentTypeID create_store();
    /** Removes a component store, after purging any leftover components of the related type. */
    void remove_store( ComponentTypeID type_id );

    /** Returns whether the type ID is valid. */
    [[nodiscard]] bool has_store( ComponentTypeID type_id ) const;

    /** Provides access to the underlying ComponentArray<> object, which is required for this function. */
    template <SubComponent ComponentType>
    ComponentArray<ComponentType> & get_array();

    template <SubComponent ComponentType>
    [[nodiscard]] ComponentTypeID get_type_id() const;

    template <SubComponent ComponentType>
    void insert_component( EntityID entity, ComponentType const & component );
    void remove_component( EntityID entity, ComponentTypeID type_id );

    [[nodiscard]] bool entity_has_component( EntityID entity, ComponentTypeID type_id ) const;
    [[nodiscard]] Component & get_component( EntityID entity, ComponentTypeID type_id ) const;

    template <SubComponent ComponentType>
    [[nodiscard]] ComponentType & get_component( EntityID entity );

    /** Returns an iterator for the component array containing all components of the specified type. These iterators can
     *  be used to retrieve the actual component data, as well as the associated entity ID. These specific functions
     *  require the components to be stored in a ComponentArray<>, declared above. */
    template <SubComponent ComponentType>
    [[nodiscard]] ComponentArray<ComponentType>::Iterator begin();
    template <SubComponent ComponentType>
    [[nodiscard]] ComponentArray<ComponentType>::Iterator end();

private:
    /// The ECS object contains this object, and the partnered EntityManager and SystemManager objects.
    EntityManager & m_entities;
    SystemManager & m_systems;

    /// All registered component stores and their respective type identifiers.
    std::array<std::unique_ptr<ComponentStore>, g_max_component_types> m_stores;
    /// A mapping of types to their respective type IDs.
    std::map<std::type_index, ComponentTypeID> m_types;

    /// A bitfield indicating which flags are currently in use (i.e. 1 = used, 0 = unused).
    ComponentFlags m_used_flags;
};


// Template definitions
#include "component_manager.ipp"


#endif //DEMO_TD_COMPONENT_MANAGER_HPP

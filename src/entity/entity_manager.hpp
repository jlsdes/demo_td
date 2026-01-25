#ifndef DEMO_TD_ENTITY_MANAGER_HPP
#define DEMO_TD_ENTITY_MANAGER_HPP

#include "entity.hpp"
#include "component/component.hpp"

#include <array>


struct ECS;
class ComponentManager;
class SystemManager;


/** Manages all entities; mainly handles the assignment of entity IDs. */
class EntityManager {
public:
    explicit EntityManager( ECS * ecs );
    ~EntityManager() = default;

    EntityManager( EntityManager const & other ) = delete;
    EntityManager & operator=( EntityManager const & other ) = delete;

    EntityManager( EntityManager && other ) = default;
    EntityManager & operator=( EntityManager && other ) = delete;

    /** Assigns an ID to the new entity. */
    [[nodiscard]] EntityID create();
    /** Removes the entity. This function does not check for left-over components. */
    void remove( EntityID entity );

    /** Returns whether an entity currently exists with the given ID. */
    [[nodiscard]] bool entity_exists( EntityID entity ) const;

    /** Returns which components the entity consists of. */
    [[nodiscard]] ComponentFlags get_flags( EntityID entity ) const;
    /** Returns whether the entity has (a) certain component(s). */
    [[nodiscard]] bool has_flags( EntityID entity, ComponentFlags flags ) const;

    /** Iterates over all existing entities, with an optional filter. The filter indicates which set of components are
     *  being queried. Any entity that does not have all of these components will be skipped. A filter with only 0 bits
     *  (the default parameter) doesn't skip any entity, if the entity exists of course. */
    class Iterator {
    public:
        Iterator( EntityManager const & manager, EntityID initial_entity, ComponentFlags filter = 0 );

        /** Advances the iterator to the next entity. */
        Iterator & operator++();
        /** Returns the current entity's ID. */
        EntityID operator*() const;
        /** Compares two iterators, which must share the same EntityManager. The filter attribute is ignored. */
        bool operator==( Iterator const & other ) const;

    private:
        /// The manager managing the entities that are being iterated over.
        EntityManager const & m_manager;
        /// The entity that is currently being pointed at.
        EntityID m_current;
        /// The filter indicating which component flags must be set (i.e. be equal to 1) for the entities being iterated
        /// over.
        ComponentFlags m_filter;
    };

    [[nodiscard]] Iterator begin( ComponentFlags filter = 0 ) const;
    [[nodiscard]] Iterator end() const;

    /** Called by the ComponentManager when a new component is created. */
    void set_flag( EntityID entity, ComponentTypeID component_type );
    /** Called by the ComponentManager when a component is removed. */
    void unset_flag( EntityID entity, ComponentTypeID component_type );
    /** Called by the ComponentManager when a component type is removed entirely. */
    void unset_all( ComponentTypeID component_type );

private:
    /// The ECS object contains this object, and the partnered ComponentManager and SystemManager objects.
    ComponentManager & m_components;
    SystemManager & m_systems;

    /// The components that each entity consists of.
    std::array<ComponentFlags, g_max_entities> m_component_flags;

    using SegmentType = unsigned long long;
    static unsigned int constexpr s_segment_size { sizeof( SegmentType ) * 8 }; // Size in 64 bit words
    static unsigned int constexpr s_array_size { g_max_entities / s_segment_size }; // Size in number of segments

    /// Existence indicators, where each bit shows whether the associated ID is currently in use.
    std::array<SegmentType, s_array_size> m_existing;
    /// The number of entities that currently exist.
    unsigned int m_nr_entities;
    /// The next entity ID that will be generated, if possible.
    EntityID m_next_entity;
};


#endif //DEMO_TD_ENTITY_MANAGER_HPP

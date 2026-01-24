#ifndef DEMO_TD_ENTITY_MANAGER_HPP
#define DEMO_TD_ENTITY_MANAGER_HPP

#include "entity.hpp"
#include "component/component.hpp"

#include <array>


class ComponentManager;
class SystemManager;


/** Manages all entities; mainly handles the assignment of entity IDs. */
class EntityManager {
public:
    EntityManager();
    ~EntityManager() = default;

    EntityManager( EntityManager const & other ) = delete;
    EntityManager & operator=( EntityManager const & other ) = delete;

    EntityManager( EntityManager && other ) = default;
    EntityManager & operator=( EntityManager && other ) = default;

    /** Assigns an ID to the new entity. */
    Entity create();
    /** Removes the entity. This function does not check for left-over components. */
    void remove( Entity entity );

    /** Returns whether an entity currently exists with the given ID. */
    [[nodiscard]] bool entity_exists( Entity entity ) const;

    /** Returns which components the entity consists of. */
    [[nodiscard]] ComponentFlags get_flags( Entity entity ) const;
    /** Returns whether the entity has (a) certain component(s). */
    [[nodiscard]] bool has_flags( Entity entity, ComponentFlags flags ) const;

    /** Iterates over all existing entities, with an optional filter. The filter indicates which set of components are
     *  being queried. Any entity that does not have all of these components will be skipped. A filter with only 0 bits
     *  (the default parameter) doesn't skip any entity, if the entity exists of course. */
    class Iterator {
    public:
        Iterator( EntityManager & manager, Entity initial_entity, ComponentFlags filter = 0 );

        /** Advances the iterator to the next entity. */
        Iterator & operator++();
        /** Returns the current entity's ID. */
        Entity operator*() const;
        /** Compares two iterators, which must share the same EntityManager. The filter attribute is ignored. */
        bool operator==( Iterator const & other ) const;

    private:
        /// The manager managing the entities that are being iterated over.
        EntityManager & m_manager;
        /// The entity that is currently being pointed at.
        Entity m_current;
        /// The filter indicating which component flags must be set (i.e. be equal to 1) for the entities being iterated
        /// over.
        ComponentFlags m_filter;
    };

    Iterator begin( ComponentFlags filter = 0 );
    Iterator end();

private:
    /** Called by the ComponentManager when a new component is created. */
    void set_flag( Entity entity, ComponentTypeID component_type );
    /** Called by the ComponentManager when a component is removed. */
    void unset_flag( Entity entity, ComponentTypeID component_type );
    /** Called by the ComponentManager when a component type is removed entirely. */
    void unset_all( ComponentTypeID component_type );


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
    Entity m_next_entity;

    // Allow these to call the (un)set_... functions
    friend class ComponentManager;
    // friend class SystemManager;

    // TODO link these classes together somehow
};


#endif //DEMO_TD_ENTITY_MANAGER_HPP

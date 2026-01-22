#ifndef DEMO_TD_ENTITY_MANAGER_HPP
#define DEMO_TD_ENTITY_MANAGER_HPP

#include "component.hpp"
#include "entity.hpp"

#include <array>


/** Manages all entities; mainly handles the assignment of entity IDs. */
class EntityManager {
public:
    EntityManager();
    ~EntityManager() = default;

    EntityManager( EntityManager const & other ) = delete;
    EntityManager & operator=( EntityManager const & other ) = delete;

    EntityManager( EntityManager && other ) = default;
    EntityManager & operator=( EntityManager && other ) = default;

    Entity create( ComponentFlag flags );
    void remove( Entity entity );

    [[nodiscard]] bool entity_exists( Entity entity ) const;

    [[nodiscard]] ComponentFlag get_flags( Entity entity ) const;
    [[nodiscard]] bool entity_has_components( Entity entity, ComponentFlag flags ) const;

    /** Iterates over all entities, filtering them on their component flags if requested. */
    class Iterator {
    public:
        Iterator( EntityManager & manager, Entity initial_entity, ComponentFlag filter = 0 );

        /** Advances the iterator to the next entity matching the filter. */
        Iterator & operator++();
        /** Returns the current entity's ID. */
        Entity operator*() const;
        /** Compares two iterators, which must share the same EntityManager. */
        bool operator==( Iterator const & other ) const;

    private:
        /// The manager managing the entities that are being iterated over.
        EntityManager & m_manager;
        /// The entity that is currently being pointed at.
        Entity m_current;
        /// The filter determines which entities to skip. If an entity has a 0 bit where the filter is 1, then it's
        /// skipped. Bits of the filter that are 0 are ignored, and thus filter = 0 doesn't filter at all.
        ComponentFlag m_filter;
    };

    Iterator begin( ComponentFlag filter = 0 );
    Iterator end();

private:
    /// Slots for all entities, where the flags indicate which components each entity contains.
    std::array<ComponentFlag, g_max_entities> m_component_flags;

    using SegmentType = unsigned long long;
    static unsigned int constexpr s_segment_size { sizeof(SegmentType) * 8 };
    static unsigned int constexpr s_array_size { g_max_entities / s_segment_size };

    /// Existence indicators, where each bit shows whether the associated ID is currently in use.
    std::array<SegmentType, s_array_size> m_existing;

    unsigned int m_nr_entities;
    Entity m_next_entity;
};


#endif //DEMO_TD_ENTITY_MANAGER_HPP
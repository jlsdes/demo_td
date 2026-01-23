#ifndef DEMO_TD_ENTITY_MANAGER_HPP
#define DEMO_TD_ENTITY_MANAGER_HPP

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

    /** Assigns an ID to the new entity. */
    Entity create();
    /** Removes the entity. This function does not check for left-over components. */
    void remove( Entity entity );

    /** Returns whether an entity currently exists with the given ID. */
    [[nodiscard]] bool entity_exists( Entity entity ) const;

    /** Iterates over all existing entities. */
    class Iterator {
    public:
        Iterator( EntityManager & manager, Entity initial_entity );

        /** Advances the iterator to the next entity. */
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
    };

    Iterator begin();
    Iterator end();

private:
    using SegmentType = unsigned long long;
    static unsigned int constexpr s_segment_size { sizeof(SegmentType) * 8 };
    static unsigned int constexpr s_array_size { g_max_entities / s_segment_size };

    /// Existence indicators, where each bit shows whether the associated ID is currently in use.
    std::array<SegmentType, s_array_size> m_existing;

    unsigned int m_nr_entities;
    Entity m_next_entity;
};


#endif //DEMO_TD_ENTITY_MANAGER_HPP
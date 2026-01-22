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
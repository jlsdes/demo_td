#include "coordinator.hpp"


Coordinator::Coordinator() : m_components {}, m_entities {}, m_systems { &m_components } {}

void Coordinator::run_systems( unsigned int const group ) const {
    m_systems.run_group( group );
}

Entity Coordinator::insert_entity() {
    return m_entities.create();
}

void Coordinator::remove_entity( Entity const entity ) {
    // ComponentFlag flags { m_entities.get_flags( entity ) };
    // ComponentFlag shifting_flag { 1 };
    // while ( flags ) {
    //     if ( flags & shifting_flag ) {
    //         m_components.remove_component( entity, shifting_flag );
    //         flags &= ~shifting_flag;
    //     }
    //     shifting_flag <<= 1;
    // }
    // m_entities.remove( entity );
}

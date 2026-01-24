#include "coordinator.hpp"


Coordinator::Coordinator() : m_components {}, m_entities {}, m_systems { &m_components } {}

void Coordinator::run_systems( unsigned int const group ) const {
    m_systems.run_group( group );
}

Entity Coordinator::insert_entity() {
    return m_entities.create();
}

void Coordinator::remove_entity( Entity const entity ) {
    m_components.remove_entity( entity );
    m_entities.remove( entity );
}

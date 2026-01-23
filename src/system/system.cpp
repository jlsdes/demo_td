#include "system.hpp"


System::System( ComponentFlags const flags, ComponentManager * const component_manager )
    : m_required_components { flags }, m_component_manager { component_manager } {}

ComponentFlags System::get_required_components() const {
    return m_required_components;
}

ComponentStore * System::get_components( ComponentFlags const flag ) const {
    return m_component_manager->get_component_store( flag );
}

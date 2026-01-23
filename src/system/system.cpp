#include "system.hpp"


System::System( ComponentFlag const flags, ComponentManager * const component_manager )
    : m_required_components { flags }, m_component_manager { component_manager } {}

ComponentFlag System::get_required_components() const {
    return m_required_components;
}

ComponentStore * System::get_components( ComponentFlag const flag ) const {
    return m_component_manager->get_component_store( flag );
}

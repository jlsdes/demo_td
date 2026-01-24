#include "system.hpp"


System::System( ComponentFlags const flags ) : m_required_components { flags } {}

ComponentFlags System::get_required_components() const {
    return m_required_components;
}

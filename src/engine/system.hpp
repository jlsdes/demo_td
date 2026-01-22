#ifndef DEMO_TD_SYSTEM_HPP
#define DEMO_TD_SYSTEM_HPP

#include "engine/component.hpp"
#include "engine/component_manager.hpp"


/** A system operates on all components of (a) certain type(s). */
class System {
public:
    System( ComponentFlag flags, ComponentManager * component_manager );
    virtual ~System() = default;

    System( System const & system ) = delete;
    System & operator = ( System const & system ) = delete;

    System( System && system ) noexcept = default;
    System & operator= ( System && system ) noexcept = delete;

protected:
    /** Returns the component store/array associated with the component flag/type. */
    [[nodiscard]] ComponentStore * get_components( ComponentFlag flag ) const;
    template <SubComponent ComponentType>
    [[nodiscard]] ComponentArray<ComponentType> & get_components() const;

private:
    /// The component types the system needs to function.
    ComponentFlag const m_required_components;
    /// The associated ComponentManager holding the components the system needs to operate on.
    ComponentManager * m_component_manager;
};


// Template definitions

template <SubComponent ComponentType>
ComponentArray<ComponentType> & System::get_components() const {
    return m_component_manager->get_component_array<ComponentType>();
}


#endif //DEMO_TD_SYSTEM_HPP
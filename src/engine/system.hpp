#ifndef DEMO_TD_SYSTEM_HPP
#define DEMO_TD_SYSTEM_HPP

#include "engine/component.hpp"
#include "engine/component_manager.hpp"

#include <type_traits>


/** A system operates on all components of (a) certain type(s). */
class System {
public:
    System( ComponentFlag flags, ComponentManager * component_manager );
    virtual ~System() = default;

    System( System const & ) = delete;
    System & operator = ( System const & ) = delete;

    System( System && ) noexcept = default;
    System & operator= ( System && ) = delete;

    /** Abstract function where derived types should implement their functionality. */
    virtual void run() = 0;

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


/// Concept that requires a type to be derived from the System base class.
template <typename DerivedSystem>
concept SubSysten = requires( DerivedSystem system ) { std::is_base_of_v<System, DerivedSystem>; };


// Template definitions

template <SubComponent ComponentType>
ComponentArray<ComponentType> & System::get_components() const {
    return m_component_manager->get_component_array<ComponentType>();
}


#endif //DEMO_TD_SYSTEM_HPP
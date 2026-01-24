#ifndef DEMO_TD_SYSTEM_HPP
#define DEMO_TD_SYSTEM_HPP

#include "component/component.hpp"

#include <type_traits>


class EntityManager;
class ComponentManager;


/** A system operates on all components of (a) certain type(s). */
class System {
public:
    explicit System( ComponentFlags flags );
    virtual ~System() = default;

    System( System const & ) = delete;
    System & operator = ( System const & ) = delete;

    System( System && ) noexcept = default;
    System & operator= ( System && ) = delete;

    /** Returns the flags of the components the system operates on. */
    [[nodiscard]] ComponentFlags get_required_components() const;

    /** Abstract function where derived types should implement their functionality. */
    virtual void run( EntityManager const & entities, ComponentManager & components ) = 0;

private:
    /// The component types the system needs to function.
    ComponentFlags const m_required_components;
};


/// Concept that requires a type to be derived from the System base class.
template <typename DerivedSystem>
concept SubSystem = requires( DerivedSystem system ) { std::is_base_of_v<System, DerivedSystem>; };


#endif //DEMO_TD_SYSTEM_HPP
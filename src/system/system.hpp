#ifndef DEMO_TD_SYSTEM_HPP
#define DEMO_TD_SYSTEM_HPP

#include <type_traits>


class EntityManager;
class ComponentManager;


/** A system operates on all components of (a) certain type(s). */
class System {
public:
    System() = default;
    virtual ~System() = default;

    System( System const & ) = delete;
    System & operator = ( System const & ) = delete;

    System( System && ) noexcept = default;
    System & operator= ( System && ) = default;

    /** Abstract function where derived types should implement their functionality. */
    virtual void run( EntityManager const & entities, ComponentManager & components ) = 0;
};


/// Concept that requires a type to be derived from the System base class.
template <typename DerivedSystem>
concept SubSystem = requires( DerivedSystem system ) { std::is_base_of_v<System, DerivedSystem>; };


#endif //DEMO_TD_SYSTEM_HPP
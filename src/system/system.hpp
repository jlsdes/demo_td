#ifndef DEMO_TD_SYSTEM_HPP
#define DEMO_TD_SYSTEM_HPP

#include "core/context.hpp"

#include <type_traits>


/** A system operates on all components of (a) certain type(s). */
class System {
public:
    explicit System( Context const & context ) : m_context { context } {}
    virtual ~System() = default;

    System( System const & ) = delete;
    System & operator =( System const & ) = delete;

    System( System && ) noexcept = default;
    System & operator=( System && ) noexcept = delete;

    /** Abstract function where derived types should implement their functionality. */
    virtual void run() = 0;

protected:
    Context const & m_context;
};


/// Concept that requires a type to be derived from the System base class.
template <typename DerivedSystem>
concept SubSystem = requires( DerivedSystem system ) { std::is_base_of_v<System, DerivedSystem>; };


#endif //DEMO_TD_SYSTEM_HPP

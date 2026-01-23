#ifndef DEMO_TD_COMPONENT_HPP
#define DEMO_TD_COMPONENT_HPP

#include <type_traits>


/// The maximum number of component types that can be registered to a single ComponentManager. If this is exceeded, then
/// a different kind of flag is needed rather than unsigned long; although this seems unlikely.
unsigned int constexpr g_max_component_types { 64 };


/** A component holds data of an aspect of an entity, and can be batch processed with similar components by a system. */
struct Component {};


/// An ID for the components' type, and also to be used by entities to indicate which components they consist of by
/// adding the flags of their respective components together.
using ComponentFlag = unsigned long long;


/** Requires a type derived from the Component base struct. */
template <typename Type>
concept SubComponent = requires( Type component ) { std::is_base_of_v<Component, Type>; };


#endif //DEMO_TD_COMPONENT_HPP
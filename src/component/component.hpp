#ifndef DEMO_TD_COMPONENT_HPP
#define DEMO_TD_COMPONENT_HPP

#include <bit>
#include <type_traits>


/// The maximum number of component types that can be registered to a single ComponentManager. If this is exceeded, then
/// a different kind of flag is needed rather than unsigned long; although this seems unlikely.
unsigned int constexpr g_max_component_types { 64 };


/** A component holds data of an aspect of an entity, and can be batch processed with similar components by a system. */
struct Component {};


/// A unique ID per component type per component manager. The ID value also represents the indicator bit in
/// ComponentFlags for the same type.
using ComponentTypeID = unsigned char;

/// An "array" of bits indicating the relevant component types. Different component managers use different
/// representations.
using ComponentFlags = unsigned long long;


/** Returns the flag used to indicate the same type as the ID. */
inline ComponentFlags id_to_flag( ComponentTypeID const id ) {
    return 1ull << id;
}
/** Returns the ID of the type represented by the rightmost 1 bit in the flag. */
inline ComponentTypeID flag_to_id( ComponentFlags const flag ) {
    return std::countr_zero( flag );
}


/** Requires a type derived from the Component base struct. */
template <typename Type>
concept SubComponent = requires( Type component ) { std::is_base_of_v<Component, Type>; };


#endif //DEMO_TD_COMPONENT_HPP
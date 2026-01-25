#ifndef DEMO_TD_ENTITY_HPP
#define DEMO_TD_ENTITY_HPP


/** Every entity is composed of a number of components, and is represented by a unique ID. */
using EntityID = unsigned int;


constexpr unsigned int g_max_entities = 1 << 15;


#endif //DEMO_TD_ENTITY_HPP
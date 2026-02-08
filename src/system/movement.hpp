#ifndef DEMO_TD_MOVEMENT_HPP
#define DEMO_TD_MOVEMENT_HPP

#include "system.hpp"
#include "entity/entity.hpp"
#include "graphics/input_manager.hpp"

#include <set>

#include <GLFW/glfw3.h>


std::set<int> const used_keys {
    GLFW_KEY_W,
    GLFW_KEY_A,
    GLFW_KEY_S,
    GLFW_KEY_D,
    GLFW_KEY_LEFT_SHIFT,
    GLFW_KEY_RIGHT_SHIFT,
    GLFW_KEY_LEFT_CONTROL,
    GLFW_KEY_RIGHT_CONTROL,
    GLFW_KEY_SPACE
};


/** The movement system for all entities, whether it's the enemies, bullets, or the player. */
class Movement : public System {
public:
    explicit Movement( InputManager & input_manager );
    Movement( InputManager & input_manager, EntityID player_entity );
    ~Movement() override;

    void run( EntityManager const & entities, ComponentManager & components ) override;

    /** Sets which entity is being controlled by the player. Normally this should be a camera. */
    void set_player_entity( EntityID entity );
    /** Disables player input affecting any entity (through this system at least). */
    void unset_player_entity();

    /** Callback function that reacts to relevant keyboard input from the player. */
    void player_input( int key, int action );

private:
    EntityID m_player_entity;
    bool m_player_active;

    InputManager & m_input_manager;
    unsigned int m_callback_id;
};


#endif //DEMO_TD_MOVEMENT_HPP

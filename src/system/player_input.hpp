#ifndef DEMO_TD_PLAYER_INPUT_HPP
#define DEMO_TD_PLAYER_INPUT_HPP

#include "system.hpp"

#include "entity/entity.hpp"
#include "graphics/input_manager.hpp"

#include <array>
#include <map>
#include <ranges>
#include <set>

#include <GLFW/glfw3.h>


/// The keyboard-related actions that affect the camera.
enum CameraActions : unsigned char {
    Forward,
    Backward,
    Left,
    Right,
    Up,
    Down,
    Sprint,
    NumberActions // Must remain the last value in this enum; not a valid enum value
};

/// The default controls, which may or may not be configurable in-game.
std::map<int, unsigned char> const default_controls {
        { GLFW_KEY_W, Forward },
        { GLFW_KEY_UP, Forward },
        { GLFW_KEY_A, Left },
        { GLFW_KEY_LEFT, Left },
        { GLFW_KEY_S, Backward },
        { GLFW_KEY_DOWN, Backward },
        { GLFW_KEY_D, Right },
        { GLFW_KEY_RIGHT, Right },
        { GLFW_KEY_LEFT_SHIFT, Up },
        { GLFW_KEY_RIGHT_SHIFT, Up },
        { GLFW_KEY_LEFT_CONTROL, Down },
        { GLFW_KEY_RIGHT_CONTROL, Down },
        { GLFW_KEY_SPACE, Sprint },
    };

/// The keys used by the default controls. These are gathered in a set here because the InputManager expects sets.
auto constexpr controls_view_ { std::ranges::views::keys( default_controls ) };
std::set<int> const used_keys { controls_view_.cbegin(), controls_view_.cend() };


class PlayerInput : public System {
public:
    PlayerInput( ECS * ecs, InputManager & input_manager );
    ~PlayerInput() override;

    void run() override;

    /** Sets which entity is being controlled by the player. Normally this should be a camera. */
    void set_player_entity( EntityID entity );
    /** Disables player input affecting any entity (through this system at least). */
    void unset_player_entity();

    /** Callback function that reacts to relevant keyboard input from the player. */
    void player_input( int key, int action );

private:
    EntityID m_player_entity;
    bool m_player_active;

    /// A mapping of keyboard key codes to their respective flag indices. A value of 0 in this array indicates that the
    /// key is not being used. This could just be the map defined above, but arrays tend to be more efficient.
    std::array<unsigned char, GLFW_KEY_LAST + 1> m_controls;

    /// Indicators for whether certain actions are being performed. These are not just boolean values, because multiple
    /// keys can represent the same action. E.g., suppose W and UP are both being pressed and UP gets released, then the
    /// camera should keep moving forward because W is still being pressed. Setting a Forward-indicating bool on any
    /// action would not achieve this, but in-/decrementing a counter does.
    std::array<unsigned char, NumberActions> m_input_counts;

    InputManager & m_input_manager;
    unsigned int m_callback_id;
};


#endif //DEMO_TD_PLAYER_INPUT_HPP
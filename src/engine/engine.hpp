#ifndef DEMO_TD_ENGINE_HPP
#define DEMO_TD_ENGINE_HPP

#include "controller_manager.hpp"
#include "model_manager.hpp"
#include "renderer.hpp"
#include "window.hpp"

#include <memory>
#include <vector>


class Engine {
public:
    Engine();
    ~Engine() = default;

    void push_model_manager( std::unique_ptr<ModelManager> && model_manager );
    bool pop_model_manager( ModelManager const * model_manager );

    void push_view_manager( std::unique_ptr<Renderer> && view_manager );
    bool pop_view_manager( Renderer const * view_manager );

    void push_controller_manager( std::unique_ptr<ControllerManager> && controller_manager );
    bool pop_controller_manager( ControllerManager const * controller_manager );

    void game_loop();
    void render_loop();

    [[nodiscard]] Window & get_window() const; // TODO Decide whether to keep this function

private:
    std::unique_ptr<Window> m_window;

    /// MVC managers
    std::vector<std::unique_ptr<ModelManager>> m_models;
    std::vector<std::unique_ptr<Renderer>> m_views;
    std::vector<std::unique_ptr<ControllerManager>> m_controllers;
};


#endif //DEMO_TD_ENGINE_HPP
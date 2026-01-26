#ifndef DEMO_TD_RENDERER_HPP
#define DEMO_TD_RENDERER_HPP

#include "system/system.hpp"


/** Renders all drawable components. */
class Renderer : public System {
public:
    Renderer() = default;
    ~Renderer() override = default;

    void run( EntityManager const & entities, ComponentManager & components ) override;

private:
};


#endif //DEMO_TD_RENDERER_HPP

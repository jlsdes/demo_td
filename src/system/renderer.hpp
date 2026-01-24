#ifndef DEMO_TD_RENDERER_HPP
#define DEMO_TD_RENDERER_HPP

#include "component/drawable.hpp"
#include "system/system.hpp"


class Renderer : public System {
public:
    explicit Renderer( ComponentFlags flags );
    ~Renderer() override = default;

    void run(EntityManager const & entities, ComponentManager & components) override;

private:
};


#endif //DEMO_TD_RENDERER_HPP
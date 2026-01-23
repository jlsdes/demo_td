#ifndef DEMO_TD_RENDERER_HPP
#define DEMO_TD_RENDERER_HPP

#include "component/drawable.hpp"
#include "system/system.hpp"


class Coordinator;


class Renderer : public System {
public:
    explicit Renderer( Coordinator const & coordinator );
    ~Renderer() override = default;

    void run() override;

private:
};


#endif //DEMO_TD_RENDERER_HPP
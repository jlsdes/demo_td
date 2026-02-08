#ifndef DEMO_TD_MOVEMENT_HPP
#define DEMO_TD_MOVEMENT_HPP

#include "system.hpp"


/** The movement system for all entities, whether it's the enemies, bullets, or the player. */
class Movement : public System {
public:
    Movement() = default;
    ~Movement() override = default;

    void run( EntityManager const & entities, ComponentManager & components ) override;

private:
};


#endif //DEMO_TD_MOVEMENT_HPP

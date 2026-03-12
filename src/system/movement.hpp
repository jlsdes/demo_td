#ifndef DEMO_TD_MOVEMENT_HPP
#define DEMO_TD_MOVEMENT_HPP

#include "system.hpp"


/** The movement system for all entities, whether it's the enemies, bullets, or the player. */
class Movement : public System {
public:
    explicit Movement( Context const & context );
    ~Movement() override = default;

    void run() override;

private:
};


#endif //DEMO_TD_MOVEMENT_HPP

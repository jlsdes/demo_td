#ifndef DEMO_TD_CONTROLLER_OBJECT_HPP
#define DEMO_TD_CONTROLLER_OBJECT_HPP

#include "../utils/manager.hpp"


/** (Base) class for all controllers, i.e. the behaviour of the entities and player input handler. */
class ControllerObject: public ManagedObject {
public:
    // TODO Figure out how to do this, because atm this is just identical to ManagedObject
    ControllerObject() = default;
    ~ControllerObject() override = default;

    void update() override = 0;
};


#endif //DEMO_TD_CONTROLLER_OBJECT_HPP
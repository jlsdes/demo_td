#ifndef DEMO_TD_CONTROLLER_MANAGER_HPP
#define DEMO_TD_CONTROLLER_MANAGER_HPP

#include "controller_object.hpp"
#include "utils/manager.hpp"


/** Manages a group of ControllerObjects. */
class ControllerManager : public Manager<ControllerObject> {
public:
    ControllerManager() = default;

private:
};


#endif //DEMO_TD_CONTROLLER_MANAGER_HPP

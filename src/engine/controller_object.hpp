#ifndef DEMO_TD_CONTROLLER_OBJECT_HPP
#define DEMO_TD_CONTROLLER_OBJECT_HPP


/** (Base) class for all controllers, i.e. the behaviour of the entities and player input handler. */
class ControllerObject {
public:
    ControllerObject() = default;
    virtual ~ControllerObject() = default;

    virtual void update() const = 0;
};


#endif //DEMO_TD_CONTROLLER_OBJECT_HPP
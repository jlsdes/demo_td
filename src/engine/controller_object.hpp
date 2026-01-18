#ifndef DEMO_TD_CONTROLLER_OBJECT_HPP
#define DEMO_TD_CONTROLLER_OBJECT_HPP

#include "utils/manager.hpp"


class ModelObject;

/** (Base) class for all controllers, i.e. the behaviour of the entities and player input handler. */
class ControllerObject: public ManagedObject {
public:
    explicit ControllerObject( ModelObject * model );
    ~ControllerObject() override = default;

    void update() override = 0;

protected:
    /// The model being controlled.
    ModelObject * m_model;
};


#endif //DEMO_TD_CONTROLLER_OBJECT_HPP
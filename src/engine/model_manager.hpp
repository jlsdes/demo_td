#ifndef DEMO_TD_MODEL_MANAGER_HPP
#define DEMO_TD_MODEL_MANAGER_HPP

#include "model_object.hpp"
#include "../utils/manager.hpp"


/** Manages the model objects, and some worker threads. */
class ModelManager: public Manager<ModelObject> {
public:
    ModelManager() = default;

private:
};


#endif //DEMO_TD_MODEL_MANAGER_HPP

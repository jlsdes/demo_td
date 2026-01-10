#ifndef DEMO_TD_MODEL_MANAGER_HPP
#define DEMO_TD_MODEL_MANAGER_HPP

#include "model_object.hpp"
#include "../utils/manager.hpp"

#include <memory>
#include <vector>


/** Manages the model objects, and some worker threads. */
class ModelManager: public Manager<ModelObject> {
public:
    ModelManager() = default;

    /** Updates all registered ModelObjects in parallel using some worker threads. */
    void update_models();

private:
    /// All registered models and their IDs; these are stored in ascending order of their IDs.
    std::vector<std::pair<unsigned int, std::unique_ptr<ModelObject>>> m_models;
};


#endif //DEMO_TD_MODEL_MANAGER_HPP

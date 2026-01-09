#ifndef DEMO_TD_MODEL_MANAGER_HPP
#define DEMO_TD_MODEL_MANAGER_HPP

#include "model_object.hpp"

#include <memory>
#include <vector>


/** Manages the model objects, and some worker threads. */
class ModelManager {
public:
    ModelManager() = default;

    /** Adds the ModelObject to the ModelManager and returns the object's new ID, which can be used to retrieve it or
     *  remove it from the manager. */
    unsigned int add_model( std::unique_ptr<ModelObject> && model );
    /** Returns a non-owning pointer to the model object based on its ID, as returned by add_model(). If the given ID
     *  can't be found, then a nullptr is returned. */
    [[nodiscard]] ModelObject * get_model( unsigned int model_id ) const;
    /** Removes the ModelObject from the ModelManager, and returns whether this was successful. */
    bool remove_model( unsigned int model_id );

    /** Updates all registered ModelObjects in parallel using some worker threads. */
    void update_models();

private:
    /// All registered models and their IDs; these are stored in ascending order of their IDs.
    std::vector<std::pair<unsigned int, std::unique_ptr<ModelObject>>> m_models;
};


#endif //DEMO_TD_MODEL_MANAGER_HPP

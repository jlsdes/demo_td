#ifndef DEMO_TD_ENTITY_FACTORY_HPP
#define DEMO_TD_ENTITY_FACTORY_HPP

#include "controller_object.hpp"
#include "model_object.hpp"
#include "view_object.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>


class ModelManager;
class ViewManager;
class ControllerManager;


/// Utility struct holding the IDs of newly constructed entities.
struct Entity {
    unsigned int model_id;
    unsigned int view_id;
    unsigned int controller_id;
    bool success;
};


template <ManagedType T>
using FactoryFunction = std::function<std::unique_ptr<T>()>;


/** Creates entities consisting of a model, view, and controller component, and performs the required setup. */
class EntityFactory {
public:
    EntityFactory( ModelManager * model_manager, ViewManager * view_manager, ControllerManager * controller_manager );
    ~EntityFactory() = default;

    /** Sets a manager under which newly created objects will be registered. */
    void set_model_manager( ModelManager * model_manager );
    void set_view_manager( ViewManager * view_manager );
    void set_controller_manager( ControllerManager * controller_manager );

    /** Registers a new factory function for creating the entity component types. If a factory function with the given
     *  name already exists, then the new one will only replace it if 'override' is true. If not, nothing happens and
     *  false is returned. */
    bool register_model_factory( std::string const & name,
                                 FactoryFunction<ModelObject> const & factory,
                                 bool override = false );
    bool register_view_factory( std::string const & name,
                                FactoryFunction<ViewObject> const & factory,
                                bool override = false );
    bool register_controller_factory( std::string const & name,
                                      FactoryFunction<ControllerObject> const & factory,
                                      bool override = false );

    /** Creates a new entity consisting of a model, view, and controller component. Returns an Entity struct containing
     *  their IDs as returned by their respective managers. */
    [[nodiscard]] Entity build( std::string const & model_type,
                                std::string const & view_type,
                                std::string const & controller_type ) const;

private:
    /// Managers to send the created objects to.
    ModelManager * m_model_manager;
    ViewManager * m_view_manager;
    ControllerManager * m_controller_manager;

    /// Individual component creator functions.
    std::map<std::string, std::function<std::unique_ptr<ModelObject>()>> m_model_factories;
    std::map<std::string, std::function<std::unique_ptr<ViewObject>()>> m_view_factories;
    std::map<std::string, std::function<std::unique_ptr<ControllerObject>()>> m_controller_factories;
};


#endif //DEMO_TD_ENTITY_FACTORY_HPP

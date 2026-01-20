#ifndef DEMO_TD_ENTITY_FACTORY_HPP
#define DEMO_TD_ENTITY_FACTORY_HPP

#include "controller_object.hpp"
#include "model_object.hpp"
#include "view_object.hpp"

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>


class ModelManager;
class ViewManager;
class ControllerManager;


/// Utility struct holding the IDs of newly constructed entities.
struct Entity {
    std::pair<unsigned int, ModelObject *> model;
    std::pair<unsigned int, ViewObject *> view;
    std::pair<unsigned int, ControllerObject *> controller;
    bool success;
};


using ModelFactory = std::function<std::unique_ptr<ModelObject>()>;
using ViewFactory = std::function<std::unique_ptr<ViewObject>( ModelObject * )>;
using ControllerFactory = std::function<std::unique_ptr<ControllerObject>( ModelObject * )>;


/** Returns a basic ViewFactory for views that take their associated model as the only constructor parameter. */
template <ViewType View, ModelType Model>
ViewFactory get_view_factory();

/** Returns a basic ControllerFactory for controllers that take their model as the only constructor parameter. */
template <ControllerType Controller, ModelType Model>
ControllerFactory get_controller_factory();


/** Singleton that creates entities consisting of a model, view, and controller component, and performs the required
 *  setup. */
class EntityFactory {
    /** Private constructor, use get_instance() instead. */
    EntityFactory();

public:
    EntityFactory( EntityFactory const & ) = delete;
    EntityFactory & operator=( EntityFactory const & ) = delete;
    EntityFactory( EntityFactory && ) = delete;
    EntityFactory & operator=( EntityFactory && ) = delete;
    ~EntityFactory() = default;

    /** Returns the same EntityFactory instance every time. */
    static EntityFactory & get_instance();

    /** Finishes initialising the factory with all managers being non-null afterwards. This function can be used to set
     *  the managers, but if the given pointers are null then they should've been set earlier. If the currently active
     *  manager and the given pointer are both null, then an exception will be thrown. This function can be skipped if
     *  all managers are set correctly using the set_manager_*() functions. */
    void initialise( ModelManager * model_manager = nullptr,
                     ViewManager * view_manager = nullptr,
                     ControllerManager * controller_manager = nullptr );

    /** Sets a manager under which newly created objects will be registered. */
    void set_model_manager( ModelManager * model_manager );
    void set_view_manager( ViewManager * view_manager );
    void set_controller_manager( ControllerManager * controller_manager );

    /** Registers a new factory function for creating the entity component types. If a factory function with the given
     *  name already exists, then the new one will only replace it if 'override' is true. If not, nothing happens and
     *  false is returned. */
    bool register_model_factory( std::string const & name, ModelFactory const & factory, bool override = false );
    bool register_view_factory( std::string const & name, ViewFactory const & factory, bool override = false );
    bool register_controller_factory( std::string const & name,
                                      ControllerFactory const & factory,
                                      bool override = false );

    /** Creates a new entity consisting of a model, view, and controller component. Returns an Entity struct containing
     *  their IDs as returned by their respective managers. The returned Entity's 'success' flag is set according to
     *  whether the construction succeeded. */
    [[nodiscard]] Entity build( std::string const & model_type,
                                std::string const & view_type,
                                std::string const & controller_type );
    [[nodiscard]] Entity build( std::string const & type );

private:
    bool m_initialised;

    /// Managers to send the created objects to.
    ModelManager * m_model_manager;
    ViewManager * m_view_manager;
    ControllerManager * m_controller_manager;

    /// Individual component creator functions.
    std::map<std::string, ModelFactory> m_model_factories;
    std::map<std::string, ViewFactory> m_view_factories;
    std::map<std::string, ControllerFactory> m_controller_factories;

    /// Thread synchronisation primitive(s)
    std::mutex m_mutex;
};


// Template definitions


template <ViewType View, ModelType Model>
ViewFactory get_view_factory() {
    return []( ModelObject * model ) { return std::make_unique<View>( dynamic_cast<Model *>(model) ); };
}

template <ControllerType Controller, ModelType Model>
ControllerFactory get_controller_factory() {
    return []( ModelObject * model ) { return std::make_unique<Controller>( dynamic_cast<Model *>(model) ); };
}


#endif //DEMO_TD_ENTITY_FACTORY_HPP

#include "entity_factory.hpp"
#include "controller_manager.hpp"
#include "model_manager.hpp"
#include "view_manager.hpp"
#include "utils/log.hpp"

#include <stdexcept>


EntityFactory::EntityFactory()
    : m_initialised { false }, m_model_manager { nullptr }, m_view_manager { nullptr },
      m_controller_manager { nullptr }, m_model_factories {}, m_view_factories {}, m_controller_factories {} {}

EntityFactory & EntityFactory::get_instance() {
    static EntityFactory instance {};
    return instance;
}

void EntityFactory::initialise( ModelManager * const model_manager,
                                ViewManager * const view_manager,
                                ControllerManager * const controller_manager ) {
    if ( m_initialised )
        Log::warning( "Initialised EntityFactory already, overwriting old managers where necessary." );

    if ( model_manager )
        set_model_manager( model_manager );
    else if ( not m_model_manager )
        throw std::runtime_error( "Failed to properly initialise the EntityFactory's ModelManager." );

    if ( view_manager )
        set_view_manager( view_manager );
    else if ( not m_view_manager )
        throw std::runtime_error( "Failed to properly initialise the EntityFactory's ViewManager." );

    if ( controller_manager )
        set_controller_manager( controller_manager );
    else if ( not m_controller_manager )
        throw std::runtime_error( "Failed to properly initialise the EntityFactory's ControllerManager." );

    m_initialised = true;
}

void EntityFactory::set_model_manager( ModelManager * const model_manager ) {
    if ( model_manager ) {
        std::lock_guard lock { m_mutex };
        m_model_manager = model_manager;
    } else
        Log::error( "Attempted to set an EntityFactory's ModelManager to null; keeping the old manager." );
}

void EntityFactory::set_view_manager( ViewManager * const view_manager ) {
    if ( view_manager ) {
        std::lock_guard lock { m_mutex };
        m_view_manager = view_manager;
    } else
        Log::error( "Attempted to set an EntityFactory's ViewManager to null; keeping the old manager." );
}

void EntityFactory::set_controller_manager( ControllerManager * const controller_manager ) {
    if ( controller_manager ) {
        std::lock_guard lock { m_mutex };
        m_controller_manager = controller_manager;
    } else
        Log::error( "Attempted to set an EntityFactory's ControllerManager to null; keeping the old manager." );
}

template <typename FactoryType>
bool register_factory( std::map<std::string, FactoryType> & registry,
                       std::string const & name,
                       FactoryType const & factory,
                       bool const override ) {
    if ( override ) {
        registry.insert_or_assign( name, factory );
        return true;
    }
    auto const [_, success] { registry.emplace( name, factory ) };
    return success;
}

bool EntityFactory::register_model_factory( std::string const & name,
                                            ModelFactory const & factory,
                                            bool const override ) {
    std::lock_guard lock { m_mutex };
    return register_factory( m_model_factories, name, factory, override );
}

bool EntityFactory::register_view_factory( std::string const & name,
                                           ViewFactory const & factory,
                                           bool const override ) {
    std::lock_guard lock { m_mutex };
    return register_factory( m_view_factories, name, factory, override );
}

bool EntityFactory::register_controller_factory( std::string const & name,
                                                 ControllerFactory const & factory,
                                                 bool const override ) {
    std::lock_guard lock { m_mutex };
    return register_factory( m_controller_factories, name, factory, override );
}

Entity EntityFactory::build( std::string const & model_type,
                             std::string const & view_type,
                             std::string const & controller_type ) {
    // Return value if the entity construction failed
    Entity constexpr failed { { 0, nullptr }, { 0, nullptr }, { 0, nullptr }, false };

    if ( not m_model_factories.contains( model_type ) ) {
        Log::error( "Attempted to build an entity with an invalid model type '", model_type, "'." );
        return failed;
    }
    if ( not m_view_factories.contains( view_type ) ) {
        Log::error( "Attempted to build an entity with an invalid view type '", view_type, "'." );
        return failed;
    }
    if ( not m_controller_factories.contains( controller_type ) ) {
        Log::error( "Attempted to build an entity with an invalid controller type '", controller_type, "'." );
        return failed;
    }

    std::lock_guard lock { m_mutex };

    auto model { m_model_factories.at( model_type )() };
    ModelObject * const model_raw { model.get() };
    auto view { m_view_factories.at( view_type )( model.get() ) };
    ViewObject * const view_raw { view.get() };
    auto controller { m_controller_factories.at( controller_type )( model.get() ) };
    ControllerObject * const controller_raw { controller.get() };

    unsigned int const model_id { model ? m_model_manager->push( std::move( model ) ) : 0 };
    unsigned int const view_id { view ? m_view_manager->push( std::move( view ) ) : 0 };
    unsigned int const controller_id { controller ? m_controller_manager->push( std::move( controller ) ) : 0 };

    return { { model_id, model_raw }, { view_id, view_raw }, { controller_id, controller_raw }, true };
}

Entity EntityFactory::build( std::string const & type ) {
    return build( type, type, type );
}

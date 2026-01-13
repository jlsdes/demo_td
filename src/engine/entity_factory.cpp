#include "entity_factory.hpp"
#include "controller_manager.hpp"
#include "model_manager.hpp"
#include "view_manager.hpp"
#include "utils/log.hpp"

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
        Log::warning( "Initialised EntityFactory already, overwriting old managers." );
    set_model_manager( model_manager );
    set_view_manager( view_manager );
    set_controller_manager( controller_manager );
}

void EntityFactory::set_model_manager( ModelManager * const model_manager ) {
    if ( model_manager ) {
        std::lock_guard lock { m_mutex };
        m_model_manager = model_manager;
    } else
        Log::error( "Attempted to set an EntityFactory's model manager to null." );
}

void EntityFactory::set_view_manager( ViewManager * const view_manager ) {
    if ( view_manager ) {
        std::lock_guard lock { m_mutex };
        m_view_manager = view_manager;
    } else
        Log::error( "Attempted to set an EntityFactory's view manager to null." );
}

void EntityFactory::set_controller_manager( ControllerManager * const controller_manager ) {
    if ( controller_manager ) {
        std::lock_guard lock { m_mutex };
        m_controller_manager = controller_manager;
    } else
        Log::error( "Attempted to set an EntityFactory's controller manager to null." );
}

template <ManagedType T>
bool register_factory( std::map<std::string, FactoryFunction<T>> & registry,
                       std::string const & name,
                       FactoryFunction<T> const & factory,
                       bool const override ) {
    if ( override ) {
        registry.insert_or_assign( name, factory );
        return true;
    }
    auto const [_, success] { registry.emplace( name, factory ) };
    return success;
}

bool EntityFactory::register_model_factory( std::string const & name,
                                            FactoryFunction<ModelObject> const & factory,
                                            bool const override ) {
    std::lock_guard lock { m_mutex };
    return register_factory( m_model_factories, name, factory, override );
}

bool EntityFactory::register_view_factory( std::string const & name,
                                           FactoryFunction<ViewObject> const & factory,
                                           bool const override ) {
    std::lock_guard lock { m_mutex };
    return register_factory( m_view_factories, name, factory, override );
}

bool EntityFactory::register_controller_factory( std::string const & name,
                                                 FactoryFunction<ControllerObject> const & factory,
                                                 bool const override ) {
    std::lock_guard lock { m_mutex };
    return register_factory( m_controller_factories, name, factory, override );
}

template <ManagedType T>
std::unique_ptr<T> build_component( std::map<std::string, FactoryFunction<T>> const & registry,
                                    std::string const & name,
                                    std::string const & type_name ) {
    if ( not registry.contains( name ) ) {
        Log::error( "Attempted to build an entity with an invalid ", type_name, " type '", name, "'." );
        return nullptr;
    }
    return registry.at( name )();
}

Entity EntityFactory::build( std::string const & model_type,
                             std::string const & view_type,
                             std::string const & controller_type ) {
    Entity constexpr failed { 0, 0, 0, false }; // Return value if the entity construction failed

    std::lock_guard lock { m_mutex };

    auto model { build_component<ModelObject>( m_model_factories, model_type, "model" ) };
    if ( not model ) return failed;
    auto view { build_component<ViewObject>( m_view_factories, view_type, "view" ) };
    if ( not view ) return failed;
    auto controller { build_component<ControllerObject>( m_controller_factories, controller_type, "controller" ) };
    if ( not controller ) return failed;

    unsigned int const model_id { m_model_manager->push( std::move( model ) ) };
    unsigned int const view_id { m_view_manager->push( std::move( view ) ) };
    unsigned int const controller_id { m_controller_manager->push( std::move( controller ) ) };

    return { model_id, view_id, controller_id, true };
}

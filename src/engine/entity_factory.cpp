#include "entity_factory.hpp"
#include "utils/log.hpp"

EntityFactory::EntityFactory( ModelManager * const model_manager,
                              Renderer * const view_manager,
                              ControllerManager * const controller_manager )
    : m_model_manager( model_manager ), m_view_manager( view_manager ), m_controller_manager( controller_manager ),
      m_model_factories {}, m_view_factories {}, m_controller_factories {} {}

void EntityFactory::set_model_manager( ModelManager * const model_manager ) {
    if ( model_manager )
        m_model_manager = model_manager;
    else
        Log::error( "Attempted to set an EntityFactory's model manager to null." );
}

void EntityFactory::set_view_manager( Renderer * const view_manager ) {
    if ( view_manager )
        m_view_manager = view_manager;
    else
        Log::error( "Attempted to set an EntityFactory's view manager to null." );
}

void EntityFactory::set_controller_manager( ControllerManager * const controller_manager ) {
    if ( controller_manager )
        m_controller_manager = controller_manager;
    else
        Log::error( "Attempted to set an EntityFactory's controller manager to null." );
}

template <ManagedType T>
bool register_factory( std::map<std::string, FactoryFunction<T>> & registry,
                       std::string const & name,
                       FactoryFunction<T> const & factory,
                       bool const override ) {
    if ( override ) {
        registry[name] = factory;
        return true;
    }
    auto const [_, success] { registry.emplace( name, factory ) };
    return success;
}

bool EntityFactory::register_model_factory( std::string const & name,
                                            FactoryFunction<ModelObject> const & factory,
                                            bool const override ) {
    return register_factory( m_model_factories, name, factory, override );
}

bool EntityFactory::register_view_factory( std::string const & name,
                                           FactoryFunction<ViewObject> const & factory,
                                           bool const override ) {
    return register_factory( m_view_factories, name, factory, override );
}

bool EntityFactory::register_controller_factory( std::string const & name,
                                                 FactoryFunction<ControllerObject> const & factory,
                                                 bool const override ) {
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
                             std::string const & controller_type ) const {
    Entity constexpr failed { 0, 0, 0, false };
    auto model { build_component<ModelObject>( m_model_factories, model_type, "model" ) };
    if ( not model ) return failed;
    auto view { build_component<ViewObject>( m_view_factories, view_type, "view" ) };
    if ( not view ) return failed;
    auto controller { build_component<ControllerObject>( m_controller_factories, controller_type, "controller" ) };
    if ( not controller ) return failed;

    unsigned int const model_id { m_model_manager->push( std::move( model ) ) };
    // unsigned int const view_id { m_view_manager->push( std::move( view ) ) };
    unsigned int const view_id { 0 };
    unsigned int const controller_id { m_controller_manager->push( std::move( controller ) ) };

    return { model_id, view_id, controller_id, true };
}

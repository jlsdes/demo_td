#include "model_object.hpp"
#include "view_object.hpp"
#include "controller_object.hpp"
#include "utils/log.hpp"

#include <cassert>


unsigned int ModelObject::s_old_iteration { 0 };
unsigned int ModelObject::s_new_iteration { 1 };
unsigned int ModelObject::s_render_data { 0 };
std::mutex ModelObject::m_mutex {};


ModelObject::ModelObject() : m_data { nullptr }, m_view { nullptr }, m_controller { nullptr } {}

ModelObject::~ModelObject() {
    if ( m_controller )
        m_controller->destroy();
    if ( m_view )
        m_view->deferred_destroy();
}

void ModelObject::set_view( ViewObject * const view ) {
    if ( m_view )
        Log::warning( "Attempted to set a ModelObject's view twice; ignoring second ViewObject." );
    else
        m_view = view;
}

void ModelObject::set_controller( ControllerObject * const controller ) {
    if ( m_controller )
        Log::warning( "Attempted to set a ModelObject's controller twice; ignoring second ControllerObject." );
    else
        m_controller = controller;
}

ModelData const * ModelObject::get_render_data() const {
    return m_data[s_render_data];
}

ModelData const * ModelObject::get_old_data() const {
    return m_data[s_old_iteration];
}

ModelData * ModelObject::get_new_data() const {
    return m_data[s_new_iteration];
}

void ModelObject::update() {}

void ModelObject::swap_model_state() {
    assert( s_new_iteration != s_old_iteration );
    assert( s_new_iteration != s_render_data );

    std::lock_guard lock { m_mutex };
    // Currently, render_data could be equal to old_iteration, and new_iteration is different from both of them
    s_old_iteration = s_new_iteration;
    // => new_iteration == old_iteration != render_data
    s_new_iteration = 0 ^ 1 ^ 2 ^ s_old_iteration ^ s_render_data;
    // => old_iteration != new_iteration != render_data

    assert( s_new_iteration != s_old_iteration );
    assert( s_new_iteration != s_render_data );
    assert( s_old_iteration != s_render_data );
}

void ModelObject::set_render_state() {
    std::lock_guard lock { m_mutex };
    s_render_data = s_old_iteration;
}

void ModelObject::initialise_data( std::array<ModelData *, 3> && data ) {
    m_data = std::move( data );
}

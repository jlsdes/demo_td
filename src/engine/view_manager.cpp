#include "view_manager.hpp"
#include "utils/log.hpp"

#include <ranges>
#include <utility>


RenderQueue::~RenderQueue() = default;

void RenderQueue::push( ViewObject const * object ) {
    push( *object );
}

NaiveRenderQueue::~NaiveRenderQueue() = default;

void NaiveRenderQueue::push( ViewObject const & object ) {
    m_objects.push( &object );
}

ViewObject const & NaiveRenderQueue::pop() {
    ViewObject const * const first { m_objects.front() };
    m_objects.pop();
    return *first;
}

bool NaiveRenderQueue::empty() {
    return m_objects.empty();
}

ViewManager::ViewManager( std::unique_ptr<RenderQueue> && queue )
    : m_queue { queue ? std::move( queue ) : std::make_unique<NaiveRenderQueue>() }, m_last_id { 0 } {}

ViewManager::~ViewManager() = default;

void ViewManager::draw() {
    for ( auto const & [object_id, object] : m_objects ) {
        auto const view_object { dynamic_cast<ViewObject *>( object.get() ) };
        m_queue->push( view_object );

        // Object IDs are generated in ascending order, so by keeping track of the last ID that currently exists, new
        // objects can be detected and initialised
        if ( object_id > m_last_id ) {
            view_object->initialise_mesh();
            m_last_id = object_id;
        }
    }
    while ( !m_queue->empty() )
        m_queue->pop().draw();
}

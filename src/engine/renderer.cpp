#include "renderer.hpp"

#include "../utils/log.hpp"

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

Renderer::Renderer( std::unique_ptr<RenderQueue> && queue )
    : m_queue { queue ? std::move( queue ) : std::make_unique<NaiveRenderQueue>() } {}

Renderer::~Renderer() = default;

void Renderer::register_object( ViewObject const & object ) {
    m_objects.emplace_back( &object );
}

void Renderer::unregister_object( ViewObject const & object ) {
    auto const location { std::ranges::find( std::as_const(m_objects), &object ) };
    if ( location == m_objects.cend() )
        Log::error( "Attempting to remove a RenderObject that cannot be found." );
    else
        m_objects.erase( location );
}

void Renderer::draw() const {
    // Push everything into the queue
    for ( auto const & object : m_objects )
        m_queue->push( object );
    // Pop everything from the queue again
    while ( !m_queue->empty() )
        m_queue->pop().draw();
}

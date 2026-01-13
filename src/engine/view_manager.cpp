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
    : m_queue { queue ? std::move( queue ) : std::make_unique<NaiveRenderQueue>() } {}

ViewManager::~ViewManager() = default;

void ViewManager::draw() const {
    for ( auto const & object : std::ranges::views::values( m_objects ) )
        m_queue->push( object.get() );
    while ( !m_queue->empty() )
        m_queue->pop().draw();
}

#include "manager.hpp"

#include <algorithm>


void ManagedObject::destroy() {
    if ( m_manager )
        m_manager->pop( m_id );
    m_manager = nullptr;
    m_id = 0;
}

unsigned int Manager::push( std::unique_ptr<ManagedObject> && object ) {
    object->m_manager = this;
    object->m_id = m_next_id;
    m_objects.emplace_back( m_next_id, std::move( object ) );
    return m_next_id++;
}

using ObjectVector = std::vector<std::pair<unsigned int, std::unique_ptr<ManagedObject>>>;

ObjectVector::const_iterator binary_search( ObjectVector const & objects, unsigned int const id ) {
    if ( objects.empty() )
        return objects.cend();

    unsigned int min_index { 0 }, max_index { static_cast<unsigned int>( objects.size() ) };
    while ( min_index != max_index ) {
        unsigned int const mid_index { (min_index + max_index) / 2 };
        auto const iterator { objects.cbegin() + mid_index };

        if ( id == iterator->first )
            return iterator;
        if ( id < iterator->first )
            max_index = mid_index - 1;
        else // id > iterator->first
            min_index = mid_index + 1;
    }
    auto const iterator { objects.cbegin() + min_index };
    return id == iterator->first ? iterator : objects.cend();
}

bool Manager::pop( unsigned int const object_id ) {
    auto const iterator { binary_search( m_objects, object_id ) };
    bool const id_found { iterator != m_objects.cend() };
    if ( id_found )
        m_objects.erase( iterator );
    return id_found;
}

bool Manager::contains( unsigned int const object_id ) const {
    auto const iterator { binary_search( m_objects, object_id ) };
    return iterator != m_objects.cend();
}

ManagedObject * Manager::get( unsigned int const object_id ) const {
    auto const iterator { binary_search( m_objects, object_id ) };
    return iterator == m_objects.cend() ? nullptr : iterator->second.get();
}

unsigned int Manager::size() const {
    return m_objects.size();
}

Manager::Iterator::Iterator( Manager const & manager, unsigned int const index )
    : m_manager { manager }, m_index { index } {}

Manager::Iterator & Manager::Iterator::operator++() {
    ++m_index;
    return *this;
}

Manager::Iterator & Manager::Iterator::operator--() {
    --m_index;
    return *this;
}

Manager::Iterator Manager::Iterator::operator++( int ) {
    ++m_index;
    return { m_manager, m_index - 1 };
}

Manager::Iterator Manager::Iterator::operator--( int ) {
    --m_index;
    return { m_manager, m_index + 1 };
}

Manager::Iterator Manager::Iterator::operator+( unsigned int const offset ) {
    return { m_manager, m_index + offset };
}

Manager::Iterator Manager::Iterator::operator-( unsigned int const offset ) {
    return { m_manager, m_index - offset };
}

bool Manager::Iterator::operator==( Iterator const & other ) const {
    return &m_manager == &other.m_manager && m_index == other.m_index;
}

bool Manager::Iterator::operator!=( Iterator const & other ) const {
    return &m_manager != &other.m_manager || m_index != other.m_index;
}

ManagedObject & Manager::Iterator::operator*() const {
    return *m_manager.m_objects.at( m_index ).second.get();
}

ManagedObject * Manager::Iterator::operator->() const {
    return m_manager.m_objects.at( m_index ).second.get();
}

Manager::Iterator Manager::begin() const {
    return { *this, 0 };
}

Manager::Iterator Manager::end() const {
    return { *this, size() };
}

void Manager::update() const {
    static WorkerPool<g_workers_per_manager_type> workers {};
    workers.update( *this );
}


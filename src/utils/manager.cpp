#include "manager.hpp"

#include <algorithm>


void ManagedObject::destroy() {
    if ( m_manager )
        m_manager->pop( m_id );
    m_manager = nullptr;
    m_id = 0;
}

void ManagedObject::deferred_destroy() {
    m_to_be_destroyed = true;
}

bool ManagedObject::is_to_be_destroyed() const {
    return m_to_be_destroyed;
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

    auto constexpr proj {
        []( std::pair<unsigned int, std::unique_ptr<ManagedObject>> const & element ) { return element.first; }
    };
    auto const iterator { std::ranges::lower_bound( objects, id, {}, proj ) };

    return iterator != objects.cend() and iterator->first == id ? iterator : objects.cend();
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

Manager::Iterator Manager::Iterator::operator+( unsigned int const offset ) const {
    return { m_manager, m_index + offset };
}

Manager::Iterator Manager::Iterator::operator-( unsigned int const offset ) const {
    return { m_manager, m_index - offset };
}

bool Manager::Iterator::operator==( Iterator const & other ) const {
    return &m_manager == &other.m_manager && m_index == other.m_index;
}

bool Manager::Iterator::operator!=( Iterator const & other ) const {
    return &m_manager != &other.m_manager || m_index != other.m_index;
}

ManagedObject & Manager::Iterator::operator*() const {
    return *m_manager.m_objects.at( m_index ).second;
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

void Manager::update() {
    thread_local WorkerPool<g_workers_per_manager_type> workers {};
    workers.update( *this );
}

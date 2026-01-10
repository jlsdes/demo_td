#ifndef DEMO_TD_MANAGER_HPP
#define DEMO_TD_MANAGER_HPP

#include <cassert>
#include <iterator>
#include <memory>
#include <vector>


template <typename ObjectType>
using IdPair = std::pair<unsigned int, std::unique_ptr<ObjectType>>;


/** A base class for any kind of manager class. */
template <typename ObjectType>
class Manager {
public:
    Manager() = default;

    [[nodiscard]] bool contains( unsigned int object_id ) const;
    [[nodiscard]] ObjectType * get( unsigned int object_id ) const;
    [[nodiscard]] size_t size() const;

    unsigned int push( std::unique_ptr<ObjectType> && object );
    bool pop( unsigned int object_id );

    /** Simple iterator wrapping the m_objects iterator. */
    class Iterator {
    public:
        Iterator( Manager const & manager, unsigned int index );

        Iterator & operator++();
        Iterator & operator--();
        Iterator operator++( int );
        Iterator operator--( int );

        Iterator operator+( unsigned int offset );
        Iterator operator-( unsigned int offset );

        bool operator==( Iterator const & other ) const;
        bool operator!=( Iterator const & other ) const;

        ObjectType & operator*() const;
        ObjectType * operator->() const;

    private:
        Manager const & m_manager;
        unsigned int m_index;
    };

    Iterator begin() const;
    Iterator end() const;

protected:
    /// All registered objects and their IDs; these are stored in strictly increasing order of ID.
    std::vector<IdPair<ObjectType>> m_objects;

private:
    unsigned int m_next_id { 0 };

    std::vector<IdPair<ObjectType>>::const_iterator binary_search( unsigned int id ) const;
};


template <typename ObjectType>
unsigned int Manager<ObjectType>::push( std::unique_ptr<ObjectType> && object ) {
    m_objects.emplace_back( m_next_id, std::move( object ) );
    return m_next_id++;
}

template <typename ObjectType>
bool Manager<ObjectType>::pop( unsigned int const object_id ) {
    auto const iterator { binary_search( object_id ) };
    bool const id_found { iterator != m_objects.cend() };
    if ( id_found )
        m_objects.erase( iterator );
    return id_found;
}

template <typename ObjectType>
bool Manager<ObjectType>::contains( unsigned int const object_id ) const {
    auto const iterator { binary_search( object_id ) };
    return iterator != m_objects.cend();
}

template <typename ObjectType>
ObjectType * Manager<ObjectType>::get( unsigned int const object_id ) const {
    auto const iterator { binary_search( object_id ) };
    return iterator == m_objects.cend() ? nullptr : iterator->second.get();
}

template <typename ObjectType>
size_t Manager<ObjectType>::size() const {
    return m_objects.size();
}

template <typename ObjectType>
std::vector<IdPair<ObjectType>>::const_iterator Manager<ObjectType>::binary_search( unsigned int const id ) const {
    if ( m_objects.empty() )
        return m_objects.cend();

    unsigned int min_index { 0 }, max_index { size() };
    while ( min_index != max_index ) {
        unsigned int const mid_index { (min_index + max_index) / 2 };
        auto const iterator { m_objects.cbegin() + mid_index };

        if ( id == iterator->first )
            return iterator;
        if ( id < iterator->first )
            max_index = mid_index - 1;
        else // id > iterator->first
            min_index = mid_index + 1;
    }
    auto const iterator { m_objects.cbegin() + min_index };
    return id == iterator->first ? iterator : m_objects.cend();
}

template <typename ObjectType>
Manager<ObjectType>::Iterator::Iterator( Manager const & manager, unsigned int const index )
    : m_manager { manager }, m_index { index } {}

template <typename ObjectType>
Manager<ObjectType>::Iterator & Manager<ObjectType>::Iterator::operator++() {
    ++m_index;
    return *this;
}

template <typename ObjectType>
Manager<ObjectType>::Iterator & Manager<ObjectType>::Iterator::operator--() {
    --m_index;
    return *this;
}

template <typename ObjectType>
Manager<ObjectType>::Iterator Manager<ObjectType>::Iterator::operator++( int ) {
    ++m_index;
    return { m_manager, m_index - 1 };
}

template <typename ObjectType>
Manager<ObjectType>::Iterator Manager<ObjectType>::Iterator::operator--( int ) {
    --m_index;
    return { m_manager, m_index + 1 };
}

template <typename ObjectType>
Manager<ObjectType>::Iterator Manager<ObjectType>::Iterator::operator+( unsigned int const offset ) {
    return { m_manager, m_index + offset };
}

template <typename ObjectType>
Manager<ObjectType>::Iterator Manager<ObjectType>::Iterator::operator-( unsigned int const offset ) {
    return { m_manager, m_index - offset };
}

template <typename ObjectType>
bool Manager<ObjectType>::Iterator::operator==( Iterator const & other ) const {
    return &m_manager == &other.m_manager && m_index == other.m_index;
}

template <typename ObjectType>
bool Manager<ObjectType>::Iterator::operator!=( Iterator const & other ) const {
    return &m_manager != &other.m_manager || m_index != other.m_index;
}

template <typename ObjectType>
ObjectType & Manager<ObjectType>::Iterator::operator*() const {
    return *m_manager.m_objects.at( m_index ).second.get();
}

template <typename ObjectType>
ObjectType * Manager<ObjectType>::Iterator::operator->() const {
    return m_manager.m_objects.at( m_index ).second.get();
}

template <typename ObjectType>
Manager<ObjectType>::Iterator Manager<ObjectType>::begin() const {
    return { *this, 0 };
}

template <typename ObjectType>
Manager<ObjectType>::Iterator Manager<ObjectType>::end() const {
    return { *this, static_cast<unsigned int>(size()) };
}

#endif //DEMO_TD_MANAGER_HPP

#ifndef DEMO_TD_MANAGER_HPP
#define DEMO_TD_MANAGER_HPP

#include <array>
#include <memory>
#include <mutex>
#include <semaphore>
#include <thread>
#include <vector>


/** Abstract base class for types being managed by a Manager object. */
class ManagedObject {
public:
    virtual ~ManagedObject() = default;
    virtual void update() = 0;
};

/** Defines a type derived from ManagedObject. Objects of this type must have a function `void update()`. */
template <typename T>
concept ManagedType = std::is_base_of_v<ManagedObject, T>;


template <ManagedType ObjectType>
using IdPair = std::pair<unsigned int, std::unique_ptr<ObjectType>>;


template <unsigned int buffer_size>
class WorkerPool;

/// Some constant parameters for the WorkerPool instances
unsigned int constexpr g_workers_per_manager_type { 4 };
unsigned int constexpr g_worker_buffer_size { 4 * g_workers_per_manager_type };


/** A base class for any kind of manager class. */
template <ManagedType ObjectType>
class Manager {
public:
    /** Constructors and destructor; copying is not allowed to avoid having multiple Manager objects managing the same
     *  object(s). */
    Manager() = default;
    Manager( Manager const & ) = delete;
    Manager & operator=( Manager const & ) = delete;
    Manager( Manager && ) = default;
    Manager & operator=( Manager && ) = default;
    virtual ~Manager() = default;

    [[nodiscard]] bool contains( unsigned int object_id ) const;
    [[nodiscard]] ObjectType * get( unsigned int object_id ) const;
    [[nodiscard]] unsigned int size() const;

    unsigned int push( std::unique_ptr<ObjectType> && object );
    bool pop( unsigned int object_id );

    /** A random-access iterator for accessing the stored objects. */
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

    void update() const;

protected:
    /// All registered objects and their IDs; these are stored in strictly increasing order of ID.
    std::vector<IdPair<ObjectType>> m_objects;

private:
    /// A counter to prevent multiple objects being registered with the same ID.
    unsigned int m_next_id { 0 };

    /** Utility function to access object pairs internally. */
    std::vector<IdPair<ObjectType>>::const_iterator binary_search( unsigned int id ) const;
};


/** A pool of worker threads that call objects' update() function. */
template <unsigned int buffer_size>
class WorkerPool {
public:
    explicit WorkerPool( unsigned int nr_workers = 4 );
    ~WorkerPool();
    WorkerPool( WorkerPool const & ) = delete;
    WorkerPool & operator=( WorkerPool const & ) = delete;
    WorkerPool( WorkerPool && ) = default;
    WorkerPool & operator=( WorkerPool && ) = default;

    /** Updates all of a manager's registered objects in parallel. */
    template <ManagedType ObjectType>
    void update( Manager<ObjectType> const & manager );

private:
    /** The main function for the worker threads. These threads sleep until the queue gets populated, and then they call
     *  the update() function on everything in the queue. Once everything has been processed, the worker threads go back
     *  to sleep. */
    void worker_thread();

    std::vector<std::thread> m_workers {};
    bool m_finished { false };

    struct CircularBuffer {
        std::array<ManagedObject *, buffer_size> objects {};
        unsigned int begin { 0 };
        unsigned int end { 0 };
        std::counting_semaphore<buffer_size> filled_slots { 0 };
        std::counting_semaphore<buffer_size> empty_slots { buffer_size };
        std::mutex mutex {};
    } m_queue {};
};


// Template definitions

template <ManagedType ObjectType>
unsigned int Manager<ObjectType>::push( std::unique_ptr<ObjectType> && object ) {
    m_objects.emplace_back( m_next_id, std::move( object ) );
    return m_next_id++;
}

template <ManagedType ObjectType>
bool Manager<ObjectType>::pop( unsigned int const object_id ) {
    auto const iterator { binary_search( object_id ) };
    bool const id_found { iterator != m_objects.cend() };
    if ( id_found )
        m_objects.erase( iterator );
    return id_found;
}

template <ManagedType ObjectType>
bool Manager<ObjectType>::contains( unsigned int const object_id ) const {
    auto const iterator { binary_search( object_id ) };
    return iterator != m_objects.cend();
}

template <ManagedType ObjectType>
ObjectType * Manager<ObjectType>::get( unsigned int const object_id ) const {
    auto const iterator { binary_search( object_id ) };
    return iterator == m_objects.cend() ? nullptr : iterator->second.get();
}

template <ManagedType ObjectType>
unsigned int Manager<ObjectType>::size() const {
    return m_objects.size();
}

template <ManagedType ObjectType>
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

template <ManagedType ObjectType>
Manager<ObjectType>::Iterator::Iterator( Manager const & manager, unsigned int const index )
    : m_manager { manager }, m_index { index } {}

template <ManagedType ObjectType>
Manager<ObjectType>::Iterator & Manager<ObjectType>::Iterator::operator++() {
    ++m_index;
    return *this;
}

template <ManagedType ObjectType>
Manager<ObjectType>::Iterator & Manager<ObjectType>::Iterator::operator--() {
    --m_index;
    return *this;
}

template <ManagedType ObjectType>
Manager<ObjectType>::Iterator Manager<ObjectType>::Iterator::operator++( int ) {
    ++m_index;
    return { m_manager, m_index - 1 };
}

template <ManagedType ObjectType>
Manager<ObjectType>::Iterator Manager<ObjectType>::Iterator::operator--( int ) {
    --m_index;
    return { m_manager, m_index + 1 };
}

template <ManagedType ObjectType>
Manager<ObjectType>::Iterator Manager<ObjectType>::Iterator::operator+( unsigned int const offset ) {
    return { m_manager, m_index + offset };
}

template <ManagedType ObjectType>
Manager<ObjectType>::Iterator Manager<ObjectType>::Iterator::operator-( unsigned int const offset ) {
    return { m_manager, m_index - offset };
}

template <ManagedType ObjectType>
bool Manager<ObjectType>::Iterator::operator==( Iterator const & other ) const {
    return &m_manager == &other.m_manager && m_index == other.m_index;
}

template <ManagedType ObjectType>
bool Manager<ObjectType>::Iterator::operator!=( Iterator const & other ) const {
    return &m_manager != &other.m_manager || m_index != other.m_index;
}

template <ManagedType ObjectType>
ObjectType & Manager<ObjectType>::Iterator::operator*() const {
    return *m_manager.m_objects.at( m_index ).second.get();
}

template <ManagedType ObjectType>
ObjectType * Manager<ObjectType>::Iterator::operator->() const {
    return m_manager.m_objects.at( m_index ).second.get();
}

template <ManagedType ObjectType>
Manager<ObjectType>::Iterator Manager<ObjectType>::begin() const {
    return { *this, 0 };
}

template <ManagedType ObjectType>
Manager<ObjectType>::Iterator Manager<ObjectType>::end() const {
    return { *this, size() };
}

template <ManagedType ObjectType>
void Manager<ObjectType>::update() const {
    static WorkerPool<g_workers_per_manager_type> workers {};
    workers.update( *this );
}

template <unsigned int buffer_size>
WorkerPool<buffer_size>::WorkerPool( unsigned int const nr_workers ) : m_workers { nr_workers } {
    for ( auto & thread : m_workers )
        thread = std::thread { &WorkerPool::worker_thread, this };
}

template <unsigned int buffer_size>
WorkerPool<buffer_size>::~WorkerPool() {
    m_finished = true;
    m_queue.filled_slots.release( m_workers.size() );
    for ( auto & thread : m_workers )
        thread.join();
}

template <unsigned int buffer_size>
template <ManagedType ObjectType>
void WorkerPool<buffer_size>::update( Manager<ObjectType> const & manager ) {
    for ( ObjectType & object : manager ) {
        m_queue.empty_slots.acquire();
        {
            std::lock_guard queue_lock { m_queue.mutex };
            m_queue.objects.at( m_queue.end++ ) = &object;
            m_queue.end %= buffer_size;
        }
        m_queue.filled_slots.release();
    }
}

template <unsigned int buffer_size>
void WorkerPool<buffer_size>::worker_thread() {
    m_queue.filled_slots.acquire();
    while ( not m_finished ) {
        ManagedObject * object;
        {
            std::lock_guard queue_lock { m_queue.mutex };
            object = m_queue.objects.at( m_queue.begin++ );
            m_queue.begin %= buffer_size;
        }
        m_queue.empty_slots.release();
        object->update();
        m_queue.filled_slots.acquire();
    }
}

#endif //DEMO_TD_MANAGER_HPP

#ifndef DEMO_TD_MANAGER_HPP
#define DEMO_TD_MANAGER_HPP

#include <array>
#include <memory>
#include <mutex>
#include <semaphore>
#include <thread>
#include <vector>


class Manager;

/** Abstract base class for types being managed by a Manager object. */
class ManagedObject {
public:
    ManagedObject() = default;
    ManagedObject( ManagedObject const & ) = delete;
    ManagedObject & operator=( ManagedObject const & ) = delete;
    ManagedObject( ManagedObject && ) = delete;
    ManagedObject & operator=( ManagedObject && ) = delete;
    virtual ~ManagedObject() = default;

    virtual void update() = 0;

    /** Removes the object from its manager, and in doing so destroys the unique_ptr and thus itself. */
    void destroy();
    void deferred_destroy();
    [[nodiscard]] bool is_to_be_destroyed() const;

private:
    /// Attributes to enable the self-destruct function.
    Manager * m_manager { nullptr };
    unsigned int m_id { 0 };
    bool m_to_be_destroyed { false };

    // Allow the Manager class to change the 'm_manager' and 'm_id' attributes, so that not every derived class of
    // ManagedObject needs to pass these through.
    friend class Manager;
};


template <unsigned int buffer_size>
class WorkerPool;

/// Some constant parameters for the WorkerPool instances
unsigned int constexpr g_workers_per_manager_type { 4 };
unsigned int constexpr g_worker_buffer_size { 4 * g_workers_per_manager_type };


/** A base class for any kind of manager class. */
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
    [[nodiscard]] ManagedObject * get( unsigned int object_id ) const;
    [[nodiscard]] unsigned int size() const;

    unsigned int push( std::unique_ptr<ManagedObject> && object );
    bool pop( unsigned int object_id );

    /** A random-access iterator for accessing the stored objects. */
    class Iterator {
    public:
        Iterator( Manager const & manager, unsigned int index );

        Iterator & operator++();
        Iterator & operator--();
        Iterator operator++( int );
        Iterator operator--( int );

        Iterator operator+( unsigned int offset ) const;
        Iterator operator-( unsigned int offset ) const;

        bool operator==( Iterator const & other ) const;
        bool operator!=( Iterator const & other ) const;

        ManagedObject & operator*() const;
        ManagedObject * operator->() const;

    private:
        Manager const & m_manager;
        unsigned int m_index;
    };

    [[nodiscard]] Iterator begin() const;
    [[nodiscard]] Iterator end() const;

    void update();

protected:
    using IdPair = std::pair<unsigned int, std::unique_ptr<ManagedObject>>;

    /// All registered objects and their IDs; these are stored in strictly increasing order of ID.
    std::vector<IdPair> m_objects;

private:
    /// A counter to prevent multiple objects being registered with the same ID. The IDs start at 1, so 0 can be used to
    /// indicate having no ID / object.
    unsigned int m_next_id { 1 };
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
    void update( Manager & manager );

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


/**********************************************************************************************************************\
* Template definitions                                                                                                 *
\**********************************************************************************************************************/

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
void WorkerPool<buffer_size>::update( Manager & manager ) {
    for ( auto iterator { manager.begin() }; iterator != manager.end(); ++iterator ) {
        ManagedObject & object { *iterator };

        if ( object.is_to_be_destroyed() ) {
            // destroy() modifies the container that is (indirectly) being iterated over. However, the iterator advances
            // an index rather than a pointer, and thus this doesn't invalidate the iterator. The iterator does need to
            // be reversed once to avoid skipping over the next item.
            object.destroy(); // Also, this does modify the manager, which is why it's not marked as 'const'.
            --iterator;
            continue;
        }

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

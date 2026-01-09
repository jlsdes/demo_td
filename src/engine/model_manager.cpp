#include "model_manager.hpp"
#include "../utils/log.hpp"

#include <mutex>
#include <ranges>
#include <semaphore>
#include <thread>


unsigned int ModelManager::add_model( std::unique_ptr<ModelObject> && model ) {
    static unsigned int model_id { 0 };
    m_models.emplace_back( model_id, std::move( model ) );
    return model_id++;
}

using ModelVector = std::vector<std::pair<unsigned int, std::unique_ptr<ModelObject>>>;

/** Returns the index of the model with the given ID, or models.size() if the given ID could not be found. */
ModelVector::const_iterator binary_search( ModelVector const & models, unsigned int const model_id ) {
    if ( models.empty() )
        return models.cend();

    unsigned long min_index { 0 };
    unsigned long max_index { models.size() - 1 };
    while ( min_index != max_index ) {
        unsigned long const centre_index { (min_index + max_index) / 2 };

        auto const iterator { models.cbegin() + static_cast<long>(centre_index) };
        if ( model_id == iterator->first )
            return iterator;

        if ( model_id < iterator->first )
            max_index = centre_index - 1;
        else
            min_index = centre_index + 1;
    }
    auto const iterator { models.cbegin() + static_cast<long>(min_index) };
    return iterator->first == model_id ? iterator : models.cend();
}

ModelObject * ModelManager::get_model( unsigned int const model_id ) const {
    auto const iterator { binary_search( m_models, model_id ) };
    return iterator == m_models.cend() ? nullptr : iterator->second.get();
}

bool ModelManager::remove_model( unsigned int const model_id ) {
    auto const iterator { binary_search( m_models, model_id ) };
    if ( iterator == m_models.cend() )
        return false;
    m_models.erase( iterator );
    return true;
}

/// The buffer size used for the model queue, which delivers models to update to the worker threads.
unsigned int constexpr g_number_workers { 4 };
unsigned int constexpr g_buffer_size { 16 };

/** A group of worker threads used to update the models. */
struct WorkerPool {
    std::array<std::thread, g_number_workers> threads {};

    struct Parameters {
        std::array<ModelObject *, g_buffer_size> queue {};
        unsigned int begin { 0 };
        unsigned int end { 0 };
        bool finished { false };

        std::counting_semaphore<g_buffer_size> filled { 0 };
        std::counting_semaphore<g_buffer_size> empty { g_buffer_size };
        std::mutex queue_mutex {};
    } params {};

    WorkerPool();
    ~WorkerPool();
    WorkerPool( WorkerPool const & ) = delete;
    WorkerPool( WorkerPool && ) = delete;
    WorkerPool & operator=( WorkerPool const & ) = delete;
    WorkerPool & operator=( WorkerPool && ) = delete;
};

void worker_thread( WorkerPool::Parameters & data ) {
    data.filled.acquire();
    while ( not data.finished ) {
        ModelObject * model;
        {
            std::lock_guard queue_lock { data.queue_mutex }; // Lock the queue to extract the next model safely
            model = data.queue.at( data.begin );
            data.begin = (data.begin + 1) % data.queue.size();
            data.empty.release();
        }
        model->update();

        data.filled.acquire(); // Wait until there's something new in the queue for the next iteration
    }
}

WorkerPool::WorkerPool() {
    for ( auto & thread : threads )
        thread = std::thread { worker_thread, std::ref( params ) };
}

WorkerPool::~WorkerPool() {
    params.finished = true;
    params.filled.release( static_cast<long>(threads.size()) );
    for ( auto & thread : threads )
        thread.join();
}

void ModelManager::update_models() {
    static WorkerPool workers {};
    for ( auto & model : std::ranges::views::values( m_models ) ) {

        // If the queue has an empty space, push the next model, else update it in this thread
        if (workers.params.empty.try_acquire()) {
            std::lock_guard queue_lock { workers.params.queue_mutex };
            workers.params.queue.at( workers.params.end ) = model.get();
            workers.params.end = (workers.params.end + 1) % workers.params.queue.size();
            workers.params.filled.release();
        } else
            model->update();
    }
}

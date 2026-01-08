#include "model_manager.hpp"
#include "../utils/log.hpp"

#include <ranges>


ModelManager::ModelManager()
    : m_models {}, m_threads {} {
    for ( unsigned int i { 0 }; i < s_number_threads; ++i )
        m_threads.at( i ) = std::thread { worker_thread, std::ref( m_worker_data ) };
}

ModelManager::~ModelManager() {
    // Signal the worker threads that they can stop executing
    m_worker_data.finish = true;
    m_worker_data.filled.release( s_number_threads );
    for ( unsigned int i { 0 }; i < s_number_threads; ++i )
        m_threads.at( i ).join();
}

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

void ModelManager::update_models() {
    for ( auto & model : std::ranges::views::values(m_models) ) {
        m_worker_data.empty.acquire();
        std::lock_guard queue_lock { m_worker_data.queue_mutex };
        m_worker_data.queue.at( m_worker_data.end ) = model.get();
        m_worker_data.end = (m_worker_data.end + 1) % s_buffer_size;
        m_worker_data.filled.release();
    }
}

void worker_thread( ModelManager::WorkerData & data ) {
    data.filled.acquire();
    do {
        ModelObject * model;
        {
            std::lock_guard queue_lock { data.queue_mutex }; // Lock the queue to extract the next model safely
            model = data.queue.at( data.begin );
            data.begin = (data.begin + 1) % ModelManager::s_buffer_size;
            data.empty.release();
        }
        model->update();

        data.filled.acquire(); // Wait until there's something new in the queue for the next iteration
    } while ( not data.finish );
}

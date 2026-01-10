#include "model_manager.hpp"
#include "../utils/log.hpp"

#include <mutex>
#include <ranges>
#include <semaphore>
#include <thread>


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

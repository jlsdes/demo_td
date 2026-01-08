#ifndef DEMO_TD_MODEL_MANAGER_HPP
#define DEMO_TD_MODEL_MANAGER_HPP

#include "model_object.hpp"

#include <array>
#include <memory>
#include <mutex>
#include <semaphore>
#include <thread>
#include <vector>


/** Manages the model objects, and some worker threads. */
class ModelManager {
public:
    ModelManager();
    ~ModelManager();

    /** Adds the ModelObject to the ModelManager and returns the object's new ID, which can be used to retrieve it or
     *  remove it from the manager. */
    unsigned int add_model( std::unique_ptr<ModelObject> && model );
    /** Returns a non-owning pointer to the model object based on its ID, as returned by add_model(). If the given ID
     *  can't be found, then a nullptr is returned. */
    [[nodiscard]] ModelObject * get_model( unsigned int model_id ) const;
    /** Removes the ModelObject from the ModelManager, and returns whether this was successful. */
    bool remove_model( unsigned int model_id );

    void update_models();

private:
    /// All registered models and their IDs; these are stored in ascending order of their IDs.
    std::vector<std::pair<unsigned int, std::unique_ptr<ModelObject>>> m_models;

    /// The buffer size used for the model queue, which delivers models to update to the worker threads.
    static unsigned int constexpr s_buffer_size { 16 };
    static unsigned int constexpr s_number_threads { 4 };

    /// The worker threads used for updating models.
    std::array<std::thread, 4> m_threads;

    /// The data passed to the worker threads, and the communication utilities.
    struct WorkerData {
        std::array<ModelObject *, s_buffer_size> queue {};
        unsigned int begin { 0 };
        unsigned int end { 0 };
        bool finish { false };

        std::counting_semaphore<s_buffer_size> filled { 0 };
        std::counting_semaphore<s_buffer_size> empty { s_buffer_size };
        std::mutex queue_mutex {};
    } m_worker_data {};

    /** The code that each of the worker threads will run. */
    friend void worker_thread( WorkerData & data );
};

void worker_thread( ModelManager::WorkerData & data );


#endif //DEMO_TD_MODEL_MANAGER_HPP

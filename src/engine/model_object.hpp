#ifndef DEMO_TD_MODEL_OBJECT_HPP
#define DEMO_TD_MODEL_OBJECT_HPP


#include "log.hpp"

#include <glm/glm.hpp>

#include <chrono>
#include <memory>
#include <mutex>


/** Base struct for the in-game model data. */
struct ModelData {
    glm::vec3 position;
};

/// A concept requiring a typename to be a subclass of ModelData.
template <typename Data>
concept DataType = std::is_base_of_v<ModelData, Data>;

/** Holds the model's data, while preventing any updates to it. This object should  */
template <DataType Data>
struct Accessor {
    Data const * data;
    std::unique_lock<std::mutex> lock;

    Accessor( Data const * data, std::mutex & mutex );

    Data const & operator*();
};


/** Base class for the in-game interaction models.
 *
 * Model objects have two "twins" containing the model's data. One is active and contains the model's current data, and
 * the other is used to compute the next iteration of the model in. At the end of each game tick, the active twin is
 * deactivated and the inactive twin, which should then contain the next iteration, is activated. */
template <DataType Data>
class ModelObject {
public:
    /** Constructor and destructor. */
    explicit ModelObject( glm::vec3 const & position = {} );
    virtual ~ModelObject();

    /** Returns the currently active data twin. */
    Data const * get_data() const;

    /** Compute the next iteration of the model in the inactive twin, and then activate it. */
    virtual void compute_next();
    virtual void activate_next();

private:
    /// Two instances of the model's data; one for the actual current data, and one for computing the next iteration.
    std::unique_ptr<Data[2]> m_data_twin;
    /// Which of the two data twins is currently active.
    bool m_twin_toggle;
    /// A mutex governing access to the active twin. Locked during activate_next(), and while get_data()
    std::timed_mutex m_mutex;
};


// Template implementations

template <DataType Data>
Accessor<Data>::Accessor( Data const * data, std::mutex & mutex )
    : data { data }, lock { mutex } {}

template <DataType Data>
Data const & Accessor<Data>::operator*() {
    return *data;
}

template <DataType Data>
ModelObject<Data>::ModelObject( glm::vec3 const & position )
    : m_data_twin { std::make_unique<Data[2]>( position ) }, m_twin_toggle { false } {}

template <DataType Data>
ModelObject<Data>::~ModelObject() = default;

template <DataType Data>
Data const * ModelObject<Data>::get_data() const {
    return Accessor<Data> { m_data_twin[m_twin_toggle].get() };
}

template <DataType Data>
void ModelObject<Data>::compute_next() {}

template <DataType Data>
void ModelObject<Data>::activate_next() {
    auto constexpr timeout { std::chrono::milliseconds { 10 } };
    std::unique_lock const lock { m_mutex, timeout };
    if ( !lock )
        Log::error( "Failed to acquire mutex, this model object's data is being read for >10ms." );
    else
        m_twin_toggle = !m_twin_toggle;
}

#endif //DEMO_TD_MODEL_OBJECT_HPP

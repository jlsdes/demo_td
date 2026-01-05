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

/** Holds the model's data, while preventing any updates to it. This object should be destroyed as soon as possible. */
template <DataType Data>
struct Accessor {
    Data const * data;
    std::unique_lock<std::mutex> lock;

    Accessor( Data const * data, std::mutex & mutex );

    Data const & operator*();
};


/** Base class for the in-game interaction models. */
template <DataType Data>
class ModelObject {
public:
    /** Constructor and destructor. */
    explicit ModelObject( glm::vec3 const & position );
    virtual ~ModelObject();

    /** Returns the model's data. The returned accessor blocks any updates to the data, so it should be destroyed as
     *  quickly as possible ideally. */
    Accessor<Data> get_data() const;

    /** Compute the next iteration of the model in the inactive twin, and then activate it. */
    virtual void compute_next();
    virtual void activate_next();

private:
    /** Constructor used exclusively to construct a twin model. */
    ModelObject();

    std::unique_ptr<ModelObject<Data>> m_twin;
    std::unique_ptr<Data> m_data;
    /// A mutex governing access to the active twin. Locked during activate_next(), and while the returned Accessor from
    /// get_data() exists.
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
    : m_twin { std::unique_ptr<ModelObject<Data>> { new ModelObject<Data>() } },
      m_data { std::make_unique<Data>( position ) }, m_mutex {} {}

template <DataType Data>
ModelObject<Data>::~ModelObject() = default;

template <DataType Data>
Accessor<Data> ModelObject<Data>::get_data() const {
    return Accessor<Data> { m_data.get(), m_mutex };
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
        std::swap( m_data, m_twin->m_data );
}

template <DataType Data>
ModelObject<Data>::ModelObject()
    : m_twin { nullptr }, m_data { std::make_unique<Data>() }, m_mutex {} {}

#endif //DEMO_TD_MODEL_OBJECT_HPP

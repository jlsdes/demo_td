#ifndef DEMO_TD_MODEL_OBJECT_HPP
#define DEMO_TD_MODEL_OBJECT_HPP

#include "utils/manager.hpp"

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
    std::unique_lock<std::timed_mutex> lock;

    Accessor( Data const * data, std::timed_mutex & mutex );

    Data const & operator*();
};


/** Base class for the in-game interaction models. */
class ModelObject : public ManagedObject {
    /** Base constructor. */
    explicit ModelObject( std::unique_ptr<ModelData[]> && data );
public:
    /** Other constructors and destructor. */
    explicit ModelObject( glm::vec3 const & position );
    ModelObject( ModelObject const & other );
    ModelObject & operator=( ModelObject const & other ) = delete;
    ModelObject( ModelObject && other ) noexcept;
    ModelObject & operator=( ModelObject && other ) = delete;
    ~ModelObject() override = default;

protected:
    /** Constructor for subclasses that uses a subclass of ModelData as its data type. */
    template <DataType Data, typename... Args>
    explicit ModelObject( Args const &... args );

public:
    /** Returns the model's data. This function will return an Accessor object that locks the object's mutex until
     *  it's destroyed, and should thus not be held for long. This is to ensure that next() can't suddenly change the
     *  object's data while it's being rendered. */
    template <DataType Data>
    Accessor<Data> get_render_data();
    /** Returns the model's data. This function simply returns a pointer to the data, and does not block the object's
     *  mutex in any way. Any rendering code should not call this function, but use get_render_data() instead. */
    template <DataType Data>
    Data const * get_model_data() const;

    /** Computes the next iteration of the model in the inactive twin, and then activates it. */
    void update() override;
    virtual void next();

protected:
    /// Two data structs; one for holding the current iteration's data, and one for building the next iteration's data
    /// in. 'm_active' holds the index of the current iteration's data.
    std::unique_ptr<ModelData[]> m_data;
    unsigned int m_active;
    /// A mutex governing access to the active twin. Locked during activate_next(), and while the returned Accessor from
    /// get_data() exists.
    std::timed_mutex m_mutex;
    /// A unique ID, which should be consistent between identical runs of the program.
    unsigned int const m_id;
};


// Template definitions

template <DataType Data>
Accessor<Data>::Accessor( Data const * data, std::timed_mutex & mutex )
    : data { data }, lock { mutex } {}

template <DataType Data>
Data const & Accessor<Data>::operator*() {
    return *data;
}

template <DataType Data, typename... Args>
ModelObject::ModelObject( Args const &... args )
    : ModelObject { std::make_unique_for_overwrite<Data[]>( 2 ) } {
    m_data[0] = Data { args... };
}

template <DataType Data>
Accessor<Data> ModelObject::get_render_data() {
    return { &m_data[m_active], m_mutex };
}

template <DataType Data>
Data const * ModelObject::get_model_data() const {
    return &m_data[m_active];
}

#endif //DEMO_TD_MODEL_OBJECT_HPP

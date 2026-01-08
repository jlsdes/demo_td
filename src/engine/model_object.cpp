#include "model_object.hpp"
#include "log.hpp"


/** Generates a new unique ID. These IDs are simply sequentially ascending from 0. */
unsigned int generate_id() {
    static unsigned int id { 0 };
    return id++;
}

ModelObject::ModelObject( std::unique_ptr<ModelData[]> && data )
    : m_data { std::move( data ) }, m_active { 0 }, m_mutex {}, m_id { generate_id() } {}

ModelObject::ModelObject( glm::vec3 const & position )
    : ModelObject { std::make_unique_for_overwrite<ModelData[]>( 2 ) } {
    m_data[0] = { position };
}

ModelObject::ModelObject( ModelObject const & other )
    : m_data { std::make_unique_for_overwrite<ModelData[]>( 2 ) }, m_active { other.m_active }, m_mutex {},
      m_id { generate_id() } {
    m_data[0] = other.m_data[0];
    m_data[1] = other.m_data[1];
}

ModelObject::ModelObject( ModelObject && other ) noexcept
    : m_data { std::move( other.m_data ) }, m_active { other.m_active }, m_mutex {}, m_id { other.m_id } {}

void ModelObject::update() {
    m_data[1 - m_active] = m_data[m_active];
}

void ModelObject::next() {
    std::chrono::milliseconds constexpr timeout { 10 };
    std::unique_lock const lock { m_mutex, timeout };
    if ( !lock )
        Log::error( "Failed to acquire ModelObject's mutex within ", timeout, "; forcing update anyway." );
    m_active = 1 - m_active;
}

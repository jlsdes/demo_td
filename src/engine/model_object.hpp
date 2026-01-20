#ifndef DEMO_TD_MODEL_OBJECT_HPP
#define DEMO_TD_MODEL_OBJECT_HPP

#include "utils/manager.hpp"

#include <array>
#include <chrono>
#include <memory>
#include <mutex>


class ViewObject;
class ControllerObject;
class EntityFactory;


/** Base struct used for the model's data. */
struct ModelData {
    virtual ~ModelData() = default;
};

/** Requires a struct to be derived from ModelObject::ModelData. */
template <typename Type>
concept DataType = requires( Type ) { std::is_base_of_v<ModelData, Type>; };


/** Base class for all models; all models that also contain some data, i.e. almost all of them, should derive from
 *  DataModel<Data> instead. */
class ModelObject : public ManagedObject {
public:
    ~ModelObject() override;

    /** Sets the model's view if there isn't one already; if there is, a warning is logged and nothing happens. */
    void set_view( ViewObject * view );
    /** Sets the model's controller if there isn't one already; if there is, a warning is logged and nothing happens. */
    void set_controller( ControllerObject * controller );

    /** To be overridden by subclasses; doesn't actually update anything in this class. */
    void update() override {}

    /** Moves from the old model state to the newly computed state. */
    static void swap_model_state();
    /** Saves the last fully computed model state as the new render state. */
    static void set_render_state();

protected:
    /// Indices into the m_data arrays, synchronised across all ModelObject instances.
    static unsigned int s_render_data;
    static unsigned int s_old_iteration;
    static unsigned int s_new_iteration;
    static std::mutex m_mutex;

private:
    /// Pointers to the model's (optional) view and controller.
    ViewObject * m_view { nullptr };
    ControllerObject * m_controller { nullptr };
};


/** Base class for model objects with some data. In every iteration of the game loop, the old state of the model can be
 *  queried with get_old_data(), and the new state should be written to the result of get_new_data(). In every render
 *  loop iteration, the state to be rendered is returned by get_render_data(). */
template <DataType Data>
class DataModel : public ModelObject {
public:
    DataModel() = default;
    ~DataModel() override;

    /** Returns the data of the model that should be used for rendering it. This is not necessarily the current state of
     *  the model, but is consistent with all other models being drawn within the same render pass. */
    [[nodiscard]] Data const * get_render_data() const;
    /** Returns the model's current data. This is the computed state from the last game loop iteration while the
     *  controllers are being updated, but may or may not be updated already while the models are being updated. */
    [[nodiscard]] Data const * get_old_data() const;
    /** Returns the data struct where model's next state should end up. */
    [[nodiscard]] Data * get_new_data();

private:
    /// 3 instances of the model's data, to allow continuous updates in both the game and render loops.
    std::array<Data, 3> m_data;
};


/** Concept that requires types derived from the ModelObject class. */
template <typename Type>
concept ModelType = std::is_base_of_v<ModelObject, Type>;


// Template definitions

template <DataType Data>
DataModel<Data>::~DataModel() {
    ModelObject::~ModelObject();
}

template <DataType Data>
Data const * DataModel<Data>::get_render_data() const {
    return &m_data.at( s_render_data );
}

template <DataType Data>
Data const * DataModel<Data>::get_old_data() const {
    return &m_data.at( s_old_iteration );
}

template <DataType Data>
Data * DataModel<Data>::get_new_data() {
    return &m_data.at( s_new_iteration );
}


#endif //DEMO_TD_MODEL_OBJECT_HPP

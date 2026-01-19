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


/** Base class for all models. Any derived classes should also derive the ModelData struct to hold any data they want to
 *  be updated and used in its controller and/or view. */
class ModelObject : public ManagedObject {
public:
    ModelObject();
    ModelObject( ModelObject const & model ) = delete;
    ModelObject & operator=( ModelObject const & model ) = delete;
    ModelObject( ModelObject && model ) noexcept = default;
    ModelObject & operator=( ModelObject && model ) noexcept = default;
    ~ModelObject() override;

    /** Sets the model's view if there isn't one already; if there is, a warning is logged and nothing happens. */
    void set_view( ViewObject * view );
    /** Sets the model's controller if there isn't one already; if there is, a warning is logged and nothing happens. */
    void set_controller( ControllerObject * controller );

    /** Returns the data of the model that should be used for rendering it. This is not necessarily the current state of
     *  the model, but is consistent with all other models being drawn within the same render pass. */
    [[nodiscard]] ModelData const * get_render_data() const;
    /** Returns the model's current data. This is the computed state from the last game loop iteration while the
     *  controllers are being updated, but may or may not be updated already while the models are being updated. */
    [[nodiscard]] ModelData const * get_old_data() const;
    template <DataType Data>
    [[nodiscard]] Data const * get_old_data() const;
    /** Returns the location of the model's next state. */
    [[nodiscard]] ModelData * get_new_data() const;
    template <DataType Data>
    [[nodiscard]] Data * get_new_data() const;

    /** To be overridden by subclasses; doesn't actually update anything in this class. */
    void update() override;

    /** Moves from the old model state to the newly computed state. */
    static void swap_model_state();
    /** Saves the last fully computed model state as the new render state. */
    static void set_render_state();

protected:
    void initialise_data( std::array<ModelData *, 3> && data );

private:
    /// 3 pointers to instances of the model's data, which should be stored by the derived classes.
    std::array<ModelData *, 3> m_data;
    /// Indices into the m_data arrays, synchronised across all ModelObject instances.
    static unsigned int s_render_data;
    static unsigned int s_old_iteration;
    static unsigned int s_new_iteration;
    static std::mutex m_mutex;

    /// Pointers to the model's (optional) view and controller.
    ViewObject * m_view;
    ControllerObject * m_controller;
};

// Template definitions

template <DataType Data>
Data const * ModelObject::get_old_data() const {
    return dynamic_cast<Data const *>(get_old_data());
}

template <DataType Data>
Data * ModelObject::get_new_data() const {
    return dynamic_cast<Data *>(get_new_data());
}


#endif //DEMO_TD_MODEL_OBJECT_HPP

#ifndef DEMO_TD_ENTITY_HPP
#define DEMO_TD_ENTITY_HPP


/** Every entity is composed of a number of components, and is represented by a unique ID. */
using EntityID = unsigned int;


constexpr unsigned int g_max_entities = 1 << 15;


struct ECS;


/** Base class for entities. (May or may not be removed in the future, as I'm not sure whether I want to use this.)
 *  This class and derived classes create, own, and destroy their own components. This should generally happen in their
 *  constructors and destructors. */
class Entity {
public:
    /** The constructor creates a new entity within the provided ECS ecosystem. */
    explicit Entity( ECS * ecs );

    /** The destructor removes the entity and its components from the ECS again. Derived classes can (normally) just
     *  default override this, as the ECS will be handled in this destructor regardless of the component types. */
    virtual ~Entity();

    /** Entities cannot be copied using this class. */
    Entity( Entity const & ) = delete;
    Entity & operator=( Entity const & ) = delete;

    /** Entities can be moved using the move constructor. The move constructor will set a flag in the old object to
     *  prevent the entity being destroyed prematurely. */
    Entity( Entity && ) noexcept;
    Entity & operator=( Entity && ) noexcept = delete;

private:
    /// An indicator that can be set after a move constructor has been called with this entity as argument. This should
    /// prevent the destructor from removing anything from the ECS.
    bool m_moved { false };
protected:
    EntityID const m_id;
    /// The ECS that this entity is part of.
    ECS * const m_ecs;
};


#endif //DEMO_TD_ENTITY_HPP

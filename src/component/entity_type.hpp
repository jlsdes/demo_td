#ifndef DEMO_TD_ENTITY_TYPE_HPP
#define DEMO_TD_ENTITY_TYPE_HPP

#include "component.hpp"


/** A simple component holding only an entity's type. */
struct EntityType : Component {
    enum TypeID : unsigned char {
        Tile,
        Enemy,
        Tower,
        Projectile,
        Ui,
        Skybox,
        Other,
        NrTypes, ///< Not a valid type; must remain last in the enum.
    } type_id;
};


#endif //DEMO_TD_ENTITY_TYPE_HPP
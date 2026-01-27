#ifndef DEMO_TD_TOWER_DATA_HPP
#define DEMO_TD_TOWER_DATA_HPP

#include "component.hpp"


struct TowerData : Component {

    enum Type : unsigned char {
        Basic,
        Mortar,
        Sniper,
        Multi,
        Melee,
        Status,
        NumberTypes // Valid tower type values go from 0 to NumberTypes
    };

    Type type;
};


#endif //DEMO_TD_TOWER_DATA_HPP
#ifndef DEMO_TD_TOWER_HPP
#define DEMO_TD_TOWER_HPP

#include "entity.hpp"
#include "component/tower_data.hpp"

#include <glm/glm.hpp>


struct ECS;


class TowerFactory {
public:

    explicit TowerFactory( ECS * ecs );

    [[nodiscard]] EntityID build( TowerData::Type type, glm::vec3 const & position ) const;

private:
    ECS * const m_ecs;
};


#endif //DEMO_TD_TOWER_HPP
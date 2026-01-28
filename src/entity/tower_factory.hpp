#ifndef DEMO_TD_TOWER_HPP
#define DEMO_TD_TOWER_HPP

#include "entity.hpp"
#include "component/tower_data.hpp"

#include <array>

#include <glm/glm.hpp>


struct ECS;


class TowerFactory {
public:

    static constexpr std::array<glm::vec3, TowerData::NumberTypes> s_colours {
        glm::vec3 { 0.f, 0.f, 1.f },
        glm::vec3 { 0.f, 1.f, 0.f },
        glm::vec3 { 1.f, 0.f, 0.f },
        glm::vec3 { 1.f, 1.f, 0.f },
        glm::vec3 { 1.f, 0.f, 1.f },
        glm::vec3 { 0.f, 1.f, 1.f },
    };

    explicit TowerFactory( ECS * ecs );

    [[nodiscard]] EntityID build( TowerData::Type type, glm::vec3 const & position ) const;

private:
    ECS * const m_ecs;
};


#endif //DEMO_TD_TOWER_HPP
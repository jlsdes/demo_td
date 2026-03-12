#ifndef DEMO_TD_TILE_HIGHLIGHT_HPP
#define DEMO_TD_TILE_HIGHLIGHT_HPP

#include "entity.hpp"

#include <glm/glm.hpp>


struct Context;


class TileHighlight : public Entity {
public:
    explicit TileHighlight( Context const & context );
    ~TileHighlight() override = default;

    void show() const;
    void hide() const;

    void set_position( glm::vec3 const & position ) const;

private:
};


#endif //DEMO_TD_TILE_HIGHLIGHT_HPP

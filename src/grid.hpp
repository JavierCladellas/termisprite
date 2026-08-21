#pragma once

#include <ftxui/dom/elements.hpp>


namespace Termisprite
{


enum class GridType
{
    LINES,
    POINTS,
    CHECKERBOARD
};

class Grid
{
public:
    Grid() = default;

    void render( std::vector<ftxui::Elements> & cells, bool isPixelSquare = true ) const;

    void switchType() { M_gridType = (GridType)(((int)M_gridType + 1) % 3); }
    GridType type() const { return M_gridType; }

    void toggle() { M_isVisible = !M_isVisible; }
    bool isVisible() const { return M_isVisible; }

private:
    GridType M_gridType = GridType::LINES;
    bool M_isVisible = true;
};

}

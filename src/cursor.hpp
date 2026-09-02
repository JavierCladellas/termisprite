#pragma once

#include <vector>

#include <ftxui/component/event.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/dom/elements.hpp>

#include "camera.hpp"

namespace Termisprite
{

class BrushTool;
class CanvasCursor
{
public:
    CanvasCursor( int x = 0, int y = 0 )
        : M_x(x), M_y(y)
    {}

    int & x() { return M_x; }
    int & y() { return M_y; }

    bool isVisible() const { return M_isVisible; }
    void setVisibility( bool isVisible ){ M_isVisible = isVisible; }

    void render( std::vector<ftxui::Elements> & cells, BrushTool const& brushTool, bool isSquarePixel );

    bool processMovement( ftxui::Event event, int maxWidth, int maxHeight );

private:
    int M_x, M_y;
    bool M_isVisible = true;

    ftxui::Color M_bgColor = ftxui::Color::Red;
};

}

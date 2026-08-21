#pragma once

#include <ftxui/screen/color.hpp>
#include <vector>

#include <ftxui/dom/elements.hpp>

namespace Termisprite
{

class EditorState;
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

    void render( std::vector<ftxui::Elements> & cells, EditorState const& editorState, bool isSquarePixel );

private:
    int M_x, M_y;
    bool M_isVisible = true;

    ftxui::Color M_bgColor = ftxui::Color::Red;
};

}

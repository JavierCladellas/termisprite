#include "cursor.hpp"
#include "brush_tool.hpp"

namespace Termisprite
{

void
CanvasCursor::render( std::vector<ftxui::Elements> & cells, BrushTool const& brushTool, bool isSquarePixel )
{
    if ( !M_isVisible ) return;

    int height = cells.size();
    int width = height > 0 ? cells[0].size() : 0;

    std::string brush = brushTool.currentChar();

    int scale = isSquarePixel ? 2 : 1;

    for ( int logicalY = M_y; logicalY < M_y + brushTool.size(); ++logicalY )
    {
        if ( logicalY < 0 || logicalY >= height ) continue;

        for ( int logicalX = M_x; logicalX < M_x + brushTool.size(); ++logicalX )
        {
            int terminalXStart = logicalX * scale;

            for ( int i = 0; i < scale; ++i )
            {
                int terminalX = terminalXStart + i;

                if ( terminalX < 0 || terminalX >= width ) continue;

                cells[logicalY][terminalX] = ftxui::text( brush )
                                             | ftxui::color( brushTool.activeColor() )
                                             | ftxui::bgcolor( M_bgColor )
                                             | ftxui::blink;
            }
        }
    }
}

bool
CanvasCursor::processMovement( ftxui::Event event, int maxWidth, int maxHeight )
{

    int dx = 0, dy = 0;

    if ( event == ftxui::Event::ArrowUp || event == ftxui::Event::Character( 'k' ) ) dy = -1;
    else if ( event == ftxui::Event::ArrowDown || event == ftxui::Event::Character( 'j' ) ) dy = 1;
    else if ( event == ftxui::Event::ArrowLeft || event == ftxui::Event::Character( 'h' ) ) dx = -1;
    else if ( event == ftxui::Event::ArrowRight || event == ftxui::Event::Character( 'l' ) ) dx = 1;

    if ( dx != 0 || dy != 0 )
    {
        // TODO CLAMP WITH EDITOR SIZE
        M_x = std::clamp( M_x + dx, 0, maxWidth - 1);
        M_y = std::clamp( M_y + dy, 0, maxHeight - 1);
        setVisibility(true);
        return true;
    }
    return false;

}


}

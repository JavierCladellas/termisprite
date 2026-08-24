#include "cursor.hpp"
#include "brush_tool.hpp"

namespace Termisprite
{

void
CanvasCursor::render( std::vector<ftxui::Elements> & cells, BrushTool const& brushTool, bool isSquarePixel )
{
    if ( !M_isVisible ) return;

    int height = cells.size();
    int width = height > 0 ? cells[0].size() : 0; //Assumes rectangular canvas

    std::string brush = brushTool.currentChar();
    if ( isSquarePixel )
        brush += brush;

    for ( int y = M_y; y < M_y + brushTool.size(); ++y )
    {
        if ( y < 0 || y >= height ) break;
        for ( int x = M_x; x < M_x + brushTool.size(); ++x )
        {
            if ( x< 0 || x >= width ) continue;

            cells[y][x] = ftxui::text( brush )
                            | ftxui::color( brushTool.activeColor() )
                            | ftxui::bgcolor( M_bgColor )
                            | ftxui::blink;

        }
    }

}

bool
CanvasCursor::processMovement( ftxui::Event event, int canvasHeight, int canvasWidth )
{

    int dx = 0, dy = 0;

    if ( event == ftxui::Event::ArrowUp || event == ftxui::Event::Character( 'k' ) ) dy = -1;
    else if ( event == ftxui::Event::ArrowDown || event == ftxui::Event::Character( 'j' ) ) dy = 1;
    else if ( event == ftxui::Event::ArrowLeft || event == ftxui::Event::Character( 'h' ) ) dx = -1;
    else if ( event == ftxui::Event::ArrowRight || event == ftxui::Event::Character( 'l' ) ) dx = 1;

    if ( dx != 0 || dy != 0 )
    {
        M_x = std::clamp( M_x + dx, 0, canvasWidth - 1 );
        M_y = std::clamp( M_y + dy, 0, canvasHeight - 1 );
        setVisibility(true);
        return true;
    }
    return false;

}


}

#include "cursor.hpp"

#include "editor.hpp"

namespace Termisprite
{

void
CanvasCursor::render( std::vector<ftxui::Elements> & cells, EditorState const& editorState, bool isSquarePixel )
{
    if ( !M_isVisible ) return;

    int height = cells.size();
    int width = height > 0 ? cells[0].size() : 0; //Assumes rectangular canvas

    std::string brush = editorState.brush;
    if ( isSquarePixel )
        brush += brush;

    for ( int y = M_y; y < M_y + editorState.brushSize; ++y )
    {
        if ( y < 0 || y >= height ) break;
        for ( int x = M_x; x < M_x + editorState.brushSize; ++x )
        {
            if ( x< 0 || x >= width ) continue;

            cells[y][x] = ftxui::text( brush )
                            | ftxui::color( editorState.color )
                            | ftxui::bgcolor( M_bgColor )
                            | ftxui::blink;

        }
    }

}


}

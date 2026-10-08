#include "sprite.hpp"

namespace Termisprite
{

void
Layer::resize( int width, int height )
{
    M_width = width;
    M_height = height;
    M_grid.resize( M_height );

    for ( auto & row : M_grid )
        row.resize( M_width );
}




void
Layer::render( std::vector<std::vector<ftxui::Element>> & cells, bool isSquarePixel ) const
{
    if (!M_isVisible) return;

    int canvasHeight = cells.size();
    if ( canvasHeight == 0 ) return;
    int canvasWidth = cells[0].size();

    int scale = isSquarePixel ? 2 : 1;

    int startY = std::max(0, M_camera.posY() - M_y);
    int endY = std::min(M_height, M_camera.posY() + canvasHeight - M_y);

    int startX = std::max(0, M_camera.posX() - M_x);
    int endX = std::min(M_width, M_camera.posX() + (canvasWidth / scale) - M_x);

    for ( int logicalY = startY; logicalY < endY; ++logicalY )
    {
        int screenY = logicalY + M_y - M_camera.posY();
        if ( screenY < 0 || screenY >= canvasHeight ) continue;

        for ( int logicalX = startX; logicalX < endX; ++logicalX )
        {
            Pixel const& cellContent = at(logicalX, logicalY);

            std::string brush = cellContent.brush;
            if ( cellContent.brush == " " )
                continue;

            int screenXLogical = logicalX + M_x - M_camera.posX();
            int terminalXStart = screenXLogical * scale;

            for ( int i = 0; i < scale; ++i )
            {
                int terminalX = terminalXStart + i;
                if ( terminalX < 0 || terminalX >= canvasWidth ) continue;
                cells[screenY][terminalX] = ftxui::text(brush) | ftxui::color( cellContent.color );
            }
        }
    }

}


}


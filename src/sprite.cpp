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

    for ( int logicalY = 0; logicalY < M_height; ++logicalY )
    {
        if ( logicalY + M_y >= canvasHeight ) break;
        for ( int logicalX = 0; logicalX < M_width; ++logicalX )
        {
            Pixel const& cellContent = at(logicalX, logicalY);

            std::string brush = cellContent.brush;
            if ( cellContent.brush == " " )
                continue;

            int terminalXStart = logicalX * scale;
            for ( int i = 0; i < scale; ++i )
            {
                int terminalX = terminalXStart + i;
                if ( terminalX + M_x >= canvasWidth  ) break;
                cells[logicalY + M_y][terminalX + M_x] = ftxui::text(brush) | ftxui::color( cellContent.color );
            }
        }
    }

}




void
SpriteHistory::save( Layer const& layer )
{
    if ( M_currentIndex < static_cast<int>(M_history.size()) - 1 )
        M_history.erase( M_history.begin() + M_currentIndex + 1, M_history.end() );

    M_history.push_back( layer );

    if ( M_history.size() > M_maxSize )
        M_history.erase( M_history.begin() );
    else
        M_currentIndex++;
}

void
SpriteHistory::undo( Layer & layer )
{
    if ( M_currentIndex > 0 )
    {
        M_currentIndex--;
        layer = M_history[M_currentIndex];
    }
}

void
SpriteHistory::redo( Layer & layer )
{
    if ( M_currentIndex < static_cast<int>(M_history.size()) - 1 )
    {
        M_currentIndex++;
        layer = M_history[M_currentIndex];
    }
}

}


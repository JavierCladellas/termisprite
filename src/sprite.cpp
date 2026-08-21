#include "sprite.hpp"

namespace Termisprite
{

void
Sprite::resize( int width, int height )
{
    M_width = width;
    M_height = height;
    M_grid.resize( M_height );

    for ( auto & row : M_grid )
        row.resize( M_width );
}




void
Sprite::render( std::vector<std::vector<ftxui::Element>> & cells, bool isSquarePixel ) const
{

    for ( int y = 0; y < M_height; ++y )
    {
        for ( int x = 0; x < M_width; ++x )
        {
            Pixel const& cellContent = at(x, y);

            std::string brush = cellContent.brush;
            if ( cellContent.brush == " " )
                continue;

            if ( isSquarePixel )
                brush += brush;

            ftxui::Element cell =  ftxui::text(brush);
            cell |= ftxui::color( cellContent.color );
            cells[y][x] = cell;
        }
    }

}




void
SpriteHistory::save( Sprite const& sprite )
{
    if ( M_currentIndex < static_cast<int>(M_history.size()) - 1 )
        M_history.erase( M_history.begin() + M_currentIndex + 1, M_history.end() );

    M_history.push_back( sprite );

    if ( M_history.size() > M_maxSize )
        M_history.erase( M_history.begin() );
    else
        M_currentIndex++;
}

void
SpriteHistory::undo( Sprite & sprite )
{
    if ( M_currentIndex > 0 )
    {
        M_currentIndex--;
        sprite = M_history[M_currentIndex];
    }
}

void
SpriteHistory::redo( Sprite & sprite )
{
    if ( M_currentIndex < static_cast<int>(M_history.size()) - 1 )
    {
        M_currentIndex++;
        sprite = M_history[M_currentIndex];
    }
}

}


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
    {
        row.resize( M_width );
    }
}

}


#include "grid.hpp" 


namespace Termisprite
{




void
Grid::render( std::vector<ftxui::Elements> & canvasGrid, bool isPixelSquare ) const
{
    
    int height = canvasGrid.size();
    for ( int y = 0; y < height; ++y )
    {
        ftxui::Elements & row = canvasGrid[y];
        int width = row.size(); // For potential support to non rectangular grids?

        for ( int x = 0; x < width; ++x )
        {
            auto & cell = row[x];
            cell = isPixelSquare ?ftxui::text("  ") : ftxui::text(" "); 

            if ( !M_isVisible )
                continue;
               
            switch( M_gridType )
            {
                case GridType::POINTS:
                    if ( x %  2 != 0 ) continue;

                    cell = isPixelSquare ? ftxui::text(". ") : ftxui::text(".");
                    cell |= ftxui::color( ftxui::Color::GrayDark );
                    break;
                case GridType::CHECKERBOARD:
                    cell = isPixelSquare ? ftxui::text("██") : ftxui::text("█");

                    if ( ( x + y ) % 2 == 0 )
                         cell |= ftxui::color( ftxui::Color::GrayDark );
                    else
                         cell |= ftxui::color( ftxui::Color::Black );

                    break;
                case GridType::LINES:

                    if ( y != 0 && y % 8 == 0 )
                        cell = isPixelSquare ? ftxui::text("──") : ftxui::text("─");
                    if ( x != 0 && x % 8 == 0 )
                        cell = isPixelSquare ?  ftxui::text("│ ") : ftxui::text("│");
                    if (  y != 0 && y % 8 == 0 &&  x != 0 && x % 8 == 0  ) //Intersections
                        cell = isPixelSquare ? ftxui::text("┼─") : ftxui::text("┼");

                    cell |= ftxui::color( ftxui::Color::GrayDark );
                    break;
            }
            

        }


    }

}

}

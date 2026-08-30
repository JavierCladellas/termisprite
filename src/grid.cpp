#include "grid.hpp"


namespace Termisprite
{




void
Grid::render( std::vector<ftxui::Elements> & cells, bool isPixelSquare ) const
{
    if ( !M_isVisible )
        return;

    int height = cells.size();
    for ( int y = 0; y < height; ++y )
    {
        ftxui::Elements & row = cells[y];
        int width = row.size(); // For potential support to non rectangular grids?

        for ( int x = 0; x < width; ++x )
        {
            auto & cell = row[x];
            cell = ftxui::text(" ");
            int logicalX = isPixelSquare ? (x / 2) : x;
            bool isLeftHalf = isPixelSquare ? (x % 2 == 0) : true;

            switch( M_gridType )
            {
                case GridType::POINTS:
                    if ( logicalX %  2 != 0 ) continue;

                    if ( isLeftHalf )
                        cell = ftxui::text(".");
                    cell |= ftxui::color( ftxui::Color::GrayDark );
                    break;
                case GridType::CHECKERBOARD:
                    cell = ftxui::text("█");

                    if ( ( logicalX + y ) % 2 == 0 )
                         cell |= ftxui::color( ftxui::Color::GrayDark );
                    else
                         cell |= ftxui::color( ftxui::Color::Black );

                    break;
                case GridType::LINES:

                    if ( y != 0 && y % 8 == 0 )
                        cell = ftxui::text("─");
                    if ( logicalX != 0 && logicalX % 8 == 0 )
                        cell = isLeftHalf ? ftxui::text("│") : ftxui::text(" ");
                    if (  y != 0 && y % 8 == 0 &&  logicalX != 0 && logicalX % 8 == 0  ) //Intersections
                        cell = isLeftHalf ? ftxui::text("┼") : ftxui::text("─");

                    cell |= ftxui::color( ftxui::Color::GrayDark );
                    break;
            }


        }


    }

}

}

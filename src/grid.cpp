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

        int worldY = y + M_camera.posY();

        for ( int x = 0; x < width; ++x )
        {
            auto & cell = row[x];
            cell = ftxui::text(" ");
            int logicalX = isPixelSquare ? (x / 2) : x;
            bool isLeftHalf = isPixelSquare ? (x % 2 == 0) : true;

            int worldX = logicalX + M_camera.posX();

            switch( M_gridType )
            {
                case GridType::POINTS:
                    if ( worldX %  2 != 0 ) continue;

                    if ( isLeftHalf )
                        cell = ftxui::text(".");
                    cell |= ftxui::color( ftxui::Color::GrayDark );
                    break;
                case GridType::CHECKERBOARD:
                    cell = ftxui::text("█");

                    if ( ( worldX + worldY ) % 2 == 0 )
                         cell |= ftxui::color( ftxui::Color::GrayDark );
                    else
                         cell |= ftxui::color( ftxui::Color::Black );

                    break;
                case GridType::LINES:

                    if ( worldY != 0 && worldY % 8 == 0 )
                        cell = ftxui::text("─");
                    if ( worldX != 0 && worldX % 8 == 0 )
                        cell = isLeftHalf ? ftxui::text("│") : ftxui::text(" ");
                    if (  worldY != 0 && worldY % 8 == 0 &&  worldX != 0 && worldX % 8 == 0  ) //Intersections
                        cell = isLeftHalf ? ftxui::text("┼") : ftxui::text("─");

                    cell |= ftxui::color( ftxui::Color::GrayDark );
                    break;
            }


        }


    }

}

}

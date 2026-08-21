#include "selection_tool.hpp"

namespace Termisprite
{


void 
SelectionTool::render( std::vector<ftxui::Elements> & cells, bool isSquarePixel )
{

    if ( !M_isActive ) return;

    std::string brushL, brushR;
    std::string brush;
    for ( int y = minY(); y < minY() + height(); ++y )
    {
        bool isTop = y == minY();
        bool isBot = y == maxY();

        for ( int x = minX(); x < minX() + width(); ++x )
        {
            bool isLeft = x == minX();
            bool isRight = x == maxX();

            if (isTop && isLeft && isBot && isRight) { brushL = "⡏"; brushR = "⢹"; } 
            else if (isTop && isLeft) { brushL = "⡏"; brushR = "⠉"; }
            else if (isTop && isRight) { brushL = "⠉"; brushR = "⢹"; }
            else if (isBot && isLeft) { brushL = "⣇"; brushR = "⣀"; }
            else if (isBot && isRight) { brushL = "⣀"; brushR = "⣸"; }
            else if (isTop) { brushL = "⠉"; brushR = "⠉"; }
            else if (isBot) { brushL = "⣀"; brushR = "⣀"; }
            else if (isLeft) { brushL = "⡇"; brushR = " "; }
            else if (isRight) { brushL = " "; brushR = "⢸"; }

            brush = brushL;
            if ( isSquarePixel )
                brush += brushR;


            if ( isTop || isBot || isLeft || isRight )
                cells[y][x] = ftxui::text(brush) | ftxui::color( M_color );
        }
    }




}


}

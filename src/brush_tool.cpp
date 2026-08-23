#include "brush_tool.hpp"


namespace Termisprite
{

void 
BrushTool::apply( Sprite & sprite, int targetX, int targetY, bool isEraser )
{
    int width, height;
    std::tie( width, height ) = sprite.size();

    for ( int by = 0; by < M_size; ++by )
    {
        for ( int bx = 0; bx < M_size; ++bx )
        {
            int px = targetX + bx;
            int py = targetY + by;

            if ( px >= 0 && px < width && py >= 0 && py < height )
            {
                auto & cell = sprite.at(px, py);
                if ( isEraser )
                {
                    cell.brush = " ";
                    cell.color = ftxui::Color::White;
                }
                else
                {
                    cell.brush = M_currentChar;
                    cell.color = M_color; 
                }
            }
        }
    }


}



}

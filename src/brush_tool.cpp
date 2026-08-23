#include "brush_tool.hpp"


namespace Termisprite
{

void 
BrushTool::apply( Sprite & sprite, int targetX, int targetY )
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
                auto & pixel = sprite.at(px, py);
                apply( pixel );
            }
        }
    }


}

bool
BrushTool::processKeyboardEvent( Sprite & sprite, CanvasCursor & cursor, ftxui::Event event )
{

    if ( event == ftxui::Event::Character( ' ' ) || event == ftxui::Event::Return )
    {
        apply( sprite, cursor.x(), cursor.y() );
        cursor.setVisibility(true);
        return true;
    }
    if ( M_currentChar != " " && ( event == ftxui::Event::Backspace || event == ftxui::Event::Delete ))
    {
        setCurrentBrush(" ");
        apply( sprite, cursor.x(), cursor.y());
        cursor.setVisibility(true);
        updateCurrentBrush();
        return true;
    }

    return false;
}




}

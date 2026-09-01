#include "geometry.hpp"
#include "brush_tool.hpp"


namespace Termisprite
{


void
BrushTool::apply()
{
    apply( M_cursor.x(), M_cursor.y() );
}

void
BrushTool::apply( int targetX, int targetY )
{
    if (!M_layer) return;

    auto [width,height] = M_layer->size();
    auto [layerX, layerY] = M_layer->position();

    int localTargetX = targetX - layerX;
    int localTargetY = targetY - layerY;

    for ( int by = 0; by < M_size; ++by )
    {
        for ( int bx = 0; bx < M_size; ++bx )
        {
            int px = localTargetX + bx;
            int py = localTargetY + by;

            if ( px >= 0 && px < width && py >= 0 && py < height )
            {
                auto & pixel = M_layer->at(px, py);
                apply( pixel );
            }
        }
    }


}

bool
BrushTool::processKeyboardEvent( ftxui::Event event )
{

    if ( event == ftxui::Event::Character( ' ' ) || event == ftxui::Event::Return )
    {
        apply();
        M_cursor.setVisibility(true);
        return true;
    }
    if ( M_currentChar != " " && ( event == ftxui::Event::Backspace || event == ftxui::Event::Delete ))
    {
        setCurrentBrush(" ");
        apply();
        M_cursor.setVisibility(true);
        updateCurrentBrush();
        return true;
    }

    return false;
}

bool
BrushTool::processMouseEvent( ftxui::Event event )
{
    if ( !event.is_mouse() )
        return false;

    auto mouse = event.mouse();

    if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Released )
    {
        if ( M_isDrawing )
        {
            M_isDrawing = false;
            return true;
        }
    }


    if ( mouse.button == ftxui::Mouse::Button::Left )
    {
        M_cursor.setVisibility(false);

        auto [worldX, worldY ] = M_screenToWorld(mouse.x, mouse.y);

        if ( worldX < 0 || worldY < 0 ) //screenToWorld returned invalid coordinates (out of screen box )
            return false;

        if ( mouse.motion == ftxui::Mouse::Pressed )
        {
            M_isDrawing = true;
            M_lastX = worldX;
            M_lastY = worldY;

            apply( worldX, worldY);
            return true;
        }
        else if ( (mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed) && M_isDrawing )
        {
            Geometry::drawLine( *this, *M_layer, M_lastX, M_lastY, worldX, worldY );
            M_lastX = worldX;
            M_lastY = worldY;
            return true;
        }
    }

    return false;

}



}

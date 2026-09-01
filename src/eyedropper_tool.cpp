#include "eyedropper_tool.hpp"
#include "brush_tool.hpp"


namespace Termisprite
{

bool EyeDropperTool::processKeyboardEvent( ftxui::Event event )
{

    if ( event != ftxui::Event::Character(' ') && event != ftxui::Event::Return )
        return false;

    int spriteW, spriteH;
    std::tie(spriteW, spriteH) = M_layer->size();
    if ( M_cursor.x() < 0 || M_cursor.x() >= spriteW || M_cursor.y() < 0 || M_cursor.y() >= spriteH )
        return false;

    Pixel & cell = M_layer->at(M_cursor.x(),M_cursor.y());

    M_brush.setCurrentBrush(cell.brush);
    M_brush.setColor(cell.color);

    return true;
}

bool EyeDropperTool::processMouseEvent( ftxui::Event event )
{
    if ( !event.is_mouse() )
        return false;

    auto mouse = event.mouse();


    auto [localX, localY] = M_screenToWorld(mouse.x, mouse.y);

    if ( localX < 0 || localY < 0 )
        return false;
    if ( mouse.button != ftxui::Mouse::Button::Left || mouse.motion != ftxui::Mouse::Pressed )
        return false;

    auto [spriteW,spriteH] = M_layer->size();
    auto [lX, lY] = M_layer->position();

    localX -= lX;
    localY -= lY;

    if ( localX >= spriteW || localY >= spriteH )
        return false;
    if ( localX < 0 || localY < 0 )
        return false;

    Pixel & cell = M_layer->at(localX, localY);
    if ( cell.brush != " " )
    {
        M_brush.setCurrentBrush(cell.brush);
        M_brush.setColor(cell.color);
    }

    return true;

}



}

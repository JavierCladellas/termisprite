#include "paint_tool.hpp"
#include "brush_tool.hpp"

namespace Termisprite
{

bool PaintTool::processKeyboardEvent( ftxui::Event event )
{

    if ( event != ftxui::Event::Character(' ') && event != ftxui::Event::Return )
        return false;

    floodFillPaint( M_cursor.x(), M_cursor.y() );
    return true;
}

bool PaintTool::processMouseEvent( ftxui::Event event )
{
    if ( !event.is_mouse() )
        return false;

    auto mouse = event.mouse();


    auto [localX, localY] = M_screenToWorld(mouse.x, mouse.y);

    if ( localX < 0 || localY < 0 )
        return false;
    if ( mouse.button != ftxui::Mouse::Button::Left || mouse.motion != ftxui::Mouse::Pressed )
        return false;


    floodFillPaint( localX, localY );
    return true;

}


void
PaintTool::floodFillPaint( int x, int y )
{
    auto [width,height] = M_layer->size();
    auto [lX, lY] = M_layer->position();
    auto [camX, camY] = M_layer->camera().position();

    int localX = x + camX - lX;
    int localY = y + camY - lY;

    if ( localX < 0 || localX >= width || localY < 0 || localY >= height )
        return;

    Pixel & targetCell = M_layer->at(localX, localY);
    Pixel targetPixel = targetCell;

    if ( M_brush == targetPixel )
        return;

    std::stack<std::pair<int, int>> stack;
    stack.push({localX, localY});

    while (!stack.empty())
    {
        auto [cx, cy] = stack.top();
        stack.pop();

        if ( cx < 0 || cx >= width || cy < 0 || cy >= height )
            continue;

        Pixel & currentCell = M_layer->at(cx, cy);

        if ( currentCell.brush != targetPixel.brush || currentCell.color != targetPixel.color )
            continue;

        M_brush.apply(currentCell);

        stack.push({cx + 1, cy});
        stack.push({cx - 1, cy});
        stack.push({cx, cy + 1});
        stack.push({cx, cy - 1});
    }
}


}

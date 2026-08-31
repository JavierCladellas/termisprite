#include "shape_tool.hpp"
#include "geometry.hpp"
#include "brush_tool.hpp"



namespace Termisprite
{


void
ShapeTool::drawLine( int x0, int y0, int x1, int y1 )
{
    Geometry::drawLine(M_brush, *M_layer, x0, y0, x1, y1);
}

void
ShapeTool::drawRectangle( int x0, int y0, int x1, int y1 )
{
    int minX = std::min(x0, x1);
    int maxX = std::max(x0, x1);
    int minY = std::min(y0, y1);
    int maxY = std::max(y0, y1);

    for ( int x = minX; x <= maxX; ++x )
    {
        M_brush.apply( x, minY);
        M_brush.apply( x, maxY);
    }

    for ( int y = minY; y <= maxY; ++y )
    {
        M_brush.apply( minX, y);
        M_brush.apply( maxX, y);
    }
}

void
ShapeTool::drawEllipse( int x0, int y0, int x1, int y1 )
{
    float xc = (x0 + x1) / 2.;
    float yc = (y0 + y1) / 2.;
        float a = std::abs(x1 - x0) / 2.;
    float b = std::abs(y1 - y0) / 2.;

    if ( x0 == x1 && y0 == y1 )
    {
        M_brush.apply( x0, y0 );
        return;
    }

    int points = std::max(a, b) * 8;
    int width, height;
    std::tie(width, height) = M_layer->size();
    for ( int i = 0; i <= points; ++i )
    {
        float theta = 2. * M_PI * i / points;

        int x = std::round(xc + a * std::cos(theta));
        int y = std::round(yc + b * std::sin(theta));

        if ( x >= 0 && x < width && y >= 0 && y < height )
            M_brush.apply( x,y );
    }
}




bool
ShapeTool::processKeyboardEvent( ftxui::Event event )
{

    if ( M_isDrawing && event == ftxui::Event::Escape )
    {
        M_isDrawing = false;
        M_layer = M_snapshot;
        return true;
    }

    if ( event == ftxui::Event::Character(' ') || event == ftxui::Event::Return )
    {
        M_cursor.setVisibility(true);
        if ( !M_isDrawing )
        {
            M_isDrawing = true;
            M_shapeStartX = M_cursor.x();
            M_shapeStartY = M_cursor.y();
            M_snapshot = M_layer;

            M_layer = M_snapshot;
            if ( M_toolType == ToolType::SQUARE )
                drawRectangle( M_shapeStartX, M_shapeStartY, M_cursor.x(), M_cursor.y() );
            else if ( M_toolType == ToolType::CIRCLE )
                drawEllipse( M_shapeStartX, M_shapeStartY, M_cursor.x(), M_cursor.y() );
            else if ( M_toolType == ToolType::LINE )
                drawLine( M_shapeStartX, M_shapeStartY, M_cursor.x(), M_cursor.y() );
        }
        else
            M_isDrawing = false;
        return true;
    }


    if ( M_isDrawing )
    {

        int width, height;
        std::tie(width, height) = M_layer->size();

        bool moved = false;

        if ( event == ftxui::Event::ArrowUp || event == ftxui::Event::Character('k') )
        {
            M_cursor.y() = std::max(0, M_cursor.y() - 1);
            moved = true;
        }
        else if ( event == ftxui::Event::ArrowDown || event == ftxui::Event::Character('j') )
        {
            M_cursor.y() = std::min(height - 1, M_cursor.y() + 1);
            moved = true;
        }
        else if ( event == ftxui::Event::ArrowLeft || event == ftxui::Event::Character('h') )
        {
            M_cursor.x() = std::max(0, M_cursor.x() - 1);
            moved = true;
        }
        else if ( event == ftxui::Event::ArrowRight || event == ftxui::Event::Character('l') )
        {
            M_cursor.x() = std::min(width - 1, M_cursor.x() + 1);
            moved = true;
        }

        if ( moved )
        {
            M_cursor.setVisibility(true);
            M_layer = M_snapshot;

            if ( M_toolType == ToolType::SQUARE )
                drawRectangle( M_shapeStartX, M_shapeStartY, M_cursor.x(), M_cursor.y() );
            else if ( M_toolType == ToolType::CIRCLE )
                drawEllipse( M_shapeStartX, M_shapeStartY, M_cursor.x(), M_cursor.y() );
            else if ( M_toolType == ToolType::LINE )
                drawLine( M_shapeStartX, M_shapeStartY, M_cursor.x(), M_cursor.y() );

            return true;
        }
    }

    return false;

}


bool
ShapeTool::processMouseEvent( ftxui::Event event )
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

    auto [localX, localY] = M_screenToWorld(mouse.x, mouse.y);
    if ( localX < 0 || localY < 0 ) //screenToWorld returned invalid coordinates (out of screen box )
        return false;

    if ( mouse.button == ftxui::Mouse::Button::Left )
    {
        M_cursor.setVisibility(false);

        if ( mouse.motion == ftxui::Mouse::Pressed )
        {
            M_isDrawing = true;
            M_shapeStartX = localX;
            M_shapeStartY = localY;

            M_snapshot = M_layer;
            return true;
        }
        else if ( (mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed) && M_isDrawing )
        {
            M_layer = M_snapshot;

            if ( M_toolType == ToolType::SQUARE )
                drawRectangle( M_shapeStartX, M_shapeStartY, localX, localY );
            else if ( M_toolType == ToolType::CIRCLE )
                drawEllipse( M_shapeStartX, M_shapeStartY, localX, localY );
            else if ( M_toolType == ToolType::LINE )
                drawLine( M_shapeStartX, M_shapeStartY, localX, localY );

            return true;
        }
    }
    return false;

}


}

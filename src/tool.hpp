#pragma once

#include "cursor.hpp"
#include "sprite.hpp"

namespace Termisprite
{

class BrushTool;
class Tool
{
public:
    Tool( Layer & layer, BrushTool & brush, CanvasCursor & cursor, std::function<std::pair<int,int>(int,int)> screenToWorld )
        : M_layer( layer ), M_brush( brush ), M_cursor(cursor), M_screenToWorld( screenToWorld )
    {}
    virtual ~Tool() = default;

    virtual bool processKeyboardEvent( ftxui::Event event ) = 0;
    virtual bool processMouseEvent( ftxui::Event event ) = 0;

    bool isDrawing() const { return M_isDrawing; }
    bool isActive() const { return M_isActive; }
    void setActive( bool active ) { M_isActive = active; }

protected:
    Layer & M_layer;
    BrushTool & M_brush;
    std::function<std::pair<int,int>(int,int)> M_screenToWorld;
    CanvasCursor & M_cursor;

    bool M_isActive = false;

    bool M_isDrawing = false;
    int M_lastX = 0, M_lastY = 0;

};


}

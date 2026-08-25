
#pragma once

#include "tool.hpp"
#include <ftxui/component/event.hpp>


namespace Termisprite
{

class PaintTool
    : public Tool
{
public:
    PaintTool( Sprite & sprite, BrushTool & brush, CanvasCursor & cursor, std::function<std::pair<int,int>(int,int)> screenToWorld )
        : Tool( sprite, brush, cursor, screenToWorld )
    {}

    bool processKeyboardEvent( ftxui::Event event ) override;
    bool processMouseEvent( ftxui::Event event ) override;


private:
    void floodFillPaint( int x, int y );

};


}

#pragma once

#include "tool.hpp"
#include "tools_section.hpp"



namespace Termisprite
{

class ShapeTool
    : public Tool
{
public:
    ShapeTool( ToolType & toolType, Sprite & sprite, Sprite & snapshot, BrushTool & brush, CanvasCursor & cursor, std::function<std::pair<int,int>(int,int)> screenToWorld )
        : Tool( sprite, brush, cursor, screenToWorld ), M_toolType( toolType ), M_snapshot( snapshot )
    {}

    bool processKeyboardEvent( ftxui::Event event ) override;
    bool processMouseEvent( ftxui::Event event ) override;

private:
    void drawLine( int x0, int y0, int x1, int y1 );
    void drawRectangle( int x0, int y0, int x1, int y1 );
    void drawEllipse( int x0, int y0, int x1, int y1 );

private:
    ToolType & M_toolType;
    Sprite & M_snapshot;

    int M_shapeStartX = 0;
    int M_shapeStartY = 0;

};






}

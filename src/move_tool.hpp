#pragma once

#include "tool.hpp"

namespace Termisprite
{

class MoveTool
    : public Tool
{
public:
    MoveTool(Layer * layer, BrushTool & brush, CanvasCursor & cursor, std::function<std::pair<int,int>(int,int)> screenToWorld )
        : Tool( layer, brush, cursor, screenToWorld )
    {}

    bool processKeyboardEvent( ftxui::Event event ) override;
    bool processMouseEvent( ftxui::Event event ) override;

private:

    bool processTranslation( ftxui::Event event, int maxWidth, int maxHeight );
    void translateLayer( int dx, int dy );
};


}

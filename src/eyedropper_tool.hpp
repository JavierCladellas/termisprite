
#pragma once

#include "tool.hpp"
#include <ftxui/component/event.hpp>


namespace Termisprite
{

class EyeDropperTool
    : public Tool
{
public:
    EyeDropperTool( Layer * layer, BrushTool & brush, CanvasCursor & cursor, std::function<std::pair<int,int>(int,int)> screenToWorld )
        : Tool( layer, brush, cursor, screenToWorld )
    {}

    bool processKeyboardEvent( ftxui::Event event ) override;
    bool processMouseEvent( ftxui::Event event ) override;


private:

};


}

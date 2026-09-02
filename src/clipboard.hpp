#pragma once

#include "selection_tool.hpp"
#include "sprite.hpp"

namespace Termisprite
{

struct Clipboard
{
    bool hasData = false;
    Layer::GridData data;


    void copy( Layer const& layer, SelectionTool & selectionTool );

    void cut( Layer & layer, SelectionTool & selectionTool );

    void paste( Layer & layer, int cursorX = 0, int cursorY = 0 );

};

}

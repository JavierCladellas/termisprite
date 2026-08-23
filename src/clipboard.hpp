#pragma once

#include "selection_tool.hpp"
#include "sprite.hpp"

namespace Termisprite
{

struct Clipboard
{
    bool hasData = false;
    Sprite::GridData data;
    

    void copy( Sprite const& sprite, SelectionTool & selectionTool );

    void cut( Sprite & sprite, SelectionTool & selectionTool );

    void paste( Sprite & sprite, int cursorX = 0, int cursorY = 0 );

};

}

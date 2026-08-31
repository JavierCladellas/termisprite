#pragma once

#include "tool.hpp"
#include <ftxui/component/event.hpp>
#include <ftxui/screen/color.hpp>
#include <vector>

#include <ftxui/dom/elements.hpp>

namespace Termisprite
{

class SelectionTool
    : public Tool
{
public:
    SelectionTool( Layer & layer, BrushTool & brush, CanvasCursor & cursor, Layer & snapshot, std::function<std::pair<int,int>(int,int)> screenToWorld )
        : Tool( layer, brush, cursor, screenToWorld ), M_snapshot(snapshot)
     {}

    void render( std::vector<ftxui::Elements> & cells, bool isSquarePixel );
    bool processKeyboardEvent( ftxui::Event event ) override;
    bool processMouseEvent( ftxui::Event event ) override;

    bool processTranslation( ftxui::Event event, int maxWidth, int maxHeight );

    int width() const { return maxX() - minX() + 1; }
    int height() const { return maxY() - minY() + 1; }

    int minX() const { return std::min(M_startX, M_endX); }
    int minY() const { return std::min(M_startY, M_endY); }
    int maxX() const { return std::max(M_startX, M_endX); }
    int maxY() const { return std::max(M_startY, M_endY); }

    int startX(){ return M_startX; }
    int startY(){ return M_startY; }
    int endX(){ return M_endX; }
    int endY(){ return M_endY; }

    void setStart( int x, int y) { M_startX = x; M_startY = y; }
    void setEnd( int x, int y) { M_endX = x; M_endY = y; }

    bool isTranslating() const { return M_isTranslating; }

public:
    void deleteContent( Layer & layer );

    void beginTranslation();
    void endTranslation();
    bool translateContent( int dx, int dy );



private:
    Layer & M_snapshot;

    int M_startX = 0, M_startY = 0;
    int M_endX = 0, M_endY = 0;

    ftxui::Color M_color = ftxui::Color::White;

    //Copy of layer fragment as sort of floating layer
    Layer::GridData M_selection;

    bool M_isTranslating = false;
};

}

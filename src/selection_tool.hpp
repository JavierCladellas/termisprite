#pragma once

#include "cursor.hpp"
#include "sprite.hpp"

#include <ftxui/component/event.hpp>
#include <ftxui/screen/color.hpp>
#include <vector>

#include <ftxui/dom/elements.hpp>

namespace Termisprite
{

class SelectionTool
{
public:
    SelectionTool() {}

    void render( std::vector<ftxui::Elements> & cells, bool isSquarePixel );
    bool processSelection( CanvasCursor & cursor, Sprite & sprite, Sprite & snapshot, bool & isDrawing,
                           int canvasHeight, int canvasWidth, ftxui::Event event,
                           std::function<std::pair<int,int>(int,int)> screenToWorld = [](int x, int y){ return std::pair<int,int>{x,y}; } );

    bool processTranslation( CanvasCursor & cursor, Sprite & sprite, Sprite & snapshot,
                             int canvasHeight, int canvasWidth, ftxui::Event event );


    bool isActive() const { return M_isActive; }
    void setActive( bool active ) { M_isActive = active; }

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
    void deleteContent( Sprite & sprite );

    void beginTranslation( Sprite const& sprite, Sprite & snapshot );
    void endTranslation();
    bool translateContent( Sprite & sprite, Sprite & snapshot, int maxH, int maxW, int dx, int dy );



private:
    int M_startX = 0, M_startY = 0;
    int M_endX = 0, M_endY = 0;

    int M_lastDragX = 0, M_lastDragY = 0;

    bool M_isActive = false;
    ftxui::Color M_color = ftxui::Color::White;

    //Copy of sprite fragment as sort of floating layer
    Sprite::GridData M_selection;

    bool M_isTranslating = false;
};

}

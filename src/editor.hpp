#pragma once

#include <deque>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>

#include "sprite.hpp"

namespace Termisprite
{



enum class ToolType
{
    DRAW,
    ERASER,
    SQUARE,
    CIRCLE,
    LINE,
    EYE_DROPPER,
    PAINT_FILL,
    BOX_SELECT
};

struct SelectionBounds
{
    bool isActive = false;
    int startX = 0, startY = 0;
    int endX = 0, endY = 0;

    int minX() const { return std::min(startX, endX); }
    int minY() const { return std::min(startY, endY); }
    int maxX() const { return std::max(startX, endX); }
    int maxY() const { return std::max(startY, endY); }
    int width() const { return maxX() - minX() + 1; }
    int height() const { return maxY() - minY() + 1; }
};

struct Clipboard
{
    bool hasData = false;
    Sprite::GridData data;
};


struct EditorState
{
    std::string brush = "█";
    ftxui::Color color = ftxui::Color::White;
    ToolType toolType = ToolType::DRAW;
    std::vector<ftxui::Color> palette;

    SelectionBounds selection;
    Sprite::GridData floatingSelection;
    Clipboard clipboard;
};




class SpriteHistory
{
public:
    SpriteHistory( int maxSize = 50 )
        : M_maxSize( maxSize ), M_currentIndex( 0 )
    { }

    void push( Sprite const & sprite ) { M_history.push_back( sprite ); }
    void save( Sprite const& sprite );
    void undo( Sprite & sprite );
    void redo( Sprite & sprite );


private:
    std::deque<Sprite> M_history;
    int M_maxSize = 50;
    int M_currentIndex = 0;
};

class EditorCanvasComponent
    : public ftxui::ComponentBase
{
public:
    EditorCanvasComponent( int width = 32, int height = 32 )
        : M_width( width ), M_height( height ),
          M_cursorX( 0 ), M_cursorY( 0 ),
          M_sprite( width, height )
    {
        M_spriteHistory.push( M_sprite );
        Add( M_rightClickModal );
    }

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

    bool Focusable() const override { return true; }


    EditorState & currentState() { return M_currentState; }
    void setCurrentState( EditorState state ) { M_currentState = state; }


    std::pair<int, int> size() const { return { M_width, M_height }; }
    void resize( int width, int height ) { M_width = width; M_height = height; M_sprite.resize( width, height ); saveState(); }

    //TODO: Implement flipVertical and flipHorizontal in Sprite class
    void flipVertical() { M_sprite.flipVertical(); saveState(); }
    void flipHorizontal() { M_sprite.flipHorizontal(); saveState(); }

    void undo() {
        M_spriteHistory.undo( M_sprite );
        auto [width, height] = M_sprite.size();
        M_width = width;
        M_height = height;
    }
    void redo() {
        M_spriteHistory.redo( M_sprite );
        auto [width, height] = M_sprite.size();
        M_width = width;
        M_height = height;
    }
    void toggleGrid() {
        M_showPointGrid = !M_showPointGrid;
        if (M_showPointGrid) M_showCheckerboardGrid = false;
    }
    void toggleCheckerboardGrid() {
        M_showCheckerboardGrid = !M_showCheckerboardGrid;
        if (M_showCheckerboardGrid) M_showPointGrid = false;
    }

    void copyToClipboard();
    void cutToClipboard();
    void pasteClipboard();
    void clear();
    void deleteSelection();

private:
    std::vector<ftxui::Color> palette() const;

    void saveState();

    //Bresenham's line algorithm
    void drawLine( int x0, int y0, int x1, int y1 );
    void drawSquare( int x0, int y0, int x1, int y1 );
    void drawCircle( int x0, int y0, int x1, int y1 );

    void floodFillPaint( int x, int y );


    void beginTranslation();
    void endTranslation();
    bool translateSelection( int dx, int dy );

    bool processCursorMovement( ftxui::Event event );
    bool processKeyboardDrawing( ftxui::Event event );

    bool processMouseDrawing( ftxui::Event event );
    bool processEyeDropper( ftxui::Event event );
    bool processPaintFill( ftxui::Event event );
    bool processBoxSelection( ftxui::Event event );
    bool processShapeDrawing( ftxui::Event event );
    bool processRightClickModal( ftxui::Event event );

private:
    int M_width, M_height;
    int M_cursorX, M_cursorY;
    bool M_showCursor = true;

    Sprite M_sprite;
    Sprite M_spriteSnapshot;
    SpriteHistory M_spriteHistory;

    EditorState M_currentState;

    ftxui::Box M_box;
    bool M_isDrawing = false;
    int M_lastDrawX = 0;
    int M_lastDrawY = 0;

    bool M_isTranslating = false;
    int M_lastDragX = 0;
    int M_lastDragY = 0;

    int M_shapeStartX = 0;
    int M_shapeStartY = 0;

    bool M_showPointGrid = true;
    bool M_showCheckerboardGrid = false;


    //TODO: REFACTOR
    int M_modalX = 0;
    int M_modalY = 0;
    ftxui::Box M_rightClickModalBox;
    bool M_showRightClickModal = false;
    int M_rightClickModalIndex = 0;
    std::vector<std::string> M_rightClickModalOptions = { "Grid [g]", "Checkerboard [G]", "Undo [u]", "Redo [Ctrl+r]", "Clear [Ctrl+d]", "Cancel [Esc]" };
    ftxui::Component M_rightClickModal = ftxui::Menu(&M_rightClickModalOptions, &M_rightClickModalIndex);

};


std::shared_ptr<EditorCanvasComponent> EditorCanvas( int width = 32, int height = 32 );

}

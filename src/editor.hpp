#pragma once

#include <deque>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

namespace Termisprite
{


struct Pixel
{
    std::string brush = " ";
    ftxui::Color color = ftxui::Color::White;
};


enum class ToolType
{
    BRUSH,
    ERASER,
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
    std::vector<std::vector<Pixel>> data;
};


struct EditorState
{
    std::string brush = "█";
    ftxui::Color color = ftxui::Color::White;
    ToolType toolType = ToolType::BRUSH;
    std::vector<ftxui::Color> palette;

    SelectionBounds selection;
    std::vector<std::vector<Pixel>> floatingSelection;
    Clipboard clipboard;
};


class EditorCanvasComponent
    : public ftxui::ComponentBase
{
    using GridData = std::vector<std::vector<Pixel>>;
public:
    EditorCanvasComponent( int width = 64, int height = 32 )
        : M_width( width ), M_height( height ),
          M_cursorX( 0 ), M_cursorY( 0 )
    {
        M_grid.resize( M_height, std::vector<Pixel>( M_width ) );
        M_history.push_back( M_grid );
    }

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

    bool Focusable() const override { return true; }


    EditorState & currentState() { return M_currentState; }
    void setCurrentState( EditorState state ) { M_currentState = state; }


private:
    std::vector<ftxui::Color> palette() const;

    void saveState();

    //Bresenham's line algorithm
    void drawLine( int x0, int y0, int x1, int y1 );

    void floodFillPaint( int x, int y );

    void beginTranslation();
    void endTranslation();
    bool translateSelection( int dx, int dy );

    bool processHistoryEvents( ftxui::Event event );
    bool processCursorMovement( ftxui::Event event );
    bool processKeyboardDrawing( ftxui::Event event );
    bool processClipboardEvents( ftxui::Event event );

    bool processMouseDrawing( ftxui::Event event );
    bool processEyeDropper( ftxui::Event event );
    bool processPaintFill( ftxui::Event event );
    bool processBoxSelection( ftxui::Event event );

private:
    int M_width, M_height;
    int M_cursorX, M_cursorY;

    bool M_showCursor = true;

    GridData M_grid;
    GridData M_gridSnapshot;

    std::deque<GridData> M_history;
    int M_historyIndex = 0;
    int M_maxHistorySize = 50;

    EditorState M_currentState;

    ftxui::Box M_box;
    bool M_isDrawing = false;
    int M_lastDrawX = 0;
    int M_lastDrawY = 0;

    bool M_isTranslating = false;
    int M_lastDragX = 0;
    int M_lastDragY = 0;
};


std::shared_ptr<EditorCanvasComponent> EditorCanvas( int width = 64, int height = 32 );

}

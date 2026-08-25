#pragma once


#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>

#include "eyedropper_tool.hpp"
#include "paint_tool.hpp"
#include "tools_section.hpp"
#include "brush_tool.hpp"
#include "cursor.hpp"
#include "grid.hpp"
#include "selection_tool.hpp"
#include "sprite.hpp"
#include "clipboard.hpp"

namespace Termisprite
{




struct EditorState
{
    ftxui::Color backgroundColor = ftxui::Color();
    ToolType toolType = ToolType::DRAW;

    SelectionTool * selectionTool;
    Clipboard clipboard;
};





class EditorCanvasComponent
    : public ftxui::ComponentBase
{
public:
    EditorCanvasComponent( int width = 48, int height = 48 )
        : M_width( width ), M_height( height ),
          M_sprite( width, height ),
          M_grid( std::make_unique<Grid>() ),
          M_cursor( std::make_unique<CanvasCursor>() )
    {
        //TODO: Its super weird that the brush tool needs a ref to itself hehe
        M_brushTool = std::make_unique<BrushTool>( M_sprite, *M_brushTool, *M_cursor, std::bind(&EditorCanvasComponent::screenToWorld, this, std::placeholders::_1, std::placeholders::_2) );

        M_selectionTool = std::make_unique<SelectionTool>( M_sprite, *M_brushTool, *M_cursor, M_spriteSnapshot, std::bind(&EditorCanvasComponent::screenToWorld, this, std::placeholders::_1, std::placeholders::_2) );
        M_currentState.selectionTool = M_selectionTool.get();
        M_eyeDropperTool = std::make_unique<EyeDropperTool>( M_sprite, *M_brushTool, *M_cursor, std::bind(&EditorCanvasComponent::screenToWorld, this, std::placeholders::_1, std::placeholders::_2) );
        M_paintTool = std::make_unique<PaintTool>( M_sprite, *M_brushTool, *M_cursor, std::bind(&EditorCanvasComponent::screenToWorld, this, std::placeholders::_1, std::placeholders::_2) );

        M_cells = std::vector<ftxui::Elements>( M_height, ftxui::Elements( M_width, ftxui::text(" ") ) );

        M_spriteHistory.push( M_sprite );

        Add( M_rightClickModal );
    }

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;
    bool Focusable() const override { return true; }


    EditorState & currentState() { return M_currentState; }
    void setCurrentState( EditorState state ) { M_currentState = state; }


    std::pair<int, int> size() const { return { M_width, M_height }; }
    void resize( int width, int height ) {
        M_width = width;
        M_height = height;

        M_cells.resize( height );
        for ( auto & row : M_cells )
            row.resize( width );

        //TODO: Remove below when layers impl
        M_sprite.resize( width, height );

        saveState();
    }

    //TODO: Implement flipVertical and flipHorizontal in Sprite class
    void flipVertical() { M_sprite.flipVertical(); saveState(); }
    void flipHorizontal() { M_sprite.flipHorizontal(); saveState(); }


    Grid & grid(){ return *M_grid; }
    SelectionTool & selectionTool(){ return *M_selectionTool; }
    Sprite & sprite(){ return M_sprite; }
    CanvasCursor & cursor(){ return *M_cursor; }
    BrushTool & brushTool(){ return *M_brushTool; }


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
    void toggleSquarePixel() { M_squarePixel = !M_squarePixel; }

    std::vector<ftxui::Color> const& colorsInCanvas() const { return M_colorsInCanvas; }
    void clear();

    std::function<void()> onBackgroundChangeRequested;


    void saveState();

private:

    std::pair<int,int> screenToWorld(int screenX, int screenY) const;
    std::pair<int,int> worldToScreen(int worldX, int worldY) const;

    std::vector<ftxui::Color> computeColorsInCanvas() const;


    void drawSquare( int x0, int y0, int x1, int y1 );
    void drawCircle( int x0, int y0, int x1, int y1 );

    bool processPanning( ftxui::Event event );

    bool processShapeDrawing( ftxui::Event event );
    bool processRightClickModal( ftxui::Event event );

private:
    int M_width, M_height;

    Sprite M_sprite;
    Sprite M_spriteSnapshot;
    SpriteHistory M_spriteHistory;

    EditorState M_currentState;

    std::unique_ptr<BrushTool> M_brushTool;
    std::unique_ptr<CanvasCursor> M_cursor;
    std::unique_ptr<SelectionTool> M_selectionTool;
    std::unique_ptr<EyeDropperTool> M_eyeDropperTool;
    std::unique_ptr<PaintTool> M_paintTool;

    std::vector<ftxui::Elements> M_cells;
    std::unique_ptr<Grid> M_grid;


    std::vector<ftxui::Color> M_colorsInCanvas;


    ftxui::Box M_box;
    bool M_isDrawing = false;
    int M_lastDrawX = 0;
    int M_lastDrawY = 0;


    int M_cameraX = 0;
    int M_cameraY = 0;

    bool M_isPanning = false;
    int M_lastPanMouseX = 0;
    int M_lastPanMouseY = 0;

    int M_shapeStartX = 0;
    int M_shapeStartY = 0;

    bool M_squarePixel = true;

    //TODO: REFACTOR
    int M_modalX = 0;
    int M_modalY = 0;
    ftxui::Box M_rightClickModalBox;
    bool M_showRightClickModal = false;
    int M_rightClickModalIndex = 0;
    std::vector<std::string> M_rightClickModalOptions = { "Background", "Toggle Grid [g]", "Switch Grid [G]", "Undo [u]", "Redo [Ctrl+r]", "Clear [Ctrl+d]", "Cancel [Esc]" };
    ftxui::Component M_rightClickModal = ftxui::Menu(&M_rightClickModalOptions, &M_rightClickModalIndex);

};


std::shared_ptr<EditorCanvasComponent> EditorCanvas( int width = 48, int height = 48 );

}

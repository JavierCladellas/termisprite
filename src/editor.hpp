#pragma once


#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>

#include "camera.hpp"
#include "contextwindow.hpp"
#include "eyedropper_tool.hpp"
#include "paint_tool.hpp"
#include "shape_tool.hpp"
#include "shortcuts.hpp"
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
    EditorCanvasComponent( int width = 48, int height = 48, ShortcutManager * shortcutManager = nullptr )
        : M_width( width ), M_height( height ),
          M_shortcutManager( shortcutManager ),
          M_contextWindow( ContextWindow( shortcutManager) )
    {
        M_layers.push_back( std::make_unique<Layer>( width, height, "Layer 1" ) );
        setActiveLayer( 0 );

        M_camera = std::make_unique<Camera>( M_width, M_height );
        //TODO: Must compute available size in screen
        M_cells = std::vector<ftxui::Elements>( M_camera->height(), ftxui::Elements( M_camera->width() * ( M_squarePixel ? 2 : 1 ), ftxui::text(" ") ) );

        // updateViewport();
        //editor width height = how many pixels, user defined
        //camera width height = how many pixels fit on the screen, can be smaller than editor and sprite. Controls M_cells
        //sprite width height = how many pixels in the sprite, can be bigger than camera and editor
        //


        M_grid = std::make_unique<Grid>(),
        M_cursor = std::make_unique<CanvasCursor>();
        M_brushTool = std::make_unique<BrushTool>( M_activeLayer, *M_cursor, std::bind(&EditorCanvasComponent::screenToWorld, this, std::placeholders::_1, std::placeholders::_2) );

        M_selectionTool = std::make_unique<SelectionTool>( M_activeLayer, *M_brushTool, *M_cursor, &M_spriteSnapshot, std::bind(&EditorCanvasComponent::screenToWorld, this, std::placeholders::_1, std::placeholders::_2) );
        M_currentState.selectionTool = M_selectionTool.get();
        M_eyeDropperTool = std::make_unique<EyeDropperTool>( M_activeLayer, *M_brushTool, *M_cursor, std::bind(&EditorCanvasComponent::screenToWorld, this, std::placeholders::_1, std::placeholders::_2) );
        M_paintTool = std::make_unique<PaintTool>( M_activeLayer, *M_brushTool, *M_cursor, std::bind(&EditorCanvasComponent::screenToWorld, this, std::placeholders::_1, std::placeholders::_2) );
        M_shapeTool = std::make_unique<ShapeTool>( M_currentState.toolType, M_activeLayer, &M_spriteSnapshot, *M_brushTool, *M_cursor, std::bind(&EditorCanvasComponent::screenToWorld, this, std::placeholders::_1, std::placeholders::_2) );


        M_spriteHistory.push( *M_activeLayer );

        Add( M_contextWindow );
    }

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;
    bool Focusable() const override { return true; }


    EditorState & currentState() { return M_currentState; }
    void setCurrentState( EditorState state ) { M_currentState = state; }

    std::pair<int, int> size() const { return { M_width, M_height }; }
    void resize( int width, int height );
    void updateViewport();


    Grid & grid(){ return *M_grid; }
    SelectionTool & selectionTool(){ return *M_selectionTool; }
    CanvasCursor & cursor(){ return *M_cursor; }
    BrushTool & brushTool(){ return *M_brushTool; }


    void undo() {
        //TODO: Fix this, buggy.
        M_spriteHistory.undo( *M_activeLayer );
    }
    void redo() {
        //TODO: Fix this, buggy.
        M_spriteHistory.redo( *M_activeLayer );
    }
    void toggleSquarePixel()
    {
        M_squarePixel = !M_squarePixel;
        updateViewport();
    }

    std::vector<ftxui::Color> const& colorsInCanvas() const { return M_colorsInCanvas; }
    void clear();

    std::function<void()> onBackgroundChangeRequested;

    void saveState();

    std::vector<std::unique_ptr<Layer>> const& layers() const { return M_layers; }
    int const& activeLayerIndex() const { return M_activeLayerIndex; }
    Layer & activeLayer() { return *M_layers[M_activeLayerIndex]; }
    Layer const& activeLayer() const { return *M_layers[M_activeLayerIndex]; }
    void addLayer( std::string const& name = "New Layer" )
    {
        M_layers.insert( M_layers.begin(), std::make_unique<Layer>( M_width, M_height, name ) );
        M_activeLayerIndex = 0;
    }
    void removeLayer( int index )
    {
        if ( index < 0 || index >= M_layers.size() )
            return;
        if ( M_layers.size() == 1 ) //Always need at least one layer
            return;
        M_layers.erase( M_layers.begin() + index );
        if ( M_activeLayerIndex >= M_layers.size() )
            M_activeLayerIndex = M_layers.size() - 1;
        M_activeLayer = M_layers[M_activeLayerIndex].get();
    }
    void setActiveLayer( int index )
    {
        if ( index < 0 || index >= M_layers.size() )
            return;
        M_activeLayerIndex = index;
        M_activeLayer = M_layers[index].get();

        if (M_brushTool) M_brushTool->setLayer( M_activeLayer );
        if (M_selectionTool) M_selectionTool->setLayer( M_activeLayer );
        if (M_eyeDropperTool) M_eyeDropperTool->setLayer( M_activeLayer );
        if (M_paintTool) M_paintTool->setLayer( M_activeLayer );
        if (M_shapeTool) M_shapeTool->setLayer( M_activeLayer );


    }
    void moveLayerUp( int index )
    {
        if ( index <= 0 || index >= M_layers.size() )
            return;
        std::swap( M_layers[index], M_layers[index - 1] );
        if ( M_activeLayerIndex == index )
            M_activeLayerIndex--;
        else if ( M_activeLayerIndex == index - 1 )
            M_activeLayerIndex++;
    }
    void moveLayerDown( int index )
    {
        if ( index < 0 || index >= M_layers.size() - 1 )
            return;
        std::swap( M_layers[index], M_layers[index + 1] );
        if ( M_activeLayerIndex == index )
            M_activeLayerIndex++;
        else if ( M_activeLayerIndex == index + 1 )
            M_activeLayerIndex--;
    }

    bool & activeLayerBorderVisible() { return M_activeLayerBorderVisible; }

private:

    void renderActiveLayerBorder();
    std::pair<int,int> screenToWorld(int screenX, int screenY) const;
    std::pair<int,int> worldToScreen(int worldX, int worldY) const;

    std::vector<ftxui::Color> computeColorsInCanvas() const;

    bool processRightClickModal( ftxui::Event event );

private:
    ShortcutManager * M_shortcutManager;

    int M_width, M_height;

    int M_activeLayerIndex = 0;
    Layer * M_activeLayer = nullptr;
    std::vector<std::unique_ptr<Layer>> M_layers;

    Layer M_spriteSnapshot;
    SpriteHistory M_spriteHistory;

    EditorState M_currentState;

    std::unique_ptr<BrushTool> M_brushTool;
    std::unique_ptr<CanvasCursor> M_cursor;
    std::unique_ptr<SelectionTool> M_selectionTool;
    std::unique_ptr<EyeDropperTool> M_eyeDropperTool;
    std::unique_ptr<PaintTool> M_paintTool;
    std::unique_ptr<ShapeTool> M_shapeTool;
    std::unique_ptr<Camera> M_camera;

    std::shared_ptr<ContextWindowComponent> M_contextWindow;

    std::vector<ftxui::Elements> M_cells;
    std::unique_ptr<Grid> M_grid;


    std::vector<ftxui::Color> M_colorsInCanvas;

    ftxui::Box M_box;
    ftxui::Box M_availableBox;

    bool M_squarePixel = true;
    bool M_activeLayerBorderVisible = false;
};


std::shared_ptr<EditorCanvasComponent> EditorCanvas( int width = 48, int height = 48, ShortcutManager * shortcutManager = nullptr );

}

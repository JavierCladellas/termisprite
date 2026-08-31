#include "geometry.hpp"
#include "editor.hpp"
#include <cmath>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>


namespace Termisprite
{


ftxui::Element
EditorCanvasComponent::OnRender()
{

    M_grid->render( M_cells, M_squarePixel );

    //Reverse order
    for ( int i = M_layers.size() - 1; i >= 0; --i )
        M_layers[i].get()->render( M_cells, M_squarePixel );

    M_selectionTool->render(M_cells, M_squarePixel);
    M_cursor->render( M_cells, *M_brushTool, M_squarePixel );

    ftxui::Elements cellsElt( M_cells.size() );
    for ( int i = 0; i < M_cells.size(); ++i )
        cellsElt[i] = ftxui::hbox( M_cells[i] );

    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    ftxui::Element canvas = ftxui::vbox( cellsElt )
                              | ftxui::bgcolor( M_currentState.backgroundColor )
                              | ftxui::borderStyled( borderColor )
                              | ftxui::size( ftxui::WIDTH, ftxui::EQUAL, (M_camera->width() * (M_squarePixel ? 2 : 1) ) + 1 )
                              | ftxui::size( ftxui::HEIGHT, ftxui::EQUAL, M_camera->height() + 2 )
                              | ftxui::reflect( M_box );


    //------------Build axis---------
    ftxui::Elements xAxis = {ftxui::text("─")};
    std::string xAxisLabel = std::to_string(M_width) + "px";

    for ( int i = 0; i < M_camera->width()*(M_squarePixel ? 2 : 1) - xAxisLabel.size(); ++i )
        xAxis.push_back(ftxui::text("─"));

    xAxis.push_back(ftxui::text("►"));
    xAxis.push_back(ftxui::text(xAxisLabel));

    ftxui::Elements yAxis = {ftxui::text(std::to_string(M_height) + "px ▲ ")};
    for (int i = 0; i < M_camera->height() + 1; ++i )
        yAxis.push_back(ftxui::text("│ ") | ftxui::align_right);

    yAxis.push_back(ftxui::text("─┼─") | ftxui::align_right);

    auto axisColor = ftxui::Color::GrayDark; //TODO: Use palette
    ftxui::Element canvasWithAxes = ftxui::hbox({
        ftxui::vbox(std::move(yAxis)) | ftxui::color(axisColor),
        ftxui::vbox({ canvas, ftxui::hbox(std::move(xAxis)) | ftxui::color(axisColor) })
    });
    //-----------------------------//

    ftxui::Element responsiveCanvasWrapper = ftxui::vbox({
        ftxui::hbox({ canvasWithAxes, ftxui::filler() }),
        ftxui::filler()
    }) | ftxui::flex | ftxui::reflect( M_availableBox );


    if ( M_contextWindow->isActive() )
        return ftxui::dbox({ responsiveCanvasWrapper, M_contextWindow->Render() });

    return responsiveCanvasWrapper;


}


void
EditorCanvasComponent::updateViewport()
{
    int availableCols = std::max(0, (M_availableBox.x_max - M_availableBox.x_min - 8));
    int availableRows = std::max(0, (M_availableBox.y_max - M_availableBox.y_min - 2));

    int pixelScale = M_squarePixel ? 2 : 1;

    int maxVisiblePixelsX = availableCols > 0 ? (availableCols / pixelScale) : M_width;
    int maxVisiblePixelsY = availableRows > 0 ? availableRows : M_height;

    M_camera->resize(
            std::min(M_width, maxVisiblePixelsX),
            std::min(M_height, maxVisiblePixelsY)
            );

    M_cells.resize( M_camera->height() );
    for ( auto & row : M_cells )
        row.resize( M_camera->width() * pixelScale, ftxui::text(" ") );
}


void
EditorCanvasComponent::resize( int width, int height )
{
    M_width = width;
    M_height = height;

    int pixelScale = M_squarePixel ? 2 : 1;

    updateViewport();

    saveState();
}




// bool
// EditorCanvasComponent::processPanning( ftxui::Event event )
// {
//     if ( !event.is_mouse() ) return false;
//     auto mouse = event.mouse();
//
//     bool isPanTrigger = ( mouse.button == ftxui::Mouse::Button::Middle ) ||
//                         ( M_currentState.toolType == ToolType::PAN && mouse.button == ftxui::Mouse::Button::Left );
//
//     if ( mouse.motion == ftxui::Mouse::Pressed && !M_isPanning )
//     {
//         if ( isPanTrigger && M_box.Contain(mouse.x, mouse.y) )
//         {
//             TakeFocus();
//             M_isPanning = true;
//             M_cursor->setVisibility(false);
//             M_lastPanMouseX = mouse.x;
//             M_lastPanMouseY = mouse.y;
//             return true;
//         }
//     }
//     else if ( M_isPanning )
//     {
//         if ( mouse.motion == ftxui::Mouse::Released )
//         {
//             M_isPanning = false;
//             return true;
//         }
//
//         if ( mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed )
//         {
//             int dx = (mouse.x - M_lastPanMouseX) / (M_squarePixel ? 2 : 1);
//             int dy = mouse.y - M_lastPanMouseY;
//
//             if ( dx != 0 || dy != 0 )
//             {
//                 int visibleW = std::max(1, (M_box.x_max - M_box.x_min + 1) / (M_squarePixel ? 2 : 1));
//                 int visibleH = std::max(1, (M_box.y_max - M_box.y_min + 1));
//
//                 int maxCameraX = std::max(0, M_width - visibleW);
//                 int maxCameraY = std::max(0, M_height - visibleH);
//
//                 M_cameraX = std::clamp(M_cameraX - dx, 0, maxCameraX);
//                 M_cameraY = std::clamp(M_cameraY - dy, 0, maxCameraY);
//
//                 M_lastPanMouseX += (dx * (M_squarePixel ? 2 : 1));
//                 M_lastPanMouseY += dy;
//             }
//             return true;
//         }
//     }
//
//     return false;
// }




bool
EditorCanvasComponent::processRightClickModal( ftxui::Event event )
{
    if ( !event.is_mouse() )
        return false;

    auto mouse = event.mouse();

    if ( mouse.button == ftxui::Mouse::Button::Right && mouse.motion == ftxui::Mouse::Pressed )
    {
        if ( M_box.Contain( mouse.x, mouse.y ) )
        {
            M_contextWindow->show();

            M_contextWindow->x() = std::clamp(mouse.x - M_box.x_min, 0, M_width * (M_squarePixel ? 2 : 1) );
            M_contextWindow->y() = std::clamp(mouse.y - M_box.y_min, 0, M_height);

            return true;
        }
    }

    return false;
}


void
EditorCanvasComponent::clear()
{
    M_activeLayer->clear();
    saveState();
}



bool
EditorCanvasComponent::OnEvent( ftxui::Event event )
{
    if( M_contextWindow->OnEvent( event ) )
        return true;

    //TODO All this can be refactored into strategy & or vector of tools

    if ( M_currentState.toolType == ToolType::DRAW || M_currentState.toolType == ToolType::ERASER )
    {
        if ( M_brushTool->processKeyboardEvent( event ) )
        {
            saveState();
            return true;
        }
        if ( M_brushTool->processMouseEvent( event ) )
        {
            TakeFocus();
            if ( !M_brushTool->isDrawing() )
                saveState();
            return true;
        }
    }

    if ( M_currentState.toolType == ToolType::CIRCLE || M_currentState.toolType == ToolType::SQUARE || M_currentState.toolType == ToolType::LINE )
    {
        if ( M_shapeTool->processKeyboardEvent( event ) )
        {
            if ( !M_shapeTool->isDrawing() )
                saveState();
            return true;
        }
        if ( M_shapeTool->processMouseEvent( event ) )
        {
            TakeFocus();
            if ( !M_shapeTool->isDrawing() )
                saveState();
            return true;
        }
    }

    if ( M_currentState.toolType == ToolType::EYE_DROPPER )
    {
        if ( M_eyeDropperTool->processKeyboardEvent( event ) )
        {
            M_currentState.toolType = ToolType::DRAW;
            return true;
        }
        if ( M_eyeDropperTool->processMouseEvent( event ) )
        {
            M_currentState.toolType = ToolType::DRAW;
            return true;
        }
    }

    if ( M_currentState.toolType == ToolType::PAINT_FILL )
    {
        if ( M_paintTool->processKeyboardEvent( event ) )
        {
            saveState();
            return true;
        }
        if ( M_paintTool->processMouseEvent( event ) )
        {
            saveState();
            return true;
        }

    }

    if ( M_currentState.toolType == ToolType::BOX_SELECT )
    {
        if ( M_selectionTool->processKeyboardEvent( event ) )
        {
            TakeFocus();
            saveState();
            return true;
        }
        if ( M_selectionTool->processMouseEvent( event ) )
        {
            TakeFocus();
            saveState();
            return true;
        }
        if ( M_cells.size() > 0 && M_selectionTool->processTranslation( event,  M_camera->width(), M_camera->height() ) )
            return true;
    }


    if ( M_cells.size() > 0 && M_cursor->processMovement( event,  M_camera->width(), M_camera->height() ) )
        return true;


    if ( processRightClickModal( event ) )
        return true;

    // if ( processPanning( event ) )
    //     return true;


    return false;
}


std::vector<ftxui::Color>
EditorCanvasComponent::computeColorsInCanvas() const
{
    std::vector<ftxui::Color> usedColors;
    for ( auto const & layer : M_layers )
    {
        if ( !layer || !layer->isVisible() ) continue;
        for ( auto const & row : *layer )
        {
            for ( auto const& cell : row )
            {
                if ( cell.brush == " " ) continue;

                if ( std::find(usedColors.begin(), usedColors.end(), cell.color) == usedColors.end() )
                    usedColors.push_back( cell.color );
            }
        }
    }
    return usedColors;
}


void
EditorCanvasComponent::saveState()
{
    M_spriteHistory.save( *M_activeLayer );
    M_colorsInCanvas = computeColorsInCanvas();
}


std::pair<int,int>
EditorCanvasComponent::screenToWorld(int screenX, int screenY) const
{
    if (!M_box.Contain(screenX, screenY))
        return {-1, -1};

    int worldX = ((screenX - M_box.x_min) / (M_squarePixel ? 2 : 1));
    int worldY = (screenY - M_box.y_min);

    worldX = std::clamp(worldX, 0, M_camera->width() - 1);
    worldY = std::clamp(worldY, 0, M_camera->height() - 1);

    return {worldX, worldY};
}

std::pair<int,int>
EditorCanvasComponent::worldToScreen(int worldX, int worldY) const
{
    int screenX = (worldX * (M_squarePixel ? 2 : 1) ) + M_box.x_min;
    int screenY = worldY + M_box.y_min;
    return {screenX, screenY};
}






std::shared_ptr<EditorCanvasComponent> EditorCanvas( int width, int height, ShortcutManager * shortcutManager )
{
    return std::make_shared<EditorCanvasComponent>( width, height, shortcutManager );
}

}

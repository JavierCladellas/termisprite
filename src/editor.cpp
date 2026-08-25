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
    int visibleW = std::max(1, (M_box.x_max - M_box.x_min) / (M_squarePixel ? 2 : 1));
    int visibleH = std::max(1, (M_box.y_max - M_box.y_min));
    M_cameraX = std::clamp(M_cameraX, 0, std::max(0, M_width - visibleW));
    M_cameraY = std::clamp(M_cameraY, 0, std::max(0, M_height - visibleH));

    M_grid->render(M_cells, M_squarePixel );
    M_sprite.render(M_cells, M_squarePixel );

    M_selectionTool->render(M_cells, M_squarePixel);

    M_cursor->render( M_cells, *M_brushTool, M_squarePixel );

    ftxui::Elements cellsElt( M_height );
    for ( int i = 0; i < M_height; ++i )
        cellsElt[i] = ftxui::hbox(M_cells[i]);

    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    ftxui::Element canvas = ftxui::vbox( cellsElt )
                              | ftxui::bgcolor( M_currentState.backgroundColor )
                              | ftxui::reflect( M_box )
                              | ftxui::borderStyled( borderColor )
                              | ftxui::size( ftxui::WIDTH, ftxui::EQUAL, (M_width * (M_squarePixel ? 2 : 1) ) + 1 )
                              | ftxui::size( ftxui::HEIGHT, ftxui::EQUAL, M_height + 2 );

    //------------Build axis---------
    ftxui::Elements xAxis = {ftxui::text("─")};

    for ( int i = 0; i < (M_width * (M_squarePixel ? 2 : 1) ) + 1; ++i )
        xAxis.push_back(ftxui::text("─"));

    xAxis.push_back(ftxui::text("►"));
    xAxis.push_back(ftxui::text(std::to_string(M_width) + "px"));

    ftxui::Elements yAxis = {ftxui::text(std::to_string(M_height) + "px ▲ ")};
    for (int i = 0; i < M_height+1; ++i )
        yAxis.push_back(ftxui::text("│ ") | ftxui::align_right);

    yAxis.push_back(ftxui::text("─┼─") | ftxui::align_right);

    auto axisColor = ftxui::Color::GrayDark; //TODO: Use palette
    ftxui::Element canvasWithAxes = ftxui::hbox({
        ftxui::vbox(std::move(yAxis)) | ftxui::color(axisColor),
        ftxui::vbox({ canvas, ftxui::hbox(std::move(xAxis)) | ftxui::color(axisColor) })
    });
    //-----------------------------//


    if ( M_showRightClickModal )
    {
        ftxui::Element modal = ftxui::window( ftxui::text(" Options "), M_rightClickModal->Render() )
                             | ftxui::clear_under
                             | ftxui::reflect( M_rightClickModalBox );

        ftxui::Element positionedModal = ftxui::vbox({
            ftxui::text("") | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, M_modalY),
            ftxui::hbox({
                ftxui::text("") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, M_modalX),
                modal
            })
        });

        return ftxui::dbox({ canvasWithAxes, positionedModal });
    }

    return canvasWithAxes;


}








bool
EditorCanvasComponent::processPanning( ftxui::Event event )
{
    if ( !event.is_mouse() ) return false;
    auto mouse = event.mouse();

    bool isPanTrigger = ( mouse.button == ftxui::Mouse::Button::Middle ) ||
                        ( M_currentState.toolType == ToolType::PAN && mouse.button == ftxui::Mouse::Button::Left );

    if ( mouse.motion == ftxui::Mouse::Pressed && !M_isPanning )
    {
        if ( isPanTrigger && M_box.Contain(mouse.x, mouse.y) )
        {
            TakeFocus();
            M_isPanning = true;
            M_cursor->setVisibility(false);
            M_lastPanMouseX = mouse.x;
            M_lastPanMouseY = mouse.y;
            return true;
        }
    }
    else if ( M_isPanning )
    {
        if ( mouse.motion == ftxui::Mouse::Released )
        {
            M_isPanning = false;
            return true;
        }

        if ( mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed )
        {
            int dx = (mouse.x - M_lastPanMouseX) / (M_squarePixel ? 2 : 1);
            int dy = mouse.y - M_lastPanMouseY;

            if ( dx != 0 || dy != 0 )
            {
                int visibleW = std::max(1, (M_box.x_max - M_box.x_min + 1) / (M_squarePixel ? 2 : 1));
                int visibleH = std::max(1, (M_box.y_max - M_box.y_min + 1));

                int maxCameraX = std::max(0, M_width - visibleW);
                int maxCameraY = std::max(0, M_height - visibleH);

                M_cameraX = std::clamp(M_cameraX - dx, 0, maxCameraX);
                M_cameraY = std::clamp(M_cameraY - dy, 0, maxCameraY);

                M_lastPanMouseX += (dx * (M_squarePixel ? 2 : 1));
                M_lastPanMouseY += dy;
            }
            return true;
        }
    }

    return false;
}




bool
EditorCanvasComponent::processShapeDrawing( ftxui::Event event )
{
    if ( M_currentState.toolType != ToolType::SQUARE &&
         M_currentState.toolType != ToolType::CIRCLE &&
         M_currentState.toolType != ToolType::LINE )
        return false;

    if ( event.is_mouse() )
    {
        auto mouse = event.mouse();

        if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Released )
        {
            if ( M_isDrawing )
            {
                M_isDrawing = false;
                saveState();
                return true;
            }
        }

        if ( !M_box.Contain( mouse.x, mouse.y ) )
            return false;

        if ( mouse.button == ftxui::Mouse::Button::Left )
        {
            M_cursor->setVisibility(false);

            int localX = std::clamp((mouse.x - M_box.x_min)/(M_squarePixel ? 2 : 1), 0, M_width - 1);
            int localY = std::clamp(mouse.y - M_box.y_min, 0, M_height - 1);

            if ( mouse.motion == ftxui::Mouse::Pressed )
            {
                TakeFocus();
                M_isDrawing = true;
                M_shapeStartX = localX;
                M_shapeStartY = localY;

                M_spriteSnapshot = M_sprite;
                return true;
            }
            else if ( (mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed) && M_isDrawing )
            {
                M_sprite = M_spriteSnapshot;

                if ( M_currentState.toolType == ToolType::SQUARE )
                    drawSquare( M_shapeStartX, M_shapeStartY, localX, localY );
                else if ( M_currentState.toolType == ToolType::CIRCLE )
                    drawCircle( M_shapeStartX, M_shapeStartY, localX, localY );
                else if ( M_currentState.toolType == ToolType::LINE )
                    Geometry::drawLine(*M_brushTool, M_sprite, M_shapeStartX, M_shapeStartY, localX, localY );

                return true;
            }
        }
        return false;
    }

    if ( M_isDrawing && event == ftxui::Event::Escape )
    {
        M_isDrawing = false;
        M_sprite = M_spriteSnapshot;
        return true;
    }

    if ( event == ftxui::Event::Character(' ') || event == ftxui::Event::Return )
    {
        M_cursor->setVisibility(true);
        if ( !M_isDrawing )
        {
            M_isDrawing = true;
            M_shapeStartX = M_cursor->x();
            M_shapeStartY = M_cursor->y();
            M_spriteSnapshot = M_sprite;

            M_sprite = M_spriteSnapshot;
            if ( M_currentState.toolType == ToolType::SQUARE )
                drawSquare( M_shapeStartX, M_shapeStartY, M_cursor->x(), M_cursor->y() );
            else if ( M_currentState.toolType == ToolType::CIRCLE )
                drawCircle( M_shapeStartX, M_shapeStartY, M_cursor->x(), M_cursor->y() );
            else if ( M_currentState.toolType == ToolType::LINE )
                Geometry::drawLine(*M_brushTool, M_sprite, M_shapeStartX, M_shapeStartY, M_cursor->x(), M_cursor->y() );
        }
        else
        {
            M_isDrawing = false;
            saveState();
        }
        return true;
    }

    if ( M_isDrawing )
    {
        bool moved = false;

        if ( event == ftxui::Event::ArrowUp || event == ftxui::Event::Character('k') )
        {
            M_cursor->y() = std::max(0, M_cursor->y() - 1);
            moved = true;
        }
        else if ( event == ftxui::Event::ArrowDown || event == ftxui::Event::Character('j') )
        {
            M_cursor->y() = std::min(M_height - 1, M_cursor->y() + 1);
            moved = true;
        }
        else if ( event == ftxui::Event::ArrowLeft || event == ftxui::Event::Character('h') )
        {
            M_cursor->x() = std::max(0, M_cursor->x() - 1);
            moved = true;
        }
        else if ( event == ftxui::Event::ArrowRight || event == ftxui::Event::Character('l') )
        {
            M_cursor->x() = std::min(M_width - 1, M_cursor->x() + 1);
            moved = true;
        }

        if ( moved )
        {
            M_cursor->setVisibility(true);
            M_sprite = M_spriteSnapshot;

            if ( M_currentState.toolType == ToolType::SQUARE )
                drawSquare( M_shapeStartX, M_shapeStartY, M_cursor->x(), M_cursor->y() );
            else if ( M_currentState.toolType == ToolType::CIRCLE )
                drawCircle( M_shapeStartX, M_shapeStartY, M_cursor->x(), M_cursor->y() );
            else if ( M_currentState.toolType == ToolType::LINE )
                Geometry::drawLine(*M_brushTool, M_sprite, M_shapeStartX, M_shapeStartY, M_cursor->x(), M_cursor->y() );

            return true;
        }
    }

    return false;
}

void
EditorCanvasComponent::drawSquare( int x0, int y0, int x1, int y1 )
{
    int minX = std::min(x0, x1);
    int maxX = std::max(x0, x1);
    int minY = std::min(y0, y1);
    int maxY = std::max(y0, y1);

    for ( int x = minX; x <= maxX; ++x )
    {
        M_brushTool->apply( x, minY);
        M_brushTool->apply( x, maxY);
    }

    for ( int y = minY; y <= maxY; ++y )
    {
        M_brushTool->apply( minX, y);
        M_brushTool->apply( maxX, y);
    }
}

void
EditorCanvasComponent::drawCircle( int x0, int y0, int x1, int y1 )
{
    float xc = (x0 + x1) / 2.;
    float yc = (y0 + y1) / 2.;
    float a = std::abs(x1 - x0) / 2.;
    float b = std::abs(y1 - y0) / 2.;

    if ( x0 == x1 && y0 == y1 )
    {
        M_brushTool->apply( x0, y0 );
        return;
    }

    int points = std::max(a, b) * 8;
    for ( int i = 0; i <= points; ++i )
    {
        float theta = 2. * M_PI * i / points;

        int x = std::round(xc + a * std::cos(theta));
        int y = std::round(yc + b * std::sin(theta));

        if ( x >= 0 && x < M_width && y >= 0 && y < M_height )
            M_brushTool->apply( x,y );
    }
}


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
            M_showRightClickModal = true;

            M_modalX = std::clamp(mouse.x - M_box.x_min, 0, M_width * (M_squarePixel ? 2 : 1) );
            M_modalY = std::clamp(mouse.y - M_box.y_min, 0, M_height);

            return true;
        }
    }

    return false;
}


void
EditorCanvasComponent::clear()
{
    M_sprite.clear();
    saveState();
}



bool
EditorCanvasComponent::OnEvent( ftxui::Event event )
{

    //TODO: Refactor into a separate method
    if ( M_showRightClickModal )
    {
        bool handled = M_rightClickModal->OnEvent( event );

        bool executeAction = ( event == ftxui::Event::Return || event == ftxui::Event::Character('\n') );

        if ( event.is_mouse() )
        {
            auto mouse = event.mouse();
            if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Pressed )
            {
                if ( M_rightClickModalBox.Contain( mouse.x, mouse.y ) )
                    executeAction = true;
                else
                {
                    M_showRightClickModal = false;
                    return true;
                }
            }
            if ( mouse.button == ftxui::Mouse::Button::Right && mouse.motion == ftxui::Mouse::Pressed )
            {
                if ( !M_rightClickModalBox.Contain( mouse.x, mouse.y ) && M_box.Contain( mouse.x, mouse.y ) )
                {
                    M_showRightClickModal = false;
                    return processRightClickModal(event);
                }
            }
        }

        if ( executeAction )
        {
            switch ( M_rightClickModalIndex )
            {
                //TODO: Use enums
                case 0: if ( onBackgroundChangeRequested ) onBackgroundChangeRequested(); break;
                case 1: M_grid->toggle(); break;
                case 2: M_grid->switchType(); break;
                case 3: this->undo(); break;
                case 4: this->redo(); break;
                case 5: this->clear(); break;
                default: break;
            }
            M_showRightClickModal = false;
            M_rightClickModalIndex = 0;
            return true;
        }

        if ( event == ftxui::Event::Escape )
        {
            M_showRightClickModal = false;
            return true;
        }

        return true;
    }


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

    if ( processShapeDrawing( event ) )
        return true;

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
        if ( M_selectionTool->processTranslation( event ) )
            return true;
    }


    if ( M_cursor->processMovement( event ) )
        return true;


    if ( event.is_mouse() )
    {

        if ( processRightClickModal( event ) )
            return true;

        if ( processPanning( event ) )
            return true;

    }

    return false;
}


std::vector<ftxui::Color>
EditorCanvasComponent::computeColorsInCanvas() const
{
    std::vector<ftxui::Color> usedColors;
    for ( auto const & row : M_sprite )
        for ( auto const& cell : row )
        {
            if ( cell.brush == " " ) continue;

            if ( std::find(usedColors.begin(), usedColors.end(), cell.color) == usedColors.end() ) 
                usedColors.push_back( cell.color );
        }
    return usedColors;
}


void
EditorCanvasComponent::saveState()
{
    M_spriteHistory.save( M_sprite );
    M_colorsInCanvas = computeColorsInCanvas();
}


std::pair<int,int>
EditorCanvasComponent::screenToWorld(int screenX, int screenY) const
{
    if (!M_box.Contain(screenX, screenY))
        return {-1, -1};

    int worldX = ((screenX - M_box.x_min) / (M_squarePixel ? 2 : 1)) + M_cameraX;
    int worldY = (screenY - M_box.y_min) + M_cameraY;

    worldX = std::clamp(worldX, 0, M_width - 1);
    worldY = std::clamp(worldY, 0, M_height - 1);

    return {worldX, worldY};
}

std::pair<int,int>
EditorCanvasComponent::worldToScreen(int worldX, int worldY) const
{
    int screenX = ((worldX - M_cameraX) * (M_squarePixel ? 2 : 1) ) + M_box.x_min;
    int screenY = (worldY - M_cameraY) + M_box.y_min;
    return {screenX, screenY};
}






std::shared_ptr<EditorCanvasComponent> EditorCanvas( int width, int height )
{
    return std::make_shared<EditorCanvasComponent>( width, height );
}

}

#include "editor.hpp"
#include "sprite_io.hpp"
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

    for ( int sy = 0; sy < M_height; ++sy )
    {
        for ( int sx = 0; sx < M_width; ++sx )
        {
            int worldX = sx + M_cameraX;
            int worldY = sy + M_cameraY;


            bool isOutOfBounds = (worldX < 0 || worldX >= M_width || worldY < 0 || worldY >= M_height);
            if ( isOutOfBounds )
            {
                M_cells[sy][sx] = ftxui::text( M_squarePixel ? "  " : " " ) | ftxui::bgcolor( M_currentState.backgroundColor );
                continue;
            }


            Pixel const& cellContent = M_sprite.at(worldX, worldY);
    
            std::string brush = cellContent.brush; 
            if ( cellContent.brush == " " )
                continue;

            if ( M_squarePixel )
                brush += brush;

            ftxui::Element cell =  ftxui::text(brush);
            cell |= ftxui::color( cellContent.color );
            cell |= ftxui::bgcolor( M_currentState.backgroundColor );
            M_cells[sy][sx] = cell;
        }
    }
    M_selectionTool->render(M_cells, M_squarePixel);

    M_cursor->render( M_cells, M_currentState, M_squarePixel );

    ftxui::Elements cellsElt( M_height );
    for ( int i = 0; i < M_height; ++i )
        cellsElt[i] = ftxui::hbox(M_cells[i]);

    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    ftxui::Element canvas = ftxui::vbox( cellsElt )
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

void
EditorCanvasComponent::importImage( std::string const& filepath, int targetWidth, int targetHeight, std::string const& format )
{
    int newWidth = (targetWidth > 0) ? targetWidth : M_width;
    int newHeight = (targetHeight > 0) ? targetHeight : M_height;
    this->resize(newWidth, newHeight);
    if ( SpriteImporter::importImage( filepath, M_sprite, newWidth, newHeight, format) )
        saveState();
}

void
EditorCanvasComponent::importProject( std::string const& filepath, std::unordered_map<std::string, std::vector<ftxui::Color>> & palettes )
{
    if ( SpriteImporter::importProject( filepath, M_sprite, M_currentState, palettes ) )
    {
        auto [width, height] = M_sprite.size();
        M_width = width;
        M_height = height;
        this->resize(width, height);
        saveState();
    }
}

void
EditorCanvasComponent::exportProject( std::string const& filepath, std::string const& projectName, std::unordered_map<std::string, std::vector<ftxui::Color>> const& palettes )
{
    SpriteExporter::exportProject( filepath, projectName, M_sprite, M_currentState, palettes );
}

void
EditorCanvasComponent::exportImage( std::string const& filepath, std::string const& format )
{
    SpriteExporter::exportImage( filepath, M_sprite, format );
}


bool
EditorCanvasComponent::processKeyboardDrawing( ftxui::Event event )
{
    if ( M_currentState.toolType != ToolType::DRAW && M_currentState.toolType != ToolType::ERASER )
        return false;

    if ( event == ftxui::Event::Escape || event == ftxui::Event::Return )
    {
        if ( M_selectionTool->isActive() )
        {
            endTranslation();
            M_selectionTool->setActive(false);
            saveState();
            return true;
        }
    }

    if ( event == ftxui::Event::Character( ' ' ) || event == ftxui::Event::Return )
    {
        applyBrushAt( M_cursor->x(), M_cursor->y() );
        M_cursor->setVisibility(true);
        saveState();
        return true;
    }
    if ( event == ftxui::Event::Backspace || event == ftxui::Event::Delete )
    {
        applyBrushAt( M_cursor->x(), M_cursor->y() );
        M_cursor->setVisibility(true);
        saveState();
        return true;
    }

    return false;
}


void
EditorCanvasComponent::copyToClipboard()
{
    int w = M_selectionTool->width();
    int h = M_selectionTool->height();

    M_currentState.clipboard.data.assign(h, std::vector<Pixel>(w));

    for ( int y = 0; y < h; ++y )
        for ( int x = 0; x < w; ++x )
            M_currentState.clipboard.data[y][x] = M_sprite.at(M_selectionTool->minX() + x,M_selectionTool->minY() + y);

    M_currentState.clipboard.hasData = true;
}

void
EditorCanvasComponent::cutToClipboard()
{
    int w = M_selectionTool->width();
    int h = M_selectionTool->height();

    M_currentState.clipboard.data.assign(h, std::vector<Pixel>(w));

    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            M_currentState.clipboard.data[y][x] = M_sprite.at(M_selectionTool->minX() + x,M_selectionTool->minY() + y);
            M_sprite.at(M_selectionTool->minX() + x,M_selectionTool->minY() + y) = Pixel{" ", ftxui::Color::White};
        }
    }

    M_currentState.clipboard.hasData = true;
    M_selectionTool->setActive(false);
    saveState();
}


void
EditorCanvasComponent::pasteClipboard()
{
    int h = M_currentState.clipboard.data.size();
    int w = (h > 0) ? M_currentState.clipboard.data[0].size() : 0;

    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            int targetY = M_cursor->y() + y;
            int targetX = M_cursor->x() + x;

            if ( targetY >= 0 && targetY < M_height && targetX >= 0 && targetX < M_width )
                M_sprite.at(targetX,targetY) = M_currentState.clipboard.data[y][x];
        }
    }

    saveState();
}

void
EditorCanvasComponent::applyBrushAt( int targetX, int targetY, bool isEraser )
{
    int bsize = M_currentState.brushSize;
    int startX = targetX;
    int startY = targetY;
    for ( int by = 0; by < bsize; ++by )
    {
        for ( int bx = 0; bx < bsize; ++bx )
        {
            int px = startX + bx;
            int py = startY + by;

            if ( px >= 0 && px < M_width && py >= 0 && py < M_height )
            {
                Pixel & cell = M_sprite.at(px, py);
                if ( isEraser )
                {
                    cell.brush = " ";
                    cell.color = ftxui::Color::White;
                }
                else
                {
                    cell.brush = M_currentState.brush;
                    cell.color = M_currentState.color;
                }
            }
        }
    }
}

bool
EditorCanvasComponent::processCursorMovement( ftxui::Event event )
{
    int dx = 0, dy = 0;

    if ( event == ftxui::Event::ArrowUp || event == ftxui::Event::Character( 'k' ) ) dy = -1;
    else if ( event == ftxui::Event::ArrowDown || event == ftxui::Event::Character( 'j' ) ) dy = 1;
    else if ( event == ftxui::Event::ArrowLeft || event == ftxui::Event::Character( 'h' ) ) dx = -1;
    else if ( event == ftxui::Event::ArrowRight || event == ftxui::Event::Character( 'l' ) ) dx = 1;

    if ( dx != 0 || dy != 0 )
    {
        if ( M_selectionTool->isActive() )
        {
            translateSelection( dx, dy );
            return true;
        }
        else
        {
            M_cursor->x() = std::clamp( M_cursor->x() + dx, 0, M_width - 1 );
            M_cursor->y() = std::clamp( M_cursor->y() + dy, 0, M_height - 1 );
            M_cursor->setVisibility(true);
            return true;
        }
    }
    return false;
}

bool
EditorCanvasComponent::processMouseDrawing( ftxui::Event event )
{

    if ( M_currentState.toolType != ToolType::DRAW && M_currentState.toolType != ToolType::ERASER )
        return false;

    if ( !event.is_mouse() )
        return false;

    auto mouse = event.mouse();

    if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Released )
    {
        if ( M_isDrawing )
        {
            M_isDrawing = false;
            saveState();
        }
    }

    if ( !M_box.Contain( mouse.x, mouse.y ) )
        return false;

    if ( mouse.button == ftxui::Mouse::Button::Left )
    {
        M_cursor->setVisibility(false);

        auto [worldX, worldY ] = screenToWorld(mouse.x, mouse.y);

        // Clamp to sprite bounds so we don't draw outside the canvas
        worldX = std::clamp(worldX, 0, M_width - 1);
        worldY = std::clamp(worldY, 0, M_height - 1);


        if ( mouse.motion == ftxui::Mouse::Pressed )
        {
            TakeFocus();
            M_isDrawing = true;
            M_lastDrawX = worldX;
            M_lastDrawY = worldY;

            applyBrushAt(worldX,worldY);
            return true;
        }
        else if ( (mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed) && M_isDrawing )
        {
            drawLine( M_lastDrawX, M_lastDrawY, worldX, worldY );
            M_lastDrawX = worldX;
            M_lastDrawY = worldY;
            return true;
        }
    }

    return false;
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
EditorCanvasComponent::processEyeDropper( ftxui::Event event )
{
    if ( M_currentState.toolType != ToolType::EYE_DROPPER )
        return false;

    if ( event == ftxui::Event::Character(' ') || event == ftxui::Event::Return )
    {
        Pixel & cell = M_sprite.at(M_cursor->x(),M_cursor->y());

        M_currentState.brush = cell.brush;
        M_currentState.color = cell.color;

        M_currentState.toolType = ToolType::DRAW;

        return true;
    }

    if ( !event.is_mouse() )
        return false;

    auto mouse = event.mouse();


    if ( !M_box.Contain( mouse.x, mouse.y ) )
        return false;

    if ( mouse.button != ftxui::Mouse::Button::Left)
        return false;
    if ( mouse.motion != ftxui::Mouse::Pressed )
        return false;

    auto [localX, localY] = screenToWorld(mouse.x, mouse.y);

    localX = std::clamp(localX, 0, M_width - 1);
    localY = std::clamp(localY, 0, M_height - 1);

    Pixel & cell = M_sprite.at(localX, localY);
    if ( cell.brush != " " )
    {
        M_currentState.brush = cell.brush;
        M_currentState.color = cell.color;
    }
    M_currentState.toolType = ToolType::DRAW;
    return true;

}

bool
EditorCanvasComponent::processPaintFill( ftxui::Event event )
{
    if ( M_currentState.toolType != ToolType::PAINT_FILL )
        return false;

    if ( event == ftxui::Event::Character(' ') || event == ftxui::Event::Return )
    {
        floodFillPaint(M_cursor->x(), M_cursor->y());
        saveState();
        return true;
    }

    if ( !event.is_mouse() )
        return false;

    auto mouse = event.mouse();

    if ( !M_box.Contain( mouse.x, mouse.y ) )
        return false;

    if ( mouse.button != ftxui::Mouse::Button::Left)
        return false;
    if ( mouse.motion != ftxui::Mouse::Pressed )
        return false;

    auto [localX, localY] = screenToWorld(mouse.x, mouse.y);

    localX = std::clamp(localX, 0, M_width - 1);
    localY = std::clamp(localY, 0, M_height - 1);

    floodFillPaint( localX, localY );
    saveState();
    return true;
}

bool
EditorCanvasComponent::processBoxSelection( ftxui::Event event )
{
    if ( M_currentState.toolType != ToolType::BOX_SELECT )
        return false;

    if ( event.is_mouse() )
    {
        auto mouse = event.mouse();

        if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Released )
        {
            if ( M_isDrawing )
            {
                M_isDrawing = false;
                return true;
            }
        }

        if ( !M_box.Contain( mouse.x, mouse.y ) )
            return false;

        if ( mouse.button == ftxui::Mouse::Button::Left )
        {
            M_cursor->setVisibility(false);

            auto [localX, localY] = screenToWorld(mouse.x, mouse.y);

            localX = std::clamp(localX, 0, M_width - 1);
            localY = std::clamp(localY, 0, M_height - 1);

            if ( mouse.motion == ftxui::Mouse::Pressed )
            {
                TakeFocus();
                M_isDrawing = true;
                M_selectionTool->setActive(false);

                M_selectionTool->setStart( localX, localY );
                M_selectionTool->setEnd( localX, localY );
                return true;
            }
            else if ( (mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed) && M_isDrawing )
            {
                M_selectionTool->setEnd( localX, localY );
                return true;
            }
        }

        if ( mouse.button == ftxui::Mouse::Button::Right )
        {
            auto [localX, localY] = screenToWorld(mouse.x, mouse.y);
            if ( mouse.motion == ftxui::Mouse::Pressed )
            {
                if ( M_selectionTool->isActive() )
                {
                    if ( localX >= M_selectionTool->minX() && localX <= M_selectionTool->maxX() &&
                         localY >= M_selectionTool->minY() && localY <= M_selectionTool->maxY() )
                    {
                        beginTranslation();
                        M_lastDragX = localX;
                        M_lastDragY = localY;
                        return true;
                    }
                    else
                    {
                        endTranslation();
                        M_selectionTool->setActive(false);
                        saveState();
                        return true;
                    }
                }
            }
            else if ( (mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed) && M_isTranslating )
            {
                int dx = localX - M_lastDragX;
                int dy = localY - M_lastDragY;

                if ( dx != 0 || dy != 0 )
                {
                    if ( translateSelection( dx, dy ) )
                    {
                        M_lastDragX = localX;
                        M_lastDragY = localY;
                    }
                }
                return true;
            }
            else if ( mouse.motion == ftxui::Mouse::Released && M_isTranslating )
            {
                endTranslation();
                return true;
            }
        }

        return false;
    }


    if ( M_isDrawing && event == ftxui::Event::Escape )
    {
        M_isDrawing = false;
        M_selectionTool->setActive(false);
        return true;
    }

    if ( !M_isDrawing &&  M_selectionTool->isActive() && 
         (event == ftxui::Event::Escape || event == ftxui::Event::Return) )
    {
        endTranslation();
        M_selectionTool->setActive(false);
        saveState();
        return true;
    }

    if ( event == ftxui::Event::Character(' ') || event == ftxui::Event::Return )
    {
        M_cursor->setVisibility(true);
        TakeFocus();

        if ( !M_isDrawing )
        {
            M_isDrawing = true;
            M_selectionTool->setActive(false);

            M_selectionTool->setStart(M_cursor->x(), M_cursor->y());
            M_selectionTool->setEnd(M_cursor->x(), M_cursor->y());
        }
        else
        {
            // Finish the box shape
            M_isDrawing = false;
        }
        return true;
    }

    if ( M_isDrawing )
    {
        bool wasActive = M_selectionTool->isActive();
        M_selectionTool->setActive(false);

        bool moved = processCursorMovement(event);

        M_selectionTool->setActive(wasActive);

        if ( moved )
        {
            M_cursor->setVisibility(true);
            M_selectionTool->setEnd(M_cursor->x(), M_cursor->y());
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
                    drawLine( M_shapeStartX, M_shapeStartY, localX, localY );

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
                drawLine( M_shapeStartX, M_shapeStartY, M_cursor->x(), M_cursor->y() );
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
                drawLine( M_shapeStartX, M_shapeStartY, M_cursor->x(), M_cursor->y() );

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
        applyBrushAt(x, minY);
        applyBrushAt(x, maxY);
    }

    for ( int y = minY; y <= maxY; ++y )
    {
        applyBrushAt(minX, y);
        applyBrushAt(maxX, y);
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
        M_sprite.at(x0, y0) = Pixel{M_currentState.brush, M_currentState.color};
        return;
    }

    int points = std::max(a, b) * 8;
    for ( int i = 0; i <= points; ++i )
    {
        float theta = 2. * M_PI * i / points;

        int x = std::round(xc + a * std::cos(theta));
        int y = std::round(yc + b * std::sin(theta));

        if ( x >= 0 && x < M_width && y >= 0 && y < M_height )
            applyBrushAt(x,y);
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

void
EditorCanvasComponent::deleteSelection()
{
    int w = M_selectionTool->width();
    int h = M_selectionTool->height();

    for ( int y = 0; y < h; ++y )
        for ( int x = 0; x < w; ++x )
            M_sprite.at(M_selectionTool->minX() + x,M_selectionTool->minY() + y) = Pixel{" ", ftxui::Color::White};

    M_selectionTool->setActive(false);
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



    if ( processKeyboardDrawing( event ) )
        return true;

    if ( processShapeDrawing( event ) )
        return true;

    if ( processEyeDropper( event ) )
        return true;

    if ( processPaintFill( event ) )
        return true;

    if ( processBoxSelection( event ) )
        return true;

    if ( processCursorMovement( event ) )
        return true;

    if ( event.is_mouse() )
    {
        if ( processMouseDrawing( event ) )
            return true;

        if ( processRightClickModal( event ) )
            return true;

        if ( processPanning( event ) )
            return true;

    }

    return false;
}


std::vector<ftxui::Color>
EditorCanvasComponent::palette() const
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
    M_currentState.palette = palette();
}


std::pair<int,int>
EditorCanvasComponent::screenToWorld(int screenX, int screenY) const
{
    int worldX = ((screenX - M_box.x_min) / (M_squarePixel ? 2 : 1)) + M_cameraX;
    int worldY = (screenY - M_box.y_min) + M_cameraY;
    return {worldX, worldY};
}

std::pair<int,int>
 EditorCanvasComponent::worldToScreen(int worldX, int worldY) const
{
    int screenX = ((worldX - M_cameraX) * (M_squarePixel ? 2 : 1) ) + M_box.x_min;
    int screenY = (worldY - M_cameraY) + M_box.y_min;
    return {screenX, screenY};
}

void
EditorCanvasComponent::drawLine( int x0, int y0, int x1, int y1 )
{
    int dx = std::abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true)
    {
        applyBrushAt(x0, y0);

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void
EditorCanvasComponent::floodFillPaint( int x, int y )
{
    if ( x < 0 || x >= M_width || y < 0 || y >= M_height )
        return;

    Pixel & targetCell = M_sprite.at(x, y);
    Pixel targetPixel = targetCell;

    if ( targetPixel.brush == M_currentState.brush && targetPixel.color == M_currentState.color )
        return;

    std::stack<std::pair<int, int>> stack;
    stack.push({x, y});

    while (!stack.empty())
    {
        auto [cx, cy] = stack.top();
        stack.pop();

        if ( cx < 0 || cx >= M_width || cy < 0 || cy >= M_height )
            continue;

        Pixel & currentCell = M_sprite.at(cx, cy);

        if ( currentCell.brush != targetPixel.brush || currentCell.color != targetPixel.color )
            continue;

        currentCell.brush = M_currentState.brush;
        currentCell.color = M_currentState.color;

        stack.push({cx + 1, cy});
        stack.push({cx - 1, cy});
        stack.push({cx, cy + 1});
        stack.push({cx, cy - 1});
    }
}

void
EditorCanvasComponent::beginTranslation()
{
    if ( M_isTranslating || !M_selectionTool->isActive() ) return;

    M_isTranslating = true;
    M_spriteSnapshot = M_sprite;

    int w = M_selectionTool->width();
    int h = M_selectionTool->height();
    M_currentState.floatingSelection.assign(h, std::vector<Pixel>(w));

    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            M_currentState.floatingSelection[y][x] = M_spriteSnapshot.at(M_selectionTool->minX() + x,M_selectionTool->minY() + y);
            M_spriteSnapshot.at(M_selectionTool->minX() + x,M_selectionTool->minY() + y) = Pixel{" ", ftxui::Color::White};
        }
    }
}

void
EditorCanvasComponent::endTranslation()
{
    if ( !M_isTranslating ) return;
    M_isTranslating = false;
    M_currentState.floatingSelection.clear();
    saveState();
}

bool
EditorCanvasComponent::translateSelection( int dx, int dy )
{
    if ( !M_selectionTool->isActive() ) return false;

    if ( !M_isTranslating ) beginTranslation();

    int minX = M_selectionTool->minX();
    int minY = M_selectionTool->minY();
    int maxX = M_selectionTool->maxX();
    int maxY = M_selectionTool->maxY();

    if ( minX + dx < 0 || maxX + dx >= M_width || minY + dy < 0 || maxY + dy >= M_height )
        return false;

    M_selectionTool->setStart( M_selectionTool->startX() + dx, M_selectionTool->startY() + dy);
    M_selectionTool->setEnd( M_selectionTool->endX() + dx, M_selectionTool->endY() + dy);

    M_sprite = M_spriteSnapshot;

    int w = M_selectionTool->width();
    int h = M_selectionTool->height();
    for ( int y = 0; y < h; ++y )
        for ( int x = 0; x < w; ++x )
            M_sprite.at(M_selectionTool->minX() + x,M_selectionTool->minY() + y) = M_currentState.floatingSelection[y][x];

    return true;
}


std::shared_ptr<EditorCanvasComponent> EditorCanvas( int width, int height )
{
    return std::make_shared<EditorCanvasComponent>( width, height );
}

}

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
    int visibleW = std::max(1, (M_box.x_max - M_box.x_min) / 2);
    int visibleH = std::max(1, (M_box.y_max - M_box.y_min));
    M_cameraX = std::clamp(M_cameraX, 0, std::max(0, M_width - visibleW));
    M_cameraY = std::clamp(M_cameraY, 0, std::max(0, M_height - visibleH));

    ftxui::Elements rows;

    for ( int sy = 0; sy < M_height; ++sy )
    {
        ftxui::Elements row;
        for ( int sx = 0; sx < M_width; ++sx )
        {
            int worldX = sx + M_cameraX;
            int worldY = sy + M_cameraY;

            std::string renderBrushL = " ";
            std::string renderBrushR = " ";
            ftxui::Color renderColor = ftxui::Color::White;

            bool isOutOfBounds = (worldX < 0 || worldX >= M_width || worldY < 0 || worldY >= M_height);

            // Only grab from M_sprite if it actually exists in the array
            if ( !isOutOfBounds )
            {
                Pixel const& cellContent = M_sprite.at(worldX, worldY);
                renderBrushL = cellContent.brush;
                renderBrushR = cellContent.brush;
                renderColor = cellContent.color;

                if ( renderBrushL == " " && M_showPointGrid)
                {
                    if ( worldX % 2 == 0 && worldY % 1 == 0 )
                    {
                        renderBrushL = "·";
                        renderBrushR = " ";
                        renderColor = ftxui::Color::GrayDark;
                    }
                }

                if ( M_currentState.selection.isActive &&
                     worldX >= M_currentState.selection.minX() && worldX <= M_currentState.selection.maxX() &&
                     worldY >= M_currentState.selection.minY() && worldY <= M_currentState.selection.maxY() )
                {
                    bool isTop = (worldY == M_currentState.selection.minY());
                    bool isBottom = (worldY == M_currentState.selection.maxY());
                    bool isLeft = (worldX == M_currentState.selection.minX());
                    bool isRight = (worldX == M_currentState.selection.maxX());

                    // Adjusting borders for 2x1 aspect ratio
                    if (isTop && isLeft && isBottom && isRight) { renderBrushL = "⡏"; renderBrushR = "⢹"; } 
                    else if (isTop && isLeft) { renderBrushL = "⡏"; renderBrushR = "⠉"; }
                    else if (isTop && isRight) { renderBrushL = "⠉"; renderBrushR = "⢹"; }
                    else if (isBottom && isLeft) { renderBrushL = "⣇"; renderBrushR = "⣀"; }
                    else if (isBottom && isRight) { renderBrushL = "⣀"; renderBrushR = "⣸"; }
                    else if (isTop) { renderBrushL = "⠉"; renderBrushR = "⠉"; }
                    else if (isBottom) { renderBrushL = "⣀"; renderBrushR = "⣀"; }
                    else if (isLeft) { renderBrushL = "⡇"; renderBrushR = " "; }
                    else if (isRight) { renderBrushL = " "; renderBrushR = "⢸"; }

                    if ( isTop || isBottom || isLeft || isRight )
                        renderColor = ftxui::Color::White;
                }
            }

            ftxui::Element cellL = ftxui::text( renderBrushL ) | ftxui::color( renderColor );
            ftxui::Element cellR = ftxui::text( renderBrushR ) | ftxui::color( renderColor );

            if ( renderBrushL == " " && M_showCheckerboardGrid )
            {
                if ( (worldX + worldY) % 2 == 0 )
                {
                    cellL |= ftxui::bgcolor( ftxui::Color::GrayDark );
                    cellR |= ftxui::bgcolor( ftxui::Color::GrayDark );
                }
                else
                {
                    cellL |= ftxui::bgcolor( ftxui::Color::Black );
                    cellR |= ftxui::bgcolor( ftxui::Color::Black );
                }
            }
            else
            {
                cellL |= ftxui::bgcolor( M_currentState.backgroundColor );
                cellR |= ftxui::bgcolor( M_currentState.backgroundColor );
            }

            if ( !isOutOfBounds && M_showCursor && worldX == M_cursorX && worldY == M_cursorY )
            {
                cellL = ftxui::text( M_currentState.brush ) | ftxui::color( M_currentState.color ) | ftxui::bgcolor( ftxui::Color::Red ) | ftxui::blink;
                cellR = ftxui::text( M_currentState.brush ) | ftxui::color( M_currentState.color ) | ftxui::bgcolor( ftxui::Color::Red ) | ftxui::blink;
            }

            row.push_back( cellL );
            row.push_back( cellR );
        }
        rows.push_back( ftxui::hbox( row ) );
    }

    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    ftxui::Element canvas = ftxui::vbox( std::move( rows ) )
                              | ftxui::reflect( M_box )
                              | ftxui::borderStyled( borderColor )
                              | ftxui::size( ftxui::WIDTH, ftxui::EQUAL, (M_width * 2) + 1 )
                              | ftxui::size( ftxui::HEIGHT, ftxui::EQUAL, M_height + 2 );

    //------------Build axis---------
    ftxui::Elements xAxis = {ftxui::text("─")};

    for ( int i = 0; i < (M_width * 2) + 1; ++i )
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
EditorCanvasComponent::importImage( std::string const& filepath, int targetWidth, int targetHeight)
{
    int newWidth = (targetWidth > 0) ? targetWidth : M_width;
    int newHeight = (targetHeight > 0) ? targetHeight : M_height;
    this->resize(newWidth, newHeight);
    if ( SpriteImporter::importImage( filepath, M_sprite, newWidth, newHeight ) )
        saveState();
}

void
EditorCanvasComponent::importProject( std::string const& filepath )
{
    if ( SpriteImporter::importProject( filepath, M_sprite, M_currentState ) )
    {
        auto [width, height] = M_sprite.size();
        M_width = width;
        M_height = height;
        saveState();
    }
}

void
EditorCanvasComponent::exportProject( std::string const& filepath, std::string const& projectName )
{
    SpriteExporter::exportProject( filepath, projectName, M_sprite, M_currentState );
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
        if ( M_currentState.selection.isActive )
        {
            endTranslation();
            M_currentState.selection.isActive = false;
            saveState();
            return true;
        }
    }

    if ( event == ftxui::Event::Character( ' ' ) || event == ftxui::Event::Return )
    {
        Pixel & cell = M_sprite.at(M_cursorX,M_cursorY);
        cell.brush = M_currentState.brush;
        cell.color = M_currentState.color;
        M_showCursor = true;
        saveState();
        return true;
    }
    if ( event == ftxui::Event::Backspace || event == ftxui::Event::Delete )
    {
        Pixel & cell = M_sprite.at(M_cursorX,M_cursorY);
        cell.brush = " ";
        cell.color = ftxui::Color::White;
        M_showCursor = true;
        saveState();
        return true;
    }

    return false;
}


void
EditorCanvasComponent::copyToClipboard()
{
    int w = M_currentState.selection.width();
    int h = M_currentState.selection.height();

    M_currentState.clipboard.data.assign(h, std::vector<Pixel>(w));

    for ( int y = 0; y < h; ++y )
        for ( int x = 0; x < w; ++x )
            M_currentState.clipboard.data[y][x] = M_sprite.at(M_currentState.selection.minX() + x,M_currentState.selection.minY() + y);

    M_currentState.clipboard.hasData = true;
}

void
EditorCanvasComponent::cutToClipboard()
{
    int w = M_currentState.selection.width();
    int h = M_currentState.selection.height();

    M_currentState.clipboard.data.assign(h, std::vector<Pixel>(w));

    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            M_currentState.clipboard.data[y][x] = M_sprite.at(M_currentState.selection.minX() + x,M_currentState.selection.minY() + y);
            M_sprite.at(M_currentState.selection.minX() + x,M_currentState.selection.minY() + y) = Pixel{" ", ftxui::Color::White};
        }
    }

    M_currentState.clipboard.hasData = true;
    M_currentState.selection.isActive = false;
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
            int targetY = M_cursorY + y;
            int targetX = M_cursorX + x;

            if ( targetY >= 0 && targetY < M_height && targetX >= 0 && targetX < M_width )
                M_sprite.at(targetX,targetY) = M_currentState.clipboard.data[y][x];
        }
    }

    saveState();
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
        if ( M_currentState.selection.isActive )
        {
            translateSelection( dx, dy );
            return true;
        }
        else
        {
            M_cursorX = std::clamp( M_cursorX + dx, 0, M_width - 1 );
            M_cursorY = std::clamp( M_cursorY + dy, 0, M_height - 1 );
            M_showCursor = true;
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
        M_showCursor = false;

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
            Pixel & cell = M_sprite.at(worldX, worldY);
            cell.brush = M_currentState.brush;
            cell.color = M_currentState.color;
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
            M_showCursor = false;
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
            int dx = (mouse.x - M_lastPanMouseX) / 2;
            int dy = mouse.y - M_lastPanMouseY;

            if ( dx != 0 || dy != 0 )
            {
                int visibleW = std::max(1, (M_box.x_max - M_box.x_min + 1) / 2);
                int visibleH = std::max(1, (M_box.y_max - M_box.y_min + 1));

                int maxCameraX = std::max(0, M_width - visibleW);
                int maxCameraY = std::max(0, M_height - visibleH);

                M_cameraX = std::clamp(M_cameraX - dx, 0, maxCameraX);
                M_cameraY = std::clamp(M_cameraY - dy, 0, maxCameraY);

                M_lastPanMouseX += (dx * 2);
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
        Pixel & cell = M_sprite.at(M_cursorX,M_cursorY);

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

    if ( !event.is_mouse() )
        return false;

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
        M_showCursor = false;

        auto [localX, localY] = screenToWorld(mouse.x, mouse.y);

        localX = std::clamp(localX, 0, M_width - 1);
        localY = std::clamp(localY, 0, M_height - 1);

        if ( mouse.motion == ftxui::Mouse::Pressed )
        {
            TakeFocus();
            M_isDrawing = true;
            M_currentState.selection.isActive = true;

            M_currentState.selection.startX = localX;
            M_currentState.selection.startY = localY;
            M_currentState.selection.endX = localX;
            M_currentState.selection.endY = localY;
            return true;
        }
        else if ( (mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed) && M_isDrawing )
        {
            M_currentState.selection.endX = localX;
            M_currentState.selection.endY = localY;
            return true;
        }
    }

    if ( mouse.button == ftxui::Mouse::Button::Right )
    {

        auto [localX, localY] = screenToWorld(mouse.x, mouse.y);
        if ( mouse.motion == ftxui::Mouse::Pressed )
        {
            if ( M_currentState.selection.isActive )
            {
                if ( localX >= M_currentState.selection.minX() && localX <= M_currentState.selection.maxX() &&
                     localY >= M_currentState.selection.minY() && localY <= M_currentState.selection.maxY() )
                {
                    beginTranslation();
                    M_lastDragX = localX;
                    M_lastDragY = localY;
                    return true;
                }
                else
                {
                    endTranslation();
                    M_currentState.selection.isActive = false;
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

bool
EditorCanvasComponent::processShapeDrawing( ftxui::Event event )
{
    if ( M_currentState.toolType != ToolType::SQUARE &&
         M_currentState.toolType != ToolType::CIRCLE &&
         M_currentState.toolType != ToolType::LINE )
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
            return true;
        }
    }

    if ( !M_box.Contain( mouse.x, mouse.y ) )
        return false;

    if ( mouse.button == ftxui::Mouse::Button::Left )
    {
        M_showCursor = false;

        int localX = std::clamp((mouse.x - M_box.x_min)/2, 0, M_width - 1);
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

void
EditorCanvasComponent::drawSquare( int x0, int y0, int x1, int y1 )
{
    int minX = std::min(x0, x1);
    int maxX = std::max(x0, x1);
    int minY = std::min(y0, y1);
    int maxY = std::max(y0, y1);

    for ( int x = minX; x <= maxX; ++x )
    {
        M_sprite.at(x, minY) = Pixel{M_currentState.brush, M_currentState.color};
        M_sprite.at(x, maxY) = Pixel{M_currentState.brush, M_currentState.color};
    }

    for ( int y = minY; y <= maxY; ++y )
    {
        M_sprite.at(minX, y) = Pixel{M_currentState.brush, M_currentState.color};
        M_sprite.at(maxX, y) = Pixel{M_currentState.brush, M_currentState.color};
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
            M_sprite.at(x, y) = Pixel{M_currentState.brush, M_currentState.color};
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

            M_modalX = std::clamp(mouse.x - M_box.x_min, 0, M_width * 2);
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
    int w = M_currentState.selection.width();
    int h = M_currentState.selection.height();

    for ( int y = 0; y < h; ++y )
        for ( int x = 0; x < w; ++x )
            M_sprite.at(M_currentState.selection.minX() + x,M_currentState.selection.minY() + y) = Pixel{" ", ftxui::Color::White};

    M_currentState.selection.isActive = false;
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
                case 1: toggleGrid(); break;
                case 2: toggleCheckerboardGrid(); break;
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


    if ( processCursorMovement( event ) )
        return true;

    if ( processKeyboardDrawing( event ) )
        return true;

    if ( processEyeDropper( event ) )
        return true;

    if ( event.is_mouse() )
    {
        if ( processMouseDrawing( event ) )
            return true;

        if ( processShapeDrawing( event ) )
            return true;


        if ( processPaintFill( event ) )
            return true;

        if ( processBoxSelection( event ) )
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
    int worldX = ((screenX - M_box.x_min) / 2) + M_cameraX;
    int worldY = (screenY - M_box.y_min) + M_cameraY;
    return {worldX, worldY};
}

std::pair<int,int>
 EditorCanvasComponent::worldToScreen(int worldX, int worldY) const
{
    int screenX = ((worldX - M_cameraX) * 2) + M_box.x_min;
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
        Pixel & cell = M_sprite.at(x0, y0);
        cell.brush = M_currentState.brush;
        cell.color = M_currentState.color;

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
    if ( M_isTranslating || !M_currentState.selection.isActive ) return;

    M_isTranslating = true;
    M_spriteSnapshot = M_sprite;

    int w = M_currentState.selection.width();
    int h = M_currentState.selection.height();
    M_currentState.floatingSelection.assign(h, std::vector<Pixel>(w));

    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            M_currentState.floatingSelection[y][x] = M_spriteSnapshot.at(M_currentState.selection.minX() + x,M_currentState.selection.minY() + y);
            M_spriteSnapshot.at(M_currentState.selection.minX() + x,M_currentState.selection.minY() + y) = Pixel{" ", ftxui::Color::White};
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
    if ( !M_currentState.selection.isActive ) return false;

    if ( !M_isTranslating ) beginTranslation();

    int minX = M_currentState.selection.minX();
    int minY = M_currentState.selection.minY();
    int maxX = M_currentState.selection.maxX();
    int maxY = M_currentState.selection.maxY();

    if ( minX + dx < 0 || maxX + dx >= M_width || minY + dy < 0 || maxY + dy >= M_height )
        return false;

    M_currentState.selection.startX += dx;
    M_currentState.selection.endX += dx;
    M_currentState.selection.startY += dy;
    M_currentState.selection.endY += dy;

    M_sprite = M_spriteSnapshot;

    int w = M_currentState.selection.width();
    int h = M_currentState.selection.height();
    for ( int y = 0; y < h; ++y )
        for ( int x = 0; x < w; ++x )
            M_sprite.at(M_currentState.selection.minX() + x,M_currentState.selection.minY() + y) = M_currentState.floatingSelection[y][x];

    return true;
}


std::shared_ptr<EditorCanvasComponent> EditorCanvas( int width, int height )
{
    return std::make_shared<EditorCanvasComponent>( width, height );
}

}

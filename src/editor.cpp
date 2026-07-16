#include "editor.hpp"
#include <cmath>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>


namespace Termisprite
{



void
SpriteHistory::save( Sprite const& sprite )
{
    if ( M_currentIndex < static_cast<int>(M_history.size()) - 1 )
        M_history.erase( M_history.begin() + M_currentIndex + 1, M_history.end() );

    M_history.push_back( sprite );

    if ( M_history.size() > M_maxSize )
        M_history.erase( M_history.begin() );
    else
        M_currentIndex++;
}

void
SpriteHistory::undo( Sprite & sprite )
{
    if ( M_currentIndex > 0 )
    {
        M_currentIndex--;
        sprite = M_history[M_currentIndex];
    }
}

void
SpriteHistory::redo( Sprite & sprite )
{
    if ( M_currentIndex < static_cast<int>(M_history.size()) - 1 )
    {
        M_currentIndex++;
        sprite = M_history[M_currentIndex];
    }
}


ftxui::Element
EditorCanvasComponent::OnRender()
{

    ftxui::Elements rows;


    for ( int y = 0; y < M_height; ++y )
    {
        ftxui::Elements row;
        for ( int x = 0; x < M_width; ++x )
        {
            Pixel const& cellContent = M_sprite.at(x,y);
            ftxui::Element cell;

            std::string renderBrush = cellContent.brush;
            ftxui::Color renderColor = cellContent.color;

            if ( renderBrush == " " && M_showGrid)
            {
                if ( x % 2 == 0 && y % 1 == 0 )
                {
                    renderBrush = "·";
                    renderColor = ftxui::Color::GrayDark;
                }
            }
            if ( M_currentState.selection.isActive &&
                 x >= M_currentState.selection.minX() && x <= M_currentState.selection.maxX() &&
                 y >= M_currentState.selection.minY() && y <= M_currentState.selection.maxY() )
            {
                bool isTop = (y == M_currentState.selection.minY());
                bool isBottom = (y == M_currentState.selection.maxY());
                bool isLeft = (x == M_currentState.selection.minX());
                bool isRight = (x == M_currentState.selection.maxX());

                if (isTop && isLeft && isBottom && isRight) renderBrush = "⣿"; // 1x1 selection
                else if (isTop && isLeft) renderBrush = "⡏";
                else if (isTop && isRight) renderBrush = "⢹";
                else if (isBottom && isLeft) renderBrush = "⣇";
                else if (isBottom && isRight) renderBrush = "⣸";
                else if (isTop) renderBrush = "⠉";
                else if (isBottom) renderBrush = "⣀";
                else if (isLeft) renderBrush = "⡇";
                else if (isRight) renderBrush = "⢸";

                if ( isTop || isBottom || isLeft || isRight )
                    renderColor = ftxui::Color::White;
            }

            cell = ftxui::text( renderBrush ) | ftxui::color( renderColor );

            if ( M_showCursor && x == M_cursorX && y == M_cursorY )
                cell = ftxui::text( M_currentState.brush ) | ftxui::color( ftxui::Color::Red ) | ftxui::blink;

            row.push_back( cell );
        }
        rows.push_back( ftxui::hbox( row ) );
    }

    ftxui::Element canvas = ftxui::vbox( std::move( rows ) )
                              | ftxui::reflect( M_box )
                              | ftxui::border
                              | ftxui::size( ftxui::WIDTH, ftxui::EQUAL, M_width + 1 )
                              | ftxui::size( ftxui::HEIGHT, ftxui::EQUAL, M_height + 2 );


    //------------Build axis--------- (refactor into another method)
    ftxui::Elements xAxis = {ftxui::text("─")};

    for ( int i = 0; i < M_width+1; ++i )
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

        int localX = mouse.x - M_box.x_min;
        int localY = mouse.y - M_box.y_min;

        localX = std::clamp(localX, 0, M_width - 1);
        localY = std::clamp(localY, 0, M_height - 1);


        if ( mouse.motion == ftxui::Mouse::Pressed )
        {
            TakeFocus();
            M_isDrawing = true;
            M_lastDrawX = localX;
            M_lastDrawY = localY;
            Pixel & cell = M_sprite.at(localX, localY);
            cell.brush = M_currentState.brush;
            cell.color = M_currentState.color;
            return true;
        }
        else if ( (mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed) && M_isDrawing )
        {
            drawLine( M_lastDrawX, M_lastDrawY, localX, localY );
            M_lastDrawX = localX;
            M_lastDrawY = localY;
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

    int localX = mouse.x - M_box.x_min;
    int localY = mouse.y - M_box.y_min;

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

    int localX = mouse.x - M_box.x_min;
    int localY = mouse.y - M_box.y_min;

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

        int localX = mouse.x - M_box.x_min;
        int localY = mouse.y - M_box.y_min;

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
        int localX = mouse.x - M_box.x_min;
        int localY = mouse.y - M_box.y_min;

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

        int localX = std::clamp(mouse.x - M_box.x_min, 0, M_width - 1);
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

            M_modalX = std::clamp(mouse.x - M_box.x_min, 0, M_width);
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

        bool execute_action = ( event == ftxui::Event::Return || event == ftxui::Event::Character('\n') );

        if ( event.is_mouse() )
        {
            auto mouse = event.mouse();
            if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Pressed )
            {
                if ( M_rightClickModalBox.Contain( mouse.x, mouse.y ) )
                    execute_action = true;
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

        if ( execute_action )
        {
            switch ( M_rightClickModalIndex )
            {
                case 0: M_showGrid = !M_showGrid; break;
                case 1: M_spriteHistory.undo( M_sprite ); break;
                case 2: M_spriteHistory.redo( M_sprite ); break;
                case 3: this->clear(); break;
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

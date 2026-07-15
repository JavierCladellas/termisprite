#include "editor.hpp"


namespace Termisprite
{




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
            }

            cell = ftxui::text( renderBrush ) | ftxui::color( cellContent.color );

            if ( M_showCursor && x == M_cursorX && y == M_cursorY )
                cell = ftxui::text( M_currentState.brush ) | ftxui::color( ftxui::Color::Red ) | ftxui::blink;

            row.push_back( cell );
        }
        rows.push_back( ftxui::hbox( row ) );
    }

    return ftxui::vbox( std::move( rows ) ) | ftxui::reflect( M_box )
                                            | ftxui::border
                                            | ftxui::size( ftxui::WIDTH, ftxui::EQUAL, M_width + 1 )
                                            | ftxui::size( ftxui::HEIGHT, ftxui::EQUAL, M_height + 2 );
}

bool
EditorCanvasComponent::processHistoryEvents( ftxui::Event event )
{
    if ( event == ftxui::Event::CtrlZ || event == ftxui::Event::Character('u') )
    {
        if ( M_historyIndex > 0 )
        {
            M_historyIndex--;
            M_sprite = M_history[M_historyIndex];
        }
        return true;
    }

    if ( event == ftxui::Event::CtrlY || event == ftxui::Event::CtrlR )
    {
        if ( M_historyIndex < static_cast<int>(M_history.size()) - 1 )
        {
            M_historyIndex++;
            M_sprite = M_history[M_historyIndex];
        }
        return true;
    }

    return false;
}


bool
EditorCanvasComponent::processKeyboardDrawing( ftxui::Event event )
{
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

bool
EditorCanvasComponent::processClipboardEvents( ftxui::Event event )
{
    if ( event == ftxui::Event::CtrlC )
    {
        if ( !M_currentState.selection.isActive ) return false;

        int w = M_currentState.selection.width();
        int h = M_currentState.selection.height();

        M_currentState.clipboard.data.assign(h, std::vector<Pixel>(w));

        for ( int y = 0; y < h; ++y )
        {
            for ( int x = 0; x < w; ++x )
            {
                M_currentState.clipboard.data[y][x] = M_sprite.at(M_currentState.selection.minX() + x,M_currentState.selection.minY() + y);
            }
        }
        M_currentState.clipboard.hasData = true;
        return true;
    }

    if ( event == ftxui::Event::CtrlX )
    {
        if ( !M_currentState.selection.isActive ) return false;

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
        return true;
    }

    if ( event == ftxui::Event::CtrlV )
    {
        if ( !M_currentState.clipboard.hasData ) return false;

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
        return true;
    }

    return false;
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
    if ( !event.is_mouse() )
        return false;

    if ( M_currentState.toolType != ToolType::BRUSH && M_currentState.toolType != ToolType::ERASER )
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
    if ( !event.is_mouse() )
        return false;

    if ( M_currentState.toolType != ToolType::EYE_DROPPER )
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
    M_currentState.toolType = ToolType::BRUSH;
    return true;

}

bool
EditorCanvasComponent::processPaintFill( ftxui::Event event )
{
    if ( !event.is_mouse() )
        return false;

    if ( M_currentState.toolType != ToolType::PAINT_FILL )
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
    if ( !event.is_mouse() )
        return false;

    if ( M_currentState.toolType != ToolType::BOX_SELECT )
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
EditorCanvasComponent::OnEvent( ftxui::Event event )
{
    if ( processClipboardEvents( event ) )
        return true;

    if ( processHistoryEvents( event ) )
        return true;

    if ( processCursorMovement( event ) )
        return true;

    if ( processKeyboardDrawing( event ) )
        return true;


    if ( event.is_mouse() )
    {
        if ( processMouseDrawing( event ) )
            return true;

        if ( processEyeDropper( event ) )
            return true;

        if ( processPaintFill( event ) )
            return true;

        if ( processBoxSelection( event ) )
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
    if ( M_historyIndex < static_cast<int>(M_history.size()) - 1 )
        M_history.erase( M_history.begin() + M_historyIndex + 1, M_history.end() );

    M_history.push_back( M_sprite );

    if ( M_history.size() > M_maxHistorySize )
        M_history.erase( M_history.begin() );
    else
        M_historyIndex++;


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

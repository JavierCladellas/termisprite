#include "selection_tool.hpp"

namespace Termisprite
{


void
SelectionTool::render( std::vector<ftxui::Elements> & cells, bool isSquarePixel )
{
    if ( !M_isActive ) return;

    int canvasHeight = cells.size();
    if ( canvasHeight == 0 ) return;
    int canvasWidth = cells[0].size();

    int scale = isSquarePixel ? 2 : 1;
    auto [camX, camY] = M_layer->camera().position();

    int startY = minY() - camY;
    int endY = maxY() - camY;
    int startX = minX() - camX;
    int endX = maxX() - camX;

    for ( int screenY = startY; screenY <= endY; ++screenY )
    {
        if ( screenY < 0 || screenY >= canvasHeight ) continue;

        bool isTop = (screenY == startY);
        bool isBot = (screenY == endY);

        for ( int screenX = startX; screenX <= endX; ++screenX )
        {
            bool isLeft = (screenX == startX);
            bool isRight = (screenX == endX);
            if ( !(isTop || isBot || isLeft || isRight) ) continue;

            int terminalXStart = screenX * scale;

            if ( terminalXStart < 0 || terminalXStart >= canvasWidth ) continue;

            std::string brushL, brushR;

            if (isTop && isLeft && isBot && isRight) { brushL = "⡏"; brushR = "⢹"; }
            else if (isTop && isLeft) { brushL = "⡏"; brushR = "⠉"; }
            else if (isTop && isRight) { brushL = "⢹"; brushR = " "; }
            else if (isBot && isLeft) { brushL = "⣇"; brushR = "⣀"; }
            else if (isBot && isRight) { brushL = "⣸"; brushR = " "; }
            else if (isTop) { brushL = "⠉"; brushR = "⠉"; }
            else if (isBot) { brushL = "⣀"; brushR = "⣀"; }
            else if (isLeft) { brushL = "⡇"; brushR = " "; }
            else if (isRight) { brushL = "⢸"; brushR = " "; }

            cells[screenY][terminalXStart] = ftxui::text(brushL) | ftxui::color( M_color );

            if ( isSquarePixel && (terminalXStart + 1 < canvasWidth) )
                cells[screenY][terminalXStart + 1] = ftxui::text(brushR) | ftxui::color( M_color );
        }
    }
}



void
SelectionTool::beginTranslation()
{
    if ( M_isTranslating || !isActive() ) return;

    M_isTranslating = true;
    M_snapshot = *M_layer;

    int w = width();
    int h = height();
    M_selection.assign(h, std::vector<Pixel>(w));

    auto [maxW, maxH] = M_layer->size();
    auto [lX, lY] = M_layer->position();

    for ( int y = 0; y < h; ++y )
    {
        int layerY = minY() + y - lY;
        if ( layerY < 0 || layerY >= maxH ) continue;
        for ( int x = 0; x < w; ++x )
        {
            int layerX = minX() + x - lX;
            if ( layerX < 0 || layerX >= maxW ) continue;

            M_selection[y][x] = M_snapshot.at(layerX,layerY);
            M_snapshot.at(layerX,layerY) = Pixel{" ", ftxui::Color::White};
        }
    }
}

void
SelectionTool::endTranslation()
{
    if ( !M_isTranslating ) return;
    M_isTranslating = false;
    M_selection.clear();
    M_isActive = false;
}

bool
SelectionTool::translateContent( int dx, int dy )
{
    if ( !isActive() ) return false;

    if ( !M_isTranslating )
        beginTranslation();


    *M_layer = M_snapshot;
    auto [maxW,maxH] = M_layer->size();
    auto [lX, lY] = M_layer->position();

    setStart( startX() + dx, startY() + dy);
    setEnd( endX() + dx, endY() + dy);


    int w = width();
    int h = height();
    for ( int y = 0; y < h; ++y )
    {
        int layerY = minY() + y - lY;
        if ( layerY < 0 || layerY >= maxH ) continue;
        for ( int x = 0; x < w; ++x )
        {
            int layerX = minX() + x - lX;
            if ( layerX < 0 || layerX >= maxW ) continue;
            M_layer->at(layerX,layerY) = M_selection[y][x];
        }
    }

    return true;
}


void
SelectionTool::deleteContent( Layer & layer )
{
    int w = width();
    int h = height();

    auto [maxW, maxH] = layer.size();
    auto [lX, lY] = layer.position();

    for ( int y = 0; y < h; ++y )
    {
        int layerY = minY() + y - lY;
        if ( layerY < 0 || layerY >= maxH ) continue;
        for ( int x = 0; x < w; ++x )
        {
            int layerX = minX() + x - lX;
            if ( layerX < 0 || layerX >= maxW ) continue;
            layer.at(layerX, layerY) = Pixel{" ", ftxui::Color::White};
        }
    }

    setActive(false);
}

bool
SelectionTool::processKeyboardEvent( ftxui::Event event )
{
    if ( M_isDrawing && event == ftxui::Event::Escape )
    {
        M_isDrawing = false;
        setActive(false);
        return true;
    }

    if ( !M_isDrawing &&  isActive() &&
         (event == ftxui::Event::Escape || event == ftxui::Event::Return) )
    {
        endTranslation();
        return true;
    }

    auto [camX, camY] = M_layer->camera().position();
    if ( event == ftxui::Event::Character(' ') || event == ftxui::Event::Return )
    {
        M_cursor.setVisibility(true);

        if ( !M_isDrawing )
        {
            M_isDrawing = true;
            setActive(true);

            setStart(M_cursor.x() + camX, M_cursor.y() + camY);
            setEnd(M_cursor.x() + camX, M_cursor.y() + camY);
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
        bool wasActive = isActive();
        setActive(false);

        int maxW, maxH;
        std::tie(maxW, maxH) = M_layer->size();
        bool moved = processTranslation( event, maxW, maxH );

        setActive(wasActive);

        if ( moved )
        {
            M_cursor.setVisibility(true);
            setEnd(M_cursor.x() + camX, M_cursor.y()+ camY);
            return true;
        }
    }

    return false;

}


bool
SelectionTool::processMouseEvent( ftxui::Event event )
{
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

    auto [localX, localY] = M_screenToWorld(mouse.x, mouse.y);
    if ( localX < 0 || localY < 0 ) //screenToWorld returned invalid coordinates (out of screen box )
        return false;

    auto [camX, camY] = M_layer->camera().position();
    int worldX = localX + camX;
    int worldY = localY + camY;

    if ( mouse.button == ftxui::Mouse::Button::Left )
    {
        M_cursor.setVisibility(false);

        if ( mouse.motion == ftxui::Mouse::Pressed )
        {
            M_isDrawing = true;
            setActive(true);

            setStart( worldX, worldY );
            setEnd( worldX, worldY );
            return true;
        }
        else if ( (mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed) && M_isDrawing )
        {
            setEnd( worldX, worldY );
            return true;
        }
    }

    if ( mouse.button == ftxui::Mouse::Button::Right )
    {
        if ( mouse.motion == ftxui::Mouse::Pressed )
        {
            if ( isActive() )
            {
                if ( worldX >= minX() && worldX <= maxX() &&
                     worldY >= minY() && worldY <= maxY() )
                {
                    beginTranslation(  );
                    M_lastX = worldX;
                    M_lastY = worldY;
                    return true;
                }
                else
                {
                    endTranslation();
                    return true;
                }
            }
        }
        else if ( (mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed) && isTranslating() )
        {
            int dx = worldX - M_lastX;
            int dy = worldY - M_lastY;

            if ( dx != 0 || dy != 0 )
            {
                if ( translateContent( dx, dy ) )
                {
                    M_lastX = worldX;
                    M_lastY = worldY;
                }
            }
            return true;
        }
        else if ( mouse.motion == ftxui::Mouse::Released && isTranslating() )
        {
            endTranslation();
            return true;
        }
    }

    return false;

}


bool
SelectionTool::processTranslation( ftxui::Event event, int maxWidth, int maxHeight )
{
    int dx = 0, dy = 0;

    if ( event == ftxui::Event::ArrowUp || event == ftxui::Event::Character( 'k' ) ) dy = -1;
    else if ( event == ftxui::Event::ArrowDown || event == ftxui::Event::Character( 'j' ) ) dy = 1;
    else if ( event == ftxui::Event::ArrowLeft || event == ftxui::Event::Character( 'h' ) ) dx = -1;
    else if ( event == ftxui::Event::ArrowRight || event == ftxui::Event::Character( 'l' ) ) dx = 1;

    if ( dx != 0 || dy != 0 )
    {
        if ( isActive() )
        {
            translateContent( dx, dy ) ;
            return true;
        }
    }

    if ( M_cursor.processMovement(event, maxWidth, maxHeight) )
        return true;

    return false;
}


}

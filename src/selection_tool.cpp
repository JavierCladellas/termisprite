#include "selection_tool.hpp"

namespace Termisprite
{


void
SelectionTool::render( std::vector<ftxui::Elements> & cells, bool isSquarePixel )
{

    if ( !M_isActive ) return;

    std::string brushL, brushR;
    std::string brush;
    for ( int y = minY(); y < minY() + height(); ++y )
    {
        bool isTop = y == minY();
        bool isBot = y == maxY();

        for ( int x = minX(); x < minX() + width(); ++x )
        {
            bool isLeft = x == minX();
            bool isRight = x == maxX();

            if (isTop && isLeft && isBot && isRight) { brushL = "⡏"; brushR = "⢹"; }
            else if (isTop && isLeft) { brushL = "⡏"; brushR = "⠉"; }
            else if (isTop && isRight) { brushL = "⠉"; brushR = "⢹"; }
            else if (isBot && isLeft) { brushL = "⣇"; brushR = "⣀"; }
            else if (isBot && isRight) { brushL = "⣀"; brushR = "⣸"; }
            else if (isTop) { brushL = "⠉"; brushR = "⠉"; }
            else if (isBot) { brushL = "⣀"; brushR = "⣀"; }
            else if (isLeft) { brushL = "⡇"; brushR = " "; }
            else if (isRight) { brushL = " "; brushR = "⢸"; }

            brush = brushL;
            if ( isSquarePixel )
                brush += brushR;


            if ( isTop || isBot || isLeft || isRight )
                cells[y][x] = ftxui::text(brush) | ftxui::color( M_color );
        }
    }

}



void
SelectionTool::beginTranslation( Sprite const& sprite, Sprite & snapshot )
{
    if ( M_isTranslating || !isActive() ) return;

    M_isTranslating = true;
    snapshot = sprite;

    int w = width();
    int h = height();
    M_selection.assign(h, std::vector<Pixel>(w));

    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            M_selection[y][x] = snapshot.at(minX() + x,minY() + y);
            snapshot.at(minX() + x,minY() + y) = Pixel{" ", ftxui::Color::White};
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
SelectionTool::translateContent( Sprite & sprite, Sprite & snapshot, int maxH, int maxW, int dx, int dy )
{
    if ( !isActive() ) return false;

    if ( !M_isTranslating )
        beginTranslation( sprite, snapshot);

    if ( minX() + dx < 0 || maxX() + dx >= maxW || minY() + dy < 0 || maxY() + dy >= maxH )
        return false;

    setStart( startX() + dx, startY() + dy);
    setEnd( endX() + dx, endY() + dy);

    sprite = snapshot;

    int w = width();
    int h = height();
    for ( int y = 0; y < h; ++y )
        for ( int x = 0; x < w; ++x )
            sprite.at(minX() + x,minY() + y) = M_selection[y][x];

    return true;
}


void
SelectionTool::deleteContent( Sprite & sprite )
{
    int w = width();
    int h = height();

    for ( int y = 0; y < h; ++y )
        for ( int x = 0; x < w; ++x )
            sprite.at(minX() + x,minY() + y) = Pixel{" ", ftxui::Color::White};

    setActive(false);
}


bool
SelectionTool::processSelection( CanvasCursor & cursor, Sprite & sprite, Sprite & snapshot, bool & isDrawing, 
                                 int canvasHeight, int canvasWidth, ftxui::Event event,
                                 std::function<std::pair<int,int>(int,int)> screenToWorld )
{

    if ( event.is_mouse() )
    {
        auto mouse = event.mouse();

        if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Released )
        {
            if ( isDrawing )
            {
                isDrawing = false;
                return true;
            }
        }

        auto [localX, localY] = screenToWorld(mouse.x, mouse.y);
        if ( localX < 0 || localY < 0 ) //screenToWorld returned invalid coordinates (out of screen box )
            return false;

        if ( mouse.button == ftxui::Mouse::Button::Left )
        {
            cursor.setVisibility(false);


            localX = std::clamp(localX, 0, canvasWidth - 1);
            localY = std::clamp(localY, 0, canvasHeight - 1);

            if ( mouse.motion == ftxui::Mouse::Pressed )
            {
                isDrawing = true;
                setActive(true);

                setStart( localX, localY );
                setEnd( localX, localY );
                return true;
            }
            else if ( (mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed) && isDrawing )
            {
                setEnd( localX, localY );
                return true;
            }
        }

        if ( mouse.button == ftxui::Mouse::Button::Right )
        {
            if ( mouse.motion == ftxui::Mouse::Pressed )
            {
                if ( isActive() )
                {
                    if ( localX >= minX() && localX <= maxX() &&
                         localY >= minY() && localY <= maxY() )
                    {
                        beginTranslation( sprite, snapshot );
                        M_lastDragX = localX;
                        M_lastDragY = localY;
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
                int dx = localX - M_lastDragX;
                int dy = localY - M_lastDragY;

                if ( dx != 0 || dy != 0 )
                {
                    if ( translateContent( sprite, snapshot, canvasHeight, canvasWidth, dx, dy ) )
                    {
                        M_lastDragX = localX;
                        M_lastDragY = localY;
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


    if ( isDrawing && event == ftxui::Event::Escape )
    {
        isDrawing = false;
        setActive(false);
        return true;
    }

    if ( !isDrawing &&  isActive() &&
         (event == ftxui::Event::Escape || event == ftxui::Event::Return) )
    {
        endTranslation();
        return true;
    }

    if ( event == ftxui::Event::Character(' ') || event == ftxui::Event::Return )
    {
        cursor.setVisibility(true);

        if ( !isDrawing )
        {
            isDrawing = true;
            setActive(true);

            setStart(cursor.x(), cursor.y());
            setEnd(cursor.x(), cursor.y());
        }
        else
        {
            // Finish the box shape
            isDrawing = false;
        }
        return true;
    }

    if ( isDrawing )
    {
        bool wasActive = isActive();
        setActive(false);

        bool moved = processTranslation(cursor, sprite, snapshot, canvasHeight, canvasWidth, event );

        setActive(wasActive);

        if ( moved )
        {
            cursor.setVisibility(true);
            setEnd(cursor.x(), cursor.y());
            return true;
        }
    }

    return false;
}

bool
SelectionTool::processTranslation( CanvasCursor & cursor, Sprite & sprite, Sprite & snapshot,
                                   int canvasHeight, int canvasWidth, ftxui::Event event )
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
            translateContent( sprite, snapshot, canvasHeight, canvasWidth, dx, dy ) ;
            return true;
        }
    }

    if ( cursor.processMovement(event, canvasHeight, canvasWidth ) )
        return true;

    return false;
}


}

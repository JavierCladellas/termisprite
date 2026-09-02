#include "move_tool.hpp"

namespace Termisprite
{




void
MoveTool::translateLayer( int dx, int dy )
{
    auto [lw,lh] = M_layer->size();
    auto [lx,ly] = M_layer->position();
    auto [maxWidth,maxHeight] = M_layer->camera().size();

    M_layer->setPosition(
        std::clamp(lx + dx, -lw + 1, maxWidth - 1),
        std::clamp(ly + dy, -lh + 1, maxHeight - 1)
    );
}

bool
MoveTool::processTranslation( ftxui::Event event, int maxWidth, int maxHeight )
{
    int dx = 0, dy = 0;

    if ( event == ftxui::Event::ArrowUp || event == ftxui::Event::Character( 'k' ) ) dy = -1;
    else if ( event == ftxui::Event::ArrowDown || event == ftxui::Event::Character( 'j' ) ) dy = 1;
    else if ( event == ftxui::Event::ArrowLeft || event == ftxui::Event::Character( 'h' ) ) dx = -1;
    else if ( event == ftxui::Event::ArrowRight || event == ftxui::Event::Character( 'l' ) ) dx = 1;

    if ( dx != 0 || dy != 0 )
    {
        translateLayer( dx, dy );
        return true;
    }
    return false;
}


bool
MoveTool::processKeyboardEvent( ftxui::Event event )
{
    if ( event == ftxui::Event::Character(' ') || event == ftxui::Event::Return )
    {
        setActive(false);
        return true;
    }

    return processTranslation( event, M_layer->camera().width(), M_layer->camera().height() );

}


//TODO: Refactor this somewhere ( shared behavior of selection tool and camera panning)
bool
MoveTool::processMouseEvent( ftxui::Event event )
{
    if ( !event.is_mouse() )
        return false;

    auto mouse = event.mouse();

    auto [localX, localY] = M_screenToWorld(mouse.x, mouse.y);
    if ( localX < 0 || localY < 0 ) //screenToWorld returned invalid coordinates (out of screen box )
    {
        M_isDrawing = false;
        return false;
    }

    if ( mouse.motion == ftxui::Mouse::Pressed && !M_isDrawing )
    {
        M_isDrawing = true;
        M_cursor.setVisibility(false);
        M_lastX = localX;
        M_lastY = localY;
        return true;
    }
    else if ( M_isDrawing )
    {
        if ( mouse.motion == ftxui::Mouse::Released )
        {
            M_isDrawing = false;
            return true;
        }

        if ( mouse.motion == ftxui::Mouse::Moved || mouse.motion == ftxui::Mouse::Pressed )
        {
            int dx = localX - M_lastX;
            int dy = localY - M_lastY;

            if ( dx != 0 || dy != 0 )
            {
                translateLayer( dx, dy );
                M_lastX = localX;
                M_lastY = localY;
            }
            return true;
        }

    }


    return false;


}


}

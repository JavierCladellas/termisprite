#include "camera.hpp"

namespace Termisprite
{


bool
Camera::processPanning( ftxui::Event event, int canvasWidth, int canvasHeight )
{

    if ( !event.is_mouse() ) return false;

    auto mouse = event.mouse();

    int maxPanX = std::max(0, canvasWidth - M_width);
    int maxPanY = std::max(0, canvasHeight - M_height);

    if ( maxPanX == 0 && maxPanY == 0 )
    {
        M_isPanning = false;
        return false;
    }



    auto [localX, localY] = M_screenToWorld(mouse.x, mouse.y);
    if ( localX < 0 || localY < 0 )
        return false;

    if ( mouse.motion == ftxui::Mouse::Pressed && !M_isPanning )
    {
        M_isPanning = true;
        // M_cursor->setVisibility(false);
        M_lastPanX = localX;
        M_lastPanY = localY;
        return true;
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
            int dx = (localX - M_lastPanX);
            int dy = localY - M_lastPanY;

            if ( dx != 0 || dy != 0 )
            {
                M_x = std::clamp(M_x - dx, 0, maxPanX);
                M_y = std::clamp(M_y - dy, 0, maxPanY);

                M_lastPanX = localX;
                M_lastPanY = localY;
            }
            return true;
        }
    }

    return false;



}


}

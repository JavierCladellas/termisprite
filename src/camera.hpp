#pragma once


#include <ftxui/component/event.hpp>
namespace Termisprite
{


class Camera
{
public:
    Camera( int width, int height )
        : M_width(width), M_height(height)
    {}

    bool isPanning() const { return M_isPanning; }
    bool processMouseEvent( ftxui::Event event );

    int width() const { return M_width; }
    int height() const { return M_height; }

    void resize( int width, int height )
    {
        M_width = width;
        M_height = height;
    }

    std::pair<int,int> screenToWorld(int screenX, int screenY) const;
    std::pair<int,int> worldToScreen(int worldX, int worldY) const;

private:
    int M_x = 0, M_y = 0;
    int M_width, M_height;

    bool M_isPanning = false;
    int M_lastPanX = 0, M_lastPanY = 0;

};

}

#pragma once


#include <ftxui/component/event.hpp>
#include <functional>
namespace Termisprite
{


class Camera
{
public:
    Camera( int width, int height, std::function<std::pair<int,int>(int,int)> screenToWorld = {})
        : M_width(width), M_height(height), M_screenToWorld(screenToWorld)
    {}

    bool isPanning() const { return M_isPanning; }
    bool processPanning( ftxui::Event event, int canvasWidth, int canvasHeight );

    int width() const { return M_width; }
    int height() const { return M_height; }
    std::pair<int,int> size() const { return { M_width, M_height }; }

    int posX() const { return M_x; }
    int posY() const { return M_y; }
    std::pair<int,int> position() const { return { M_x, M_y }; }

    void resize( int width, int height )
    {
        M_width = width;
        M_height = height;
    }

private:
    int M_x = 0, M_y = 0;
    int M_width, M_height;

    bool M_isPanning = false;
    int M_lastPanX = 0, M_lastPanY = 0;

    std::function<std::pair<int,int>(int,int)> M_screenToWorld;
};

}

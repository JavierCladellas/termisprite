#pragma once


#include "sprite.hpp"
#include <deque>

namespace Termisprite
{


struct DocumentSnapshot
{
    std::vector<Layer> layers;
    int activeLayerIndex = 0;

    int canvasWidth;
    int canvasHeight;
};


class History
{
public:
    History( int maxSize = 100 )
        : M_maxSize( maxSize )
    {}

    void push( DocumentSnapshot const& snapshot );
    void save( DocumentSnapshot const& snapshot );
    bool undo( DocumentSnapshot & snapshot );
    bool redo( DocumentSnapshot & snapshot );

private:
    std::deque<DocumentSnapshot> M_snapshots;
    int M_maxSize = 100;
    int M_currentIndex = 0;
};


}

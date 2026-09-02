#include "history.hpp"



namespace Termisprite
{

void
History::push( DocumentSnapshot const& snapshot )
{



}


void
History::save( DocumentSnapshot const& snapshot )
{
    if ( M_currentIndex < (int)M_snapshots.size() - 1 )
        M_snapshots.erase( M_snapshots.begin() + M_currentIndex + 1, M_snapshots.end() );

    M_snapshots.push_back( snapshot );

    if ( M_snapshots.size() > M_maxSize )
        M_snapshots.pop_front();
    else
        M_currentIndex++;
}

bool
History::undo( DocumentSnapshot & snapshot )
{
    if ( M_currentIndex > 0 )
    {
        M_currentIndex--;
        snapshot = M_snapshots[M_currentIndex];
        return true;
    }
    return false;
}

bool
History::redo( DocumentSnapshot & snapshot )
{
    if ( M_currentIndex < (int)M_snapshots.size() - 1 )
    {
        M_currentIndex++;
        snapshot = M_snapshots[M_currentIndex];
        return true;
    }
    return false;
}


}






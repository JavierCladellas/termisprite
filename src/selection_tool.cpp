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

}

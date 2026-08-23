#include "clipboard.hpp"

namespace Termisprite
{

void
Clipboard::copy( Sprite const& sprite, SelectionTool & selectionTool )
{
    int w = selectionTool.width();
    int h = selectionTool.height();

    data.assign(h, std::vector<Pixel>(w));

    for ( int y = 0; y < h; ++y )
        for ( int x = 0; x < w; ++x )
            data[y][x] = sprite.at( selectionTool.minX() + x, selectionTool.minY() + y );

    hasData = true;
}


void
Clipboard::cut( Sprite & sprite, SelectionTool & selectionTool )
{
    int w = selectionTool.width();
    int h = selectionTool.height();

    data.assign(h, std::vector<Pixel>(w));

    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            data[y][x] = sprite.at( selectionTool.minX() + x, selectionTool.minY() + y);
            sprite.at( selectionTool.minX() + x, selectionTool.minY() + y) = Pixel{" ", ftxui::Color::White};
        }
    }
    hasData = true;
    selectionTool.setActive(false);

}


void
Clipboard::paste( Sprite & sprite, int cursorX, int cursorY )
{
    int h = data.size();
    int w = (h > 0) ? data[0].size() : 0;

    int spriteW, spriteH;
    std::tie( spriteW, spriteH ) = sprite.size();

    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            int targetY = cursorY + y;
            int targetX = cursorX + x;

            if ( targetY >= 0 && targetY < spriteH && targetX >= 0 && targetX < spriteW )
                sprite.at(targetX,targetY) = data[y][x];
        }
    }

}


}

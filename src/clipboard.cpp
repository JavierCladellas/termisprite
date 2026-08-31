#include "clipboard.hpp"

namespace Termisprite
{

void
Clipboard::copy( Layer const& layer, SelectionTool & selectionTool )
{
    int w = selectionTool.width();
    int h = selectionTool.height();

    data.assign(h, std::vector<Pixel>(w));

    for ( int y = 0; y < h; ++y )
        for ( int x = 0; x < w; ++x )
            data[y][x] = layer.at( selectionTool.minX() + x, selectionTool.minY() + y );

    hasData = true;
}


void
Clipboard::cut( Layer & layer, SelectionTool & selectionTool )
{
    int w = selectionTool.width();
    int h = selectionTool.height();

    data.assign(h, std::vector<Pixel>(w));

    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            data[y][x] = layer.at( selectionTool.minX() + x, selectionTool.minY() + y);
            layer.at( selectionTool.minX() + x, selectionTool.minY() + y) = Pixel{" ", ftxui::Color::White};
        }
    }
    hasData = true;
    selectionTool.setActive(false);

}


void
Clipboard::paste( Layer & layer, int cursorX, int cursorY )
{
    int h = data.size();
    int w = (h > 0) ? data[0].size() : 0;

    int spriteW, spriteH;
    std::tie( spriteW, spriteH ) = layer.size();

    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            int targetY = cursorY + y;
            int targetX = cursorX + x;

            if ( targetY >= 0 && targetY < spriteH && targetX >= 0 && targetX < spriteW )
                layer.at(targetX,targetY) = data[y][x];
        }
    }

}


}

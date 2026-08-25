namespace Termisprite
{

class BrushTool;
class Sprite;

namespace Geometry
{


//Bresenham's line algorithm
void
drawLine(BrushTool & brush, Sprite & sprite,  int x0, int y0, int x1, int y1 );


//BFS flood fill algorithm
void
floodFill( BrushTool & brush, Sprite & sprite, int x, int y );

}

}

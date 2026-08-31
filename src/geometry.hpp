namespace Termisprite
{

class BrushTool;
class Layer;

namespace Geometry
{


//Bresenham's line algorithm
void
drawLine(BrushTool & brush, Layer & layer,  int x0, int y0, int x1, int y1 );


//BFS flood fill algorithm
void
floodFill( BrushTool & brush, Layer & layer, int x, int y );

}

}

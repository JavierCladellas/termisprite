#pragma once

#include <algorithm>
#include <ftxui/dom/elements.hpp>
#include <stdexcept>
#include <string>
#include <vector>
#include <deque>

#include <ftxui/screen/color.hpp>

namespace Termisprite
{

struct Pixel
{
    std::string brush = " ";
    ftxui::Color color = ftxui::Color::RGB(255, 255, 255);
};

class Layer
{
public:
    using GridData = std::vector<std::vector<Pixel>>;
public:
    Layer( int width = 48, int height = 48, std::string const& name = "Unamed Layer" )
        : M_width( width ), M_height( height ), M_name( name )
    {
        M_grid.resize( M_height, std::vector<Pixel>( M_width ) );
    }


    std::vector<Pixel> & operator[]( int y ) { return M_grid[y]; }
    Pixel & at( int x, int y )
    {
        if ( x < 0 || x >= M_width || y < 0 || y >= M_height )
            throw std::out_of_range("Sprite::at: Coordinates out of bounds");
        return M_grid[y][x];
    }

    Pixel const& at( int x, int y ) const
    {
        if ( x < 0 || x >= M_width || y < 0 || y >= M_height )
            throw std::out_of_range("Sprite::at: Coordinates out of bounds");
        return M_grid[y][x];
    }

    GridData::iterator begin() { return M_grid.begin(); }
    GridData::iterator end() { return M_grid.end(); }
    GridData::const_iterator begin() const { return M_grid.begin(); }
    GridData::const_iterator end() const { return M_grid.end(); }

    void clear() { M_grid.assign(M_height, std::vector<Pixel>(M_width)); };
    void resize( int width, int height );

    bool isVisible() const { return M_isVisible; };
    void toggleVisibility() { M_isVisible = !M_isVisible; }

    void flipVertical()
    {
        std::reverse( M_grid.begin(), M_grid.end() );
    }

    void flipHorizontal()
    {
        for ( auto & row : M_grid )
            std::reverse( row.begin(), row.end() );
    }

    std::pair<int, int> size() const { return { M_width, M_height }; }

    void render( std::vector<std::vector<ftxui::Element>> & cells, bool isSquarePixel ) const;

    std::string const& name() const { return M_name; }

private:
    int M_width, M_height;
    GridData M_grid;

    bool M_isVisible = true;
    std::string M_name;
};


class SpriteHistory
{
public:
    SpriteHistory( int maxSize = 50 )
        : M_maxSize( maxSize ), M_currentIndex( 0 )
    { }

    void push( Layer const & layer ) { M_history.push_back( layer ); }
    void save( Layer const& layer );
    void undo( Layer & layer );
    void redo( Layer & layer );


private:
    std::deque<Layer> M_history;
    int M_maxSize = 50;
    int M_currentIndex = 0;
};


}

#pragma once

#include "camera.hpp"
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
    Layer() : M_camera(*new Camera( 48, 48 ) ) { }
    Layer(Camera & camera, int width = 48, int height = 48, std::string const& name = "Unamed Layer" )
        : M_width( width ), M_height( height ), M_name( name ), M_camera( camera )
    {
        M_grid.resize( M_height, std::vector<Pixel>( M_width ) );
    }

    Layer( Layer const& other )
        : M_width( other.M_width ), M_height( other.M_height ), M_x( other.M_x ), M_y( other.M_y ), M_grid( other.M_grid ), M_isVisible( other.M_isVisible ), M_name( other.M_name ), M_camera( other.M_camera )
    {}
    Layer operator=( Layer const& other )
    {
        M_width = other.M_width;
        M_height = other.M_height;
        M_x = other.M_x;
        M_y = other.M_y;
        M_grid = other.M_grid;
        M_isVisible = other.M_isVisible;
        M_name = other.M_name;
        return *this;
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
    std::pair<int, int> position() const { return { M_x, M_y }; }
    void setPosition( int x, int y ) { M_x = x; M_y = y; }

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

    Camera const& camera() const { return M_camera; }

private:
    int M_width, M_height;
    int M_x = 0, M_y = 0;
    GridData M_grid;

    bool M_isVisible = true;
    std::string M_name;

    Camera & M_camera;
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

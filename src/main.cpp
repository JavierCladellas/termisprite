//!

#include "editor.hpp"
#include "tools.hpp"
#include "colorpicker.hpp"
#include "menu.hpp"
#include "statusbar.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>


// struct PixelInfo
// {
//     std::string character = " ";
//     ftxui::Color foreground_color = ftxui::Color::Black;
//     ftxui::Color background_color = ftxui::Color::White;
// };


// struct PairHash {
//     std::size_t operator()(const std::pair<int, int>& p) const {
//         // A simple hash function to combine the X and Y coordinates
//         return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
//     }
// };

// Create a clean alias for our infinite map
// using CanvasBuffer = std::unordered_map<std::pair<int, int>, PixelInfo, PairHash>;



// class EditorNode : public ftxui::Node
// {
//     using TransformFunc = std::function<std::pair<double, double>(int, int)>;
// public:
//     EditorNode( CanvasBuffer & buffer, TransformFunc transform )
//         : M_buffer( buffer ), M_transform( std::move( transform ) )
//     { }
//
//     void ComputeRequirement() override
//     {
//         requirement_.min_x = 100;
//         requirement_.min_y = 100;
//     }
//
//     void Render( ftxui::Screen & screen ) override
//     {
//         // 1. Calculate the current view bounds in world coordinates
//         auto [worldMinX, worldMinY] = M_transform(box_.x_min, box_.y_min);
//         auto [worldMaxX, worldMaxY] = M_transform(box_.x_max, box_.y_max);
//
//         int minWcharX = static_cast<int>(std::min(worldMinX, worldMaxX)) - 1;
//         int maxWcharX = static_cast<int>(std::max(worldMinX, worldMaxX)) + 1;
//         int minWcharY = static_cast<int>(std::min(worldMinY, worldMaxY)) - 1;
//         int maxWcharY = static_cast<int>(std::max(worldMinY, worldMaxY)) + 1;
//
//         // 2. Clear the screen layout within our component box boundaries
//         for ( int y = box_.y_min; y <= box_.y_max; ++y )
//         {
//             for ( int x = box_.x_min; x <= box_.x_max; ++x )
//             {
//                 auto & pixel = screen.PixelAt(x, y);
//                 pixel.foreground_color = ftxui::Color::Black;
//                 pixel.background_color = ftxui::Color::White;
//                 pixel.character = " ";
//             }
//         }
//
//         // 3. Loop over only the drawn pixels in the map.
//         // This is extremely efficient and scales cleanly with zoom out!
//         for ( const auto& [coord, pixelInfo] : M_buffer )
//         {
//             int pX = coord.first;
//             int pY = coord.second;
//
//             // Frustum Culling: Skip if this pixel is outside the user's visible window
//             if ( pX < minWcharX || pX > maxWcharX || pY < minWcharY || pY > maxWcharY )
//                 continue;
//
//             // Reverse translate the world coordinates to locate the exact terminal grid cell
//             // We use standard linear interpolation to find where the world pixel lands on the screen box
//             double pctX = (maxWcharX == minWcharX) ? 0.5 : double(pX - minWcharX) / (maxWcharX - minWcharX);
//             double pctY = (maxWcharY == minWcharY) ? 0.5 : double(pY - minWcharY) / (maxWcharY - minWcharY);
//
//             int screenX = box_.x_min + static_cast<int>(pctX * (box_.x_max - box_.x_min));
//             int screenY = box_.y_min + static_cast<int>(pctY * (box_.y_max - box_.y_min));
//
//             // Extra protection check against terminal boundary exceptions
//             if ( screenX >= box_.x_min && screenX <= box_.x_max && screenY >= box_.y_min && screenY <= box_.y_max )
//             {
//                 auto & pixel = screen.PixelAt(screenX, screenY);
//                 pixel.character = pixelInfo.character;
//                 pixel.foreground_color = pixelInfo.foreground_color;
//                 pixel.background_color = pixelInfo.background_color;
//             }
//         }
//     }
//
// private:
//     CanvasBuffer & M_buffer;
//     TransformFunc M_transform;
// };;

// class EditorCanvas
//     : public Feel::Core::tui::Viewport
// {
// public:
//     explicit EditorCanvas(  bool allowZoom = true, bool allowPan = true )
//         : Viewport( 100, 100, 1, 1, allowZoom, allowPan )
//     { }
//
//
//     void setBrush( std::string brush ) { M_currentBrush = brush; }
//
//     void setColor( ftxui::Color color ) { M_currentColor = color; }
//
//     bool OnEvent( ftxui::Event event ) override
//     {
//         if ( event.is_mouse() )
//         {
//             auto mouse = event.mouse();
//
//             if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Released )
//                 M_isDrawing = false;
//
//             // Handle Zoom Clamping
//             if ( M_allowZoom && isInsideBox( mouse.x, mouse.y ) )
//             {
//                 double nextZoom = M_zoom;
//                 if ( mouse.button == ftxui::Mouse::WheelUp )   nextZoom *= 1.15;
//                 if ( mouse.button == ftxui::Mouse::WheelDown ) nextZoom *= (1.0 / 1.15);
//
//                 // Define strict limits: 0.25x (zoomed out) to 4.0x (zoomed in close)
//                 const double MIN_ZOOM = 0.25;
//                 const double MAX_ZOOM = 4.0;
//
//                 if ( nextZoom < MIN_ZOOM || nextZoom > MAX_ZOOM )
//                 {
//                     return true; // Consume the event but do nothing, capping the zoom perfectly
//                 }
//             }
//
//         }
//
//         return Viewport::OnEvent( event );
//     }
//
// private:
//     CanvasBuffer M_buffer;
//     std::string M_currentBrush = "█";
//     ftxui::Color M_currentColor = ftxui::Color::White;
//
//     bool M_isDrawing = false;
//     int M_lastDrawX = 0;
//     int M_lastDrawY = 0;
//
// };


// Helper function to make instantiation clean
// ftxui::Component ColorPicker(ftxui::Color* color) {
//     return ftxui::Make<ColorPickerBase>(color);
// }

int main( int argc, char** argv )
{
    using namespace std::chrono_literals;
    using namespace ftxui;

    auto screen = ScreenInteractive::Fullscreen();
    screen.ForceHandleCtrlZ(false);
    screen.ForceHandleCtrlC(true);

    auto menu = Termisprite::Menu();
    auto editorCanvas = Termisprite::EditorCanvas( 64, 32 );

    auto toolsSection = Termisprite::ToolsSection( editorCanvas->currentState() );
    auto colorSection = Termisprite::ColorSection( editorCanvas->currentState() );

    Component toolsContainer = Container::Vertical({ toolsSection, colorSection });

    Component statusBar = Termisprite::StatusBar();

    Component masterContainer = Container::Vertical({
        menu,
        Container::Horizontal({
            editorCanvas,
            toolsContainer
        }),
        statusBar
    });

    auto layout = Renderer( masterContainer, [&]() {
        return vbox( {
            menu->Render(),
            hbox({
                vbox({
                    editorCanvas->Render(),
                    filler()
                }) | flex,
                vbox({
                    toolsSection->Render(),
                    colorSection->Render()
                })
            }),
            statusBar->Render()
        } );

    } );


    screen.Loop(layout);



#if 0

    // ====== BRUSHES ========= //
    int selectedBrush = 0;
    std::vector<std::string> brushes = { "█","▓","▒","░"," "};

    Component brushSelector = RadioSelector( &brushes, &selectedBrush, "Brushes" );
    //=======================================//

    //========== COLORS ==================//

    ftxui::Color activeColor = ftxui::Color::White; // State to hold the current color
    auto colorPicker = ColorPicker(&activeColor);

    // ======= EDITOR CANVAS ============= //
    auto editorCanvas = std::make_shared<EditorCanvas>( true, true );

    // ====== EDITOR ========= //


    Component toolsContainer = Container::Vertical({ brushSelector, colorPicker });

    // Create the side panel renderer with a live preview
    auto sidePanelRenderer = Renderer(toolsContainer, [&]() {
        return vbox({
            brushSelector->Render(),
            window(text(" TrueColors: 24bits "), vbox({
                colorPicker->Render(),
                separator(),
                // Live preview block
                text("   PREVIEW   ") 
                    | bold 
                    | center 
                    | bgcolor(activeColor)
                    | color(Color::White) // Adjust contrast if needed
            }))
        });
    });
    Component editorContainer = Container::Horizontal( { editorCanvas, sidePanelRenderer } );

    //=======================================//

    //================== TABS ==================
    int selectedTab = 0;
    std::vector<std::string> inputTabs = { "Editor" };

    Component tabsMenu = Menu( &inputTabs, &selectedTab );

    Component tabsContainer = Container::Tab( { editorContainer }, &selectedTab );

    //=======================================//

    Component masterContainer = Container::Horizontal({ tabsMenu, tabsContainer });

    auto layout = Renderer( masterContainer, [&tabsMenu, &tabsContainer, editorCanvas, &brushes, &selectedBrush, &activeColor]() {

        editorCanvas->setBrush( brushes[selectedBrush] );
        editorCanvas->setColor( activeColor );

        return vbox( {
            tabsMenu->Render(),
            separator(),
            tabsContainer->Render() | flex
        } );
    } );


    screen.Loop(layout);

#endif

    return 0;
}

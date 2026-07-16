//!

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include "termisprite.hpp"


int main( int argc, char** argv )
{
    using namespace std::chrono_literals;
    using namespace ftxui;

    auto screen = ScreenInteractive::Fullscreen();
    screen.ForceHandleCtrlZ(false);
    screen.ForceHandleCtrlC(true);

    auto termispriteApp = std::make_shared<Termisprite::Termisprite>();

    screen.Loop(termispriteApp);

    return 0;
}

#pragma once

#include <ftxui/screen/color.hpp>



namespace Termisprite::Color
{


struct Palette {
    ftxui::Color background;
    ftxui::Color foreground;
    ftxui::Color accent;
    ftxui::Color borderFocused;
};

inline const Palette DarkPalette{
    .background = ftxui::Color::Black,
    .foreground = ftxui::Color::White,
    .accent = ftxui::Color::Blue,
    .borderFocused = ftxui::Color::BlueLight,
};

inline const Palette LightPalette{
    .background = ftxui::Color::White,
    .foreground = ftxui::Color::Black,
    .accent = ftxui::Color::Blue,
    .borderFocused = ftxui::Color::Blue,
};


class Theme {
public:
    Palette colors = DarkPalette;

    static Theme& Current() {
        static Theme instance;
        return instance;
    }

    static void Set(Theme theme) {
        Current() = std::move(theme);
    }
};


}


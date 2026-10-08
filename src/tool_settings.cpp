
#include "tool_settings.hpp"
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>


namespace Termisprite
{

ftxui::Component
BrushCharacterInput( BrushTool & brushTool )
{
    return ftxui::Input(&brushTool.selectedChar(), "Symbol", {
        .transform = []( ftxui::InputState const& state) {
            if ( state.focused )
                return state.element | ftxui::inverted | ftxui::bold | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2);
            return state.element | ftxui::color(ftxui::Color::White) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2);

        },
        .multiline = false,
        .insert = false,
        .on_change = [&brushTool] {
            std::wstring wbrush = ftxui::to_wstring(brushTool.selectedChar());
            if ( wbrush.empty() )
                brushTool.selectedChar() = "█";
            else
            {
                wbrush = wbrush.substr(wbrush.length() - 1, 1);
                brushTool.selectedChar() = ftxui::to_string(wbrush);
            }
            brushTool.currentChar() = brushTool.selectedChar();
        },
        .on_enter = [&brushTool] {
            if ( brushTool.selectedChar().empty() )
                brushTool.selectedChar() = "█";
            brushTool.currentChar() = brushTool.selectedChar();
        },
        .cursor_position = brushTool.selectedChar().size(),
    } );

}

ftxui::Component
SpinnerInput( int & target , int min = 1, int max = 10 )
{
    ftxui::ButtonOption spinnerOpt = ftxui::ButtonOption::Ascii();
    spinnerOpt.transform = [](const ftxui::EntryState& s) {
        auto element = ftxui::text(s.label);
        if (s.focused)
            return element | ftxui::bold | ftxui::inverted;
        return element | ftxui::color(ftxui::Color::White);
    };

    auto decr = ftxui::Button(" - ", [&target,min]{ if (target > min) target--; }, spinnerOpt);
    auto incr = ftxui::Button(" + ", [&target,max]{ if (target < max) target++; }, spinnerOpt);

    auto spinnerContainer = ftxui::Container::Horizontal({ decr, incr });

    return ftxui::Renderer(spinnerContainer, [&target,spinnerContainer] {
        return ftxui::hbox({
            ftxui::text("   "),
            ftxui::text(std::to_string(target) + " px") | ftxui::color(ftxui::Color::White) | ftxui::center,
            ftxui::separatorEmpty(),
            spinnerContainer->Render()
        });
    });

}

bool
ToolSettingsComponent::OnEvent( ftxui::Event event )
{
    if (event == ftxui::Event::Tab || event == ftxui::Event::TabReverse)
        return false;

    return ftxui::ComponentBase::OnEvent(event);
}



BrushSettingsComponent::BrushSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( brushTool, shortcutManager ), M_brushSize( brushTool.size() )
{
    M_brushCharInput = BrushCharacterInput( brushTool );
    M_sizeSpinner = SpinnerInput( M_brushSize, 1, 10 );
    M_container = ftxui::Container::Vertical({ M_brushCharInput, M_sizeSpinner });
    ftxui::ComponentBase::Add( M_container );
}

ftxui::Element
BrushSettingsComponent::OnRender()
{
    using namespace ftxui;

    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    return window( text(" Brush Settings ") | bold | center,
        vbox({
            vbox({
                text("Brush ") | dim | color( ftxui::Color::White ),
                M_brushCharInput->Render() | border | size( WIDTH, EQUAL, 2),
                text("Size ") | dim | color( Color::White ),
                hbox({ text("   "), separatorEmpty(), M_sizeSpinner->Render() })
            })
        })
    ) | color( borderColor );
}




EraserSettingsComponent::EraserSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( brushTool, shortcutManager ), M_brushSize( brushTool.size() )
{
    M_sizeSpinner = SpinnerInput( M_brushSize, 1, 10 );
    M_container = ftxui::Container::Vertical({ M_sizeSpinner });
    ftxui::ComponentBase::Add( M_container );
}

ftxui::Element
EraserSettingsComponent::OnRender()
{
    using namespace ftxui;

    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    return window( text(" Eraser Settings ") | bold | center,
        vbox({
            vbox({
                text("Size ") | dim | color( Color::White ),
                hbox({ text("   "), separatorEmpty(), M_sizeSpinner->Render() })
            })
        })
    ) | color( borderColor );
}


RectangleSettingsComponent::RectangleSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( brushTool, shortcutManager ), M_brushSize( brushTool.size() )
{
    M_sizeSpinner = SpinnerInput( M_brushSize, 1, 10 );
    M_container = ftxui::Container::Vertical({ M_sizeSpinner });
    ftxui::ComponentBase::Add( M_container );
}

ftxui::Element
RectangleSettingsComponent::OnRender()
{
    using namespace ftxui;

    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    return window( text(" Rectangle Settings ") | bold | center,
        vbox({
            vbox({
                text("Size ") | dim | color( Color::White ),
                hbox({ text("   "), separatorEmpty(), M_sizeSpinner->Render() })
            })
        })
    ) | color( borderColor );
}


LineSettingsComponent::LineSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( brushTool, shortcutManager ), M_brushSize( brushTool.size() )
{
    M_sizeSpinner = SpinnerInput( M_brushSize, 1, 10 );
    M_container = ftxui::Container::Vertical({ M_sizeSpinner });
    ftxui::ComponentBase::Add( M_container );
}

ftxui::Element
LineSettingsComponent::OnRender()
{
    using namespace ftxui;

    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    return window( text(" Line Settings ") | bold | center,
        vbox({
            vbox({
                text("Size ") | dim | color( Color::White ),
                hbox({ text("   "), separatorEmpty(), M_sizeSpinner->Render() })
            })
        })
    ) | color( borderColor );
}


EllipseSettingsComponent::EllipseSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( brushTool, shortcutManager ), M_brushSize( brushTool.size() )
{
    M_sizeSpinner = SpinnerInput( M_brushSize, 1, 10 );
    M_container = ftxui::Container::Vertical({ M_sizeSpinner });
    ftxui::ComponentBase::Add( M_container );
}

ftxui::Element
EllipseSettingsComponent::OnRender()
{
    using namespace ftxui;

    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    return window( text(" Ellipse Settings ") | bold | center,
        vbox({
            vbox({
                text("Size ") | dim | color( Color::White ),
                hbox({ text("   "), separatorEmpty(), M_sizeSpinner->Render() })
            })
        })
    ) | color( borderColor );
}


PaintFillSettingsComponent::PaintFillSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( brushTool, shortcutManager )
{
    // ftxui::ComponentBase::Add( M_container );
}

ftxui::Element
PaintFillSettingsComponent::OnRender()
{
    using namespace ftxui;

    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    return window( text(" Paint Fill Settings ") | bold | center,
        vbox({
            vbox({
                text("")
                // text("Size ") | dim | color( Color::White ),
                // hbox({ text("   "), separatorEmpty(), M_sizeSpinner->Render() })
            })
        })
    ) | color( borderColor );
}


BoxSelectSettingsComponent::BoxSelectSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( brushTool, shortcutManager )
{
    // ftxui::ComponentBase::Add( M_container );
}

ftxui::Element
BoxSelectSettingsComponent::OnRender()
{
    using namespace ftxui;

    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    return window( text(" Box Select Settings ") | bold | center,
        vbox({
            vbox({
                text("")
                // text("Size ") | dim | color( Color::White ),
                // hbox({ text("   "), separatorEmpty(), M_sizeSpinner->Render() })
            })
        })
    ) | color( borderColor );
}


std::shared_ptr<BrushSettingsComponent>
BrushSettings( BrushTool & brushTool, ShortcutManager * shortcutManager )
{
    return std::make_shared<BrushSettingsComponent>( brushTool, shortcutManager );
};


std::shared_ptr<EraserSettingsComponent>
EraserSettings( BrushTool & brushTool, ShortcutManager * shortcutManager )
{
    return std::make_shared<EraserSettingsComponent>( brushTool, shortcutManager );
};

std::shared_ptr<RectangleSettingsComponent>
RectangleSettings( BrushTool & brushTool, ShortcutManager * shortcutManager )
{
    return std::make_shared<RectangleSettingsComponent>( brushTool, shortcutManager );
};

std::shared_ptr<EllipseSettingsComponent>
EllipseSettings( BrushTool & brushTool, ShortcutManager * shortcutManager )
{
    return std::make_shared<EllipseSettingsComponent>( brushTool, shortcutManager );
};

std::shared_ptr<LineSettingsComponent>
LineSettings( BrushTool & brushTool, ShortcutManager * shortcutManager )
{
    return std::make_shared<LineSettingsComponent>( brushTool, shortcutManager );
};

std::shared_ptr<PaintFillSettingsComponent>
PaintFillSettings( BrushTool & brushTool, ShortcutManager * shortcutManager )
{
    return std::make_shared<PaintFillSettingsComponent>( brushTool, shortcutManager );
};

std::shared_ptr<BoxSelectSettingsComponent>
BoxSelectSettings( BrushTool & brushTool, ShortcutManager * shortcutManager )
{
    return std::make_shared<BoxSelectSettingsComponent>( brushTool, shortcutManager );
};


}


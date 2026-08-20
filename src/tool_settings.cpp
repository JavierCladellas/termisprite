
#include "tool_settings.hpp"
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>


namespace Termisprite
{

ftxui::Component
BrushCharacterInput( EditorState & editorState )
{
    return ftxui::Input(&editorState.selectedBrush, "Symbol", {
        .transform = []( ftxui::InputState const& state) {
            if ( state.focused )
                return state.element | ftxui::inverted | ftxui::bold | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2);
            return state.element | ftxui::color(ftxui::Color::White) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2);

        },
        .multiline = false,
        .insert = false,
        .on_change = [&editorState] {
            std::wstring wbrush = ftxui::to_wstring(editorState.selectedBrush);
            if ( wbrush.empty() )
                editorState.selectedBrush = "█";
            else
            {
                wbrush = wbrush.substr(wbrush.length() - 1, 1);
                editorState.selectedBrush = ftxui::to_string(wbrush);
            }
            editorState.brush = editorState.selectedBrush;
        },
        .on_enter = [&editorState] {
            if ( editorState.selectedBrush.empty() )
                editorState.selectedBrush = "█";
            editorState.brush = editorState.selectedBrush;
        },
        .cursor_position = editorState.selectedBrush.size(),
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



BrushSettingsComponent::BrushSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( editorState, shortcutManager ), M_brushSize( editorState.brushSize )
{
    M_brushCharInput = BrushCharacterInput( editorState );
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




EraserSettingsComponent::EraserSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( editorState, shortcutManager ), M_brushSize( editorState.brushSize )
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


RectangleSettingsComponent::RectangleSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( editorState, shortcutManager ), M_brushSize( editorState.brushSize )
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


LineSettingsComponent::LineSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( editorState, shortcutManager ), M_brushSize( editorState.brushSize )
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


EllipseSettingsComponent::EllipseSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( editorState, shortcutManager ), M_brushSize( editorState.brushSize )
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


PaintFillSettingsComponent::PaintFillSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( editorState, shortcutManager )
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


BoxSelectSettingsComponent::BoxSelectSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager )
    : ToolSettingsComponent( editorState, shortcutManager )
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
BrushSettings( EditorState & editorState, ShortcutManager * shortcutManager )
{
    return std::make_shared<BrushSettingsComponent>( editorState, shortcutManager );
};


std::shared_ptr<EraserSettingsComponent>
EraserSettings( EditorState & editorState, ShortcutManager * shortcutManager )
{
    return std::make_shared<EraserSettingsComponent>( editorState, shortcutManager );
};

std::shared_ptr<RectangleSettingsComponent>
RectangleSettings( EditorState & editorState, ShortcutManager * shortcutManager )
{
    return std::make_shared<RectangleSettingsComponent>( editorState, shortcutManager );
};

std::shared_ptr<EllipseSettingsComponent>
EllipseSettings( EditorState & editorState, ShortcutManager * shortcutManager )
{
    return std::make_shared<EllipseSettingsComponent>( editorState, shortcutManager );
};

std::shared_ptr<LineSettingsComponent>
LineSettings( EditorState & editorState, ShortcutManager * shortcutManager )
{
    return std::make_shared<LineSettingsComponent>( editorState, shortcutManager );
};

std::shared_ptr<PaintFillSettingsComponent>
PaintFillSettings( EditorState & editorState, ShortcutManager * shortcutManager )
{
    return std::make_shared<PaintFillSettingsComponent>( editorState, shortcutManager );
};

std::shared_ptr<BoxSelectSettingsComponent>
BoxSelectSettings( EditorState & editorState, ShortcutManager * shortcutManager )
{
    return std::make_shared<BoxSelectSettingsComponent>( editorState, shortcutManager );
};


}


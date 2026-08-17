
#include "brush_settings.hpp"
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>


namespace Termisprite
{

BrushSettingsComponent::BrushSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager )
    : M_editorState( editorState ), M_shortcutManager( shortcutManager )
{
    M_brushCharInput = ftxui::Input(&M_editorState.brush, "Symbol", {
        .transform = []( ftxui::InputState const& state) {
            auto elt = state.element;
            elt |= ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2);
            if ( state.focused )
                return elt | ftxui::inverted | ftxui::bold;
            return elt | ftxui::color(ftxui::Color::White);

        },
        .multiline = false,
        .on_change = [this] {
            std::wstring wbrush = ftxui::to_wstring(M_editorState.brush);
            if ( wbrush.empty() )
                M_editorState.brush = "█";
            else
            {
                wbrush = wbrush.substr(wbrush.length() - 1, 1);
                M_editorState.brush = ftxui::to_string(wbrush);
            }
            M_cursorPos = M_editorState.brush.size();
        },
        .on_enter = [this] {
            if ( M_editorState.brush.empty() )
                M_editorState.brush = "█";
            M_cursorPos = M_editorState.brush.size();
        },
        .cursor_position = &M_cursorPos,
    } );

    ftxui::ButtonOption spinnerOpt = ftxui::ButtonOption::Ascii();
    spinnerOpt.transform = [](const ftxui::EntryState& s) {
        auto element = ftxui::text(s.label);
        if (s.focused)
            return element | ftxui::bold | ftxui::inverted;
        return element | ftxui::color(ftxui::Color::White);
    };

    M_btnSizeDec = ftxui::Button(" - ", [this]{ if (M_editorState.brushSize > 1) M_editorState.brushSize--; }, spinnerOpt);
    M_btnSizeInc = ftxui::Button(" + ", [this]{ if (M_editorState.brushSize < 10) M_editorState.brushSize++; }, spinnerOpt);

    M_container = ftxui::Container::Vertical({ M_brushCharInput, M_btnSizeDec, M_btnSizeInc });

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
                text("Character ") | dim | ftxui::color( ftxui::Color::White ),
                M_brushCharInput->Render() | ftxui::border,
                text("Size ") | dim | ftxui::color( ftxui::Color::White ),
                hbox({
                    text("   "),
                    text(std::to_string(M_editorState.brushSize) + " px") | ftxui::color(ftxui::Color::White) | center,
                    separatorEmpty(),
                    M_btnSizeDec->Render(),
                    M_btnSizeInc->Render()
                })
            })
        })
    ) | color( borderColor );
}

bool
BrushSettingsComponent::OnEvent( ftxui::Event event )
{
    if (event == ftxui::Event::Tab || event == ftxui::Event::TabReverse)
        return false;

    return ftxui::ComponentBase::OnEvent(event);
}




std::shared_ptr<BrushSettingsComponent>
BrushSettings( EditorState & editorState, ShortcutManager * shortcutManager )
{
    return std::make_shared<BrushSettingsComponent>( editorState, shortcutManager );
};

}


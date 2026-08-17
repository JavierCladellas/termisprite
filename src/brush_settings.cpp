
#include "brush_settings.hpp"


namespace Termisprite
{

BrushSettingsComponent::BrushSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager )
    : M_editorState( editorState ), M_shortcutManager( shortcutManager )
{
    M_brushCharInput = ftxui::Input(&M_editorState.brush, "Symbol");

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
                M_brushCharInput->Render() | ftxui::color(ftxui::Color::White) | border,
                text("Dimensions ") | dim | ftxui::color( ftxui::Color::White ),
                hbox({
                    text(std::to_string(M_editorState.brushSize) + "px") | ftxui::color(ftxui::Color::White) | center,
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


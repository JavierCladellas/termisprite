#pragma once

#include "editor.hpp"
#include "shortcuts.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>



namespace Termisprite
{

class BrushSettingsComponent
    : public ftxui::ComponentBase
{
public:
    BrushSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager );

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

private:
    ftxui::Component M_brushCharInput;

    ftxui::Component M_btnSizeInc;
    ftxui::Component M_btnSizeDec;

    ftxui::Component M_container;

    EditorState & M_editorState;
    ShortcutManager * M_shortcutManager;

};


std::shared_ptr<BrushSettingsComponent> BrushSettings( EditorState & editorState, ShortcutManager * shortcutManager );


}


#pragma once

#include "editor.hpp"
#include "shortcuts.hpp"
#include <ftxui/component/component_base.hpp>

namespace Termisprite
{




class LayersComponent
    : public ftxui::ComponentBase
{
public:
    LayersComponent( EditorState & editorState, ShortcutManager * shortcutManager );

    ftxui::Element OnRender() override;
    // bool OnEvent( ftxui::Event event ) override;

private:

private:
    ftxui::Component M_container;
    EditorState & M_editorState;
    ShortcutManager * M_shortcutManager;

};


std::shared_ptr<LayersComponent> LayersSection(  EditorState & editorState, ShortcutManager * shortcutManager  );



}



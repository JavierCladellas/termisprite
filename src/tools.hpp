#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include "editor.hpp"
#include "shortcuts.hpp"


namespace Termisprite
{


class ToolsComponent
    : public ftxui::ComponentBase
{

public:
    ToolsComponent( EditorState & editorState, ShortcutManager * shortcutManager );

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

    void selectTool( ToolType type );
private:
    ftxui::Component makeToolButton(std::string icon, Shortcut const& shortcut, ToolType type);

private:

    ftxui::Component M_container;
    EditorState & M_editorState;
    ShortcutManager * M_shortcutManager;

};


std::shared_ptr<ToolsComponent> ToolsSection( EditorState & editorState, ShortcutManager * shortcutManager );


}

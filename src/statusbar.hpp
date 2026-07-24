#pragma once

#include "editor.hpp"
#include "shortcuts.hpp"
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <map>


namespace Termisprite
{


class StatusBarComponent
    : public ftxui::ComponentBase
{
public:
    StatusBarComponent( EditorState & editorState, ShortcutManager * shortcutManager );

    ftxui::Element OnRender() override;

private:
    std::string toolTypeToString( ToolType type ) const;

private:
    EditorState & M_editorState;
    ShortcutManager * M_shortcutManager;

    std::map<std::pair<std::string,std::string>, std::function<bool(EditorState &)>> M_displayedShortcuts;

};


std::shared_ptr<StatusBarComponent> StatusBar( EditorState & editorState, ShortcutManager * shortcutManager );


}

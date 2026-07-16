#pragma once

#include "editor.hpp"
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>


namespace Termisprite
{

struct Shortcut
{
    std::string keys;
    std::string action;
    std::function<bool(const EditorState&)> condition;
};

class StatusBarComponent
    : public ftxui::ComponentBase
{
public:
    StatusBarComponent( EditorState & editorState );

    ftxui::Element OnRender() override;

private:
    std::string toolTypeToString( ToolType type ) const;
    void initializeShortcuts();

private:
    EditorState & M_editorState;

    std::vector<Shortcut> M_shortcuts;

};


std::shared_ptr<StatusBarComponent> StatusBar( EditorState & editorState );


}

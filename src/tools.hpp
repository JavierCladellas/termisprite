#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <optional>
#include "editor.hpp"


namespace Termisprite
{


class ToolsComponent
    : public ftxui::ComponentBase
{

public:
    ToolsComponent( EditorState & editorState );

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

private:
    ftxui::Component makeToolButton(std::string icon, std::string name, char shortcut, ToolType type);
    void selectTool( ToolType type );

private:

    ftxui::Component M_container;
    EditorState & M_editorState;

};


std::shared_ptr<ToolsComponent> ToolsSection( EditorState & editorState );


}

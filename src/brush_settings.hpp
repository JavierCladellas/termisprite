#pragma once

#include "editor.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>



namespace Termisprite
{

class BrushSettingsComponent
    : public ftxui::ComponentBase
{
public:
    BrushSettingsComponent( EditorState & editorState )
        : M_editorState( editorState )
    {
        ftxui::ComponentBase::Add(
            ftxui::Container::Vertical({
            })
        );
    }

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

private:
    EditorState & M_editorState;

};


}


#pragma once

#include "editor.hpp"
#include "shortcuts.hpp"
#include <ftxui/component/component_base.hpp>


namespace Termisprite
{


class AnimationsPanelComponent
    : public ftxui::ComponentBase
{
public:
    AnimationsPanelComponent( EditorCanvasComponent * editorCanvas, ShortcutManager * shortcutManager )
        : M_editorCanvas( editorCanvas ), M_shortcutManager( shortcutManager )
    {
        M_loopCheckbox = ftxui::Checkbox("Loop", &M_loop);

        ftxui::ComponentBase::Add(M_loopCheckbox);

    }

    ftxui::Element OnRender() override;
    // bool OnEvent(ftxui::Event event) override;

private:
    EditorCanvasComponent * M_editorCanvas;
    ShortcutManager * M_shortcutManager;

    //TODO: This shoudl be handled by editor canvas?
    bool M_loop = true;
    ftxui::Component M_loopCheckbox;

};



std::shared_ptr<AnimationsPanelComponent>
AnimationsPanel( EditorCanvasComponent * editorCanvas, ShortcutManager * shortcutManager );

}

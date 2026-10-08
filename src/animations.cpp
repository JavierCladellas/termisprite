#include "animations.hpp"


namespace Termisprite
{



ftxui::Element
AnimationsPanelComponent::OnRender()
{
    using namespace ftxui;

    Color borderColor = Focused() ? Color::Cyan : Color::White;

    return window ( text(" Frames ") | bold | center,
        vbox({
            hbox({
                text("idle"), // text(M_editorCanvas->currentState().animationName)
                separatorEmpty(),
                text("FPS: ") | bold,
                text("12"), // text(std::to_string(M_editorCanvas->currentState().fps
                separatorEmpty(),
                M_loopCheckbox->Render(),
            }),

            //Frames TODO
            hbox({
                text("1"), text("2"), text("3"), text("4"), text("5"), text("6"), text("7"), text("8"), text("9"), text("10")
            })
        })
    ) | color( borderColor );

}


std::shared_ptr<AnimationsPanelComponent>
AnimationsPanel( EditorCanvasComponent * editorCanvas, ShortcutManager * shortcutManager )
{
    return std::make_shared<AnimationsPanelComponent>( editorCanvas, shortcutManager );
}


}



#include "layers.hpp"

namespace Termisprite
{



LayersComponent::LayersComponent( EditorState & editorState, ShortcutManager * shortcutManager )
    : M_editorState( editorState ), M_shortcutManager( shortcutManager )
{}

ftxui::Element
LayersComponent::OnRender()
{
    using namespace ftxui;

    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    return window( text(" Layers ") | bold | center,
        vbox({
            ftxui::emptyElement()
        })
    ) | ftxui::color( borderColor );

}






std::shared_ptr<LayersComponent>
LayersSection(  EditorState & editorState, ShortcutManager * shortcutManager  )
{
    return std::make_shared<LayersComponent>( editorState, shortcutManager );
}



}



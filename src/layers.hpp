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
    LayersComponent( EditorCanvasComponent * editor, ShortcutManager * shortcutManager );

    ftxui::Element OnRender() override;
    // bool OnEvent( ftxui::Event event ) override;

private:

    void rebuild();
    ftxui::Component createLayerRow( int index );

private:
    EditorCanvasComponent * M_editor;
    ShortcutManager * M_shortcutManager;

};


std::shared_ptr<LayersComponent> LayersSection( EditorCanvasComponent * editor, ShortcutManager * shortcutManager );



}



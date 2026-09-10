#pragma once

#include "editor.hpp"
#include "shortcuts.hpp"
#include <ftxui/component/component_base.hpp>

namespace Termisprite
{



//TODO: Refactor with contextWindow for right click in editor. Tons of repeated code here lol
class LayerContextMenuComponent
    : public ftxui::ComponentBase
{
public:
    LayerContextMenuComponent( EditorCanvasComponent * editor )
        : M_editor( editor )
    { }

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

    bool isActive() const { return M_isActive; }
    void show() { M_isActive = true; }
    void hide() { M_isActive = false; }


    void setLayerIndex( int index ) { M_layerIndex = index; }

    int & x() { return M_posX; }
    int & y() { return M_posY; }

    std::function<void(int)> onRenameAction;

private:
    EditorCanvasComponent * M_editor;
    int M_layerIndex = 0;

    int M_posX = 0;
    int M_posY = 0;

    bool M_isActive = false;

    ftxui::Box M_box;

    int M_menuIndex = 0;
    std::vector<std::string> M_menuOptions = {
        "Rename",
        "Duplicate",
        "Delete",
        "Clear",
        "Hide/Show",
        "Export",
        "Move Up",
        "Move Down",
        "Cancel [Esc]"
    };

    ftxui::Component M_menu = ftxui::Menu(&M_menuOptions, &M_menuIndex);
};

std::shared_ptr<LayerContextMenuComponent> LayerContextMenu( EditorCanvasComponent * editor );

class LayersComponent
    : public ftxui::ComponentBase
{
public:
    LayersComponent( EditorCanvasComponent * editor, ShortcutManager * shortcutManager );

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

private:

    void rebuild();
    ftxui::Component createLayerRow( int index );

private:
    EditorCanvasComponent * M_editor;
    ShortcutManager * M_shortcutManager;

    std::shared_ptr<LayerContextMenuComponent> M_contextMenu;
    ftxui::Box M_box;

    int M_renamingLayerIndex = -1;
    std::string M_renameBuffer;
    ftxui::Component M_renameInput;

};


std::shared_ptr<LayersComponent> LayersSection( EditorCanvasComponent * editor, ShortcutManager * shortcutManager );



}



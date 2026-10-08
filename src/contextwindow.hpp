#pragma once

#include "shortcuts.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>

namespace Termisprite
{

class ContextWindowComponent
    : public ftxui::ComponentBase
{
public:
    ContextWindowComponent( ShortcutManager * shortcutManager )
        : M_shortcutManager( shortcutManager )
    {}

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;


    int & x() { return M_posX; }
    int & y() { return M_posY; }

    bool isActive() const { return M_isActive; }
    void show() { M_isActive = true; }
    void hide() { M_isActive = false; }

private:
    ShortcutManager * M_shortcutManager;

    int M_posX = 0;
    int M_posY = 0;

    bool M_isActive = false;

    ftxui::Box M_box;

    int M_rightClickModalIndex = 0;
    //TODO Use shortcut Manager for this
    std::vector<std::string> M_rightClickModalOptions = {
        M_shortcutManager->getMenuLabel( ShortcutType::BACKGROUND_COLOR ),
        M_shortcutManager->getMenuLabel( ShortcutType::TOGGLE_GRID ),
        M_shortcutManager->getMenuLabel( ShortcutType::CHANGE_GRID_TYPE ),
        M_shortcutManager->getMenuLabel( ShortcutType::UNDO ),
        M_shortcutManager->getMenuLabel( ShortcutType::REDO ),
        M_shortcutManager->getMenuLabel( ShortcutType::CLEAR ),
        "Cancel [Esc]"
    };
    ftxui::Component M_rightClickModal = ftxui::Menu(&M_rightClickModalOptions, &M_rightClickModalIndex);
};


std::shared_ptr<ContextWindowComponent> ContextWindow( ShortcutManager * shortcutManager );

}

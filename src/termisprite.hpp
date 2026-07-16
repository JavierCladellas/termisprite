#pragma once

#include "colorpicker.hpp"
#include "editor.hpp"
#include "menu.hpp"
#include "statusbar.hpp"
#include "tools.hpp"

#include <ftxui/component/component_base.hpp>
#include <functional>


namespace Termisprite
{


class ResizeModal
    : public ftxui::ComponentBase
{
public:
    ResizeModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose);

    ftxui::Element OnRender() override;

private:
    EditorCanvasComponent & M_editorCanvas;
    std::function<void()> M_closeCallback;

    std::string M_widthInput;
    std::string M_heightInput;

    ftxui::Component M_widthInputComponent = ftxui::Input(&M_widthInput, "Width");
    ftxui::Component M_heightInputComponent = ftxui::Input(&M_heightInput, "Height");

    ftxui::Component M_okButton = ftxui::Button("OK", [this] {
        M_editorCanvas.resize(std::stoi(M_widthInput), std::stoi(M_heightInput));
        M_closeCallback();
    });
    ftxui::Component M_cancelButton = ftxui::Button("Cancel", [this] { M_closeCallback(); });

};


class AboutModal
    : public ftxui::ComponentBase
{
public:
    AboutModal( std::function<void()> onClose );

    ftxui::Element OnRender() override;

private:
    std::function<void()> M_closeCallback;
    ftxui::Component M_closeButton;
};

class ShortcutsModal
    : public ftxui::ComponentBase
{
public:
    ShortcutsModal( std::function<void()> onClose );

    ftxui::Element OnRender() override;

private:
    std::function<void()> M_closeCallback;
    ftxui::Component M_closeButton;
};



class Termisprite
    : public ftxui::ComponentBase
{
public:
    Termisprite();

    ftxui::Element OnRender() override;

    bool OnEvent( ftxui::Event event ) override;

private:

    bool processHistoryEvents( ftxui::Event event );
    bool processClipboardEvents( ftxui::Event event );
    bool processToggleGrid( ftxui::Event event );
    bool processClearSelection( ftxui::Event event );
    bool processToolSelection( ftxui::Event event );


private:
    ftxui::Component M_masterComponent;

    std::shared_ptr<EditorCanvasComponent> M_editorCanvas;
    std::shared_ptr<MenuComponent> M_menu;
    std::shared_ptr<ToolsComponent> M_tools;
    std::shared_ptr<ColorSectionComponent> M_colorSection;
    std::shared_ptr<StatusBarComponent> M_statusBar;

    bool M_showResizeModal = false;
    std::shared_ptr<ResizeModal> M_resizeModal;

    bool M_showAboutModal = false;
    std::shared_ptr<AboutModal> M_aboutModal;

    bool M_showShortcutsModal = false;
    std::shared_ptr<ShortcutsModal> M_shortcutsModal;
};


}

#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <map>
#include "shortcuts.hpp"


namespace Termisprite
{


enum class ToolType
{
    DRAW,
    ERASER,
    SQUARE,
    CIRCLE,
    LINE,
    EYE_DROPPER,
    PAINT_FILL,
    BOX_SELECT,
    PAN,
    MOVE_LAYER
};

class ToolsComponent
    : public ftxui::ComponentBase
{

public:
    ToolsComponent(ToolType & activeTool, ShortcutManager * shortcutManager );

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

private:
    ftxui::Component makeToolButton(std::string icon, Shortcut const& shortcut, ToolType type );

private:

    ftxui::Component M_container;
    ToolType & M_activeTool;
    ShortcutManager * M_shortcutManager;

    //TODO THIS NEEDS TO BE FIXED....
    const std::map<ToolType, ShortcutType > M_toolToAction = {
        { ToolType::DRAW, ShortcutType::SELECT_BRUSH_TOOL },
        { ToolType::ERASER, ShortcutType::SELECT_ERASER_TOOL },
        { ToolType::SQUARE, ShortcutType::SELECT_RECTANGLE_TOOL },
        { ToolType::CIRCLE, ShortcutType::SELECT_ELLIPSE_TOOL },
        { ToolType::LINE, ShortcutType::SELECT_LINE_TOOL },
        { ToolType::EYE_DROPPER, ShortcutType::SELECT_EYE_DROPPER_TOOL },
        { ToolType::PAINT_FILL, ShortcutType::SELECT_PAINT_FILL_TOOL },
        { ToolType::BOX_SELECT, ShortcutType::SELECT_BOX_SELECT_TOOL },
        { ToolType::PAN, ShortcutType::TOGGLE_PAN },
        { ToolType::MOVE_LAYER, ShortcutType::MOVE_LAYER }
    };

};


std::shared_ptr<ToolsComponent> ToolsSection( ToolType & activeTool, ShortcutManager * shortcutManager );


}

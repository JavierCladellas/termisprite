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
    enum class Category
    {
        DRAWING,
        SHAPES,
        TOOLS
    };

public:
    ToolsComponent( EditorState & editorState );

    ftxui::Element OnRender() override;

private:
    void updateState();

private:
    //Drawing
    std::vector<std::string> M_drawingModes{ "Draw","Eraser" };
    int M_drawingIndex = 0;
    ftxui::Component M_drawingMenu;

    //Shapes
    std::vector<std::string> M_shapeModes{ "Line","Rectangle","Ellipse" };
    int M_shapeIndex = 0;
    ftxui::Component M_shapeMenu;

    //Tools
    std::vector<std::string> M_toolModes{ "Eye Dropper","Paint Fill","Box Select" };
    int M_toolIndex = 0;
    ftxui::Component M_toolMenu;


    ftxui::Component M_container;

    EditorState & M_editorState;

    Category M_activeCategory = Category::DRAWING;

};


std::shared_ptr<ToolsComponent> ToolsSection( EditorState & editorState );


}

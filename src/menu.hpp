#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>


namespace Termisprite
{


struct MenuCategory
{
    std::string name;
    std::vector<std::string> options;
    std::function<void(int)> onSelect;
};

class MenuComponent
    : public ftxui::ComponentBase
{
public:
    MenuComponent( std::vector<MenuCategory> categories );

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

    ftxui::Element RenderOverlay();

private:
    ftxui::Component M_buttonsContainer;

    std::vector<MenuCategory> M_categories;
    std::vector<ftxui::Component> M_dropdownMenus;
    std::vector<int> M_selectedIndices;

    std::vector<ftxui::Box> M_buttonBoxes;
    ftxui::Box M_dropdownBox;

    int M_activeDropdown = -1;
};

std::shared_ptr<MenuComponent> Menu( std::vector<MenuCategory> categories );

}

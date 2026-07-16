#include <ftxui/dom/elements.hpp>

#include "menu.hpp"


namespace Termisprite
{

ftxui::Element
MenuItemBase::OnRender()
{
    return M_button->Render();
}


ftxui::Component MenuItem( std::string const& label, std::function<void()> onClick )
{
    return std::make_shared<MenuItemBase>( label, onClick );
}


MenuComponent::MenuComponent()
{
    M_menuContainer = ftxui::Container::Horizontal({
        MenuItem("File"),
        MenuItem("Edit"),
        MenuItem("Canvas"),
        MenuItem("View"),
        MenuItem("Tools"),
        MenuItem("Help")
    } );

    ftxui::ComponentBase::Add( M_menuContainer );
}

ftxui::Element
MenuComponent::OnRender()
{
    return M_menuContainer->Render() | ftxui::xflex;
}


std::shared_ptr<MenuComponent>
Menu()
{
    return std::make_shared<MenuComponent>();
}

}


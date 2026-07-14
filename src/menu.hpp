#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>


namespace Termisprite
{


class MenuItemBase
    : public ftxui::ComponentBase
{
public:
    MenuItemBase( std::string const& label, std::function<void()> onClick = [](){} )
        : M_label( std::move( label ) ), M_onClick( std::move( onClick ) ),
          M_button ( ftxui::Button( M_label, M_onClick, ftxui::ButtonOption::Ascii() ) )
    {
        ftxui::ComponentBase::Add( M_button );
    }

    ftxui::Element OnRender() override
    {
        return M_button->Render();
    }


    bool OnEvent( ftxui::Event event ) override
    {
        return M_button->OnEvent( event );
    }

private:
    std::string M_label;
    std::function<void()> M_onClick;
    ftxui::Component M_button;
};

ftxui::Component MenuItem( std::string const& label, std::function<void()> onClick = [](){} )
{
    auto menuItem = std::make_shared<MenuItemBase>( label, onClick );
    return menuItem;
}



class MenuComponent
    : public ftxui::ComponentBase
{
public:
    MenuComponent()
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
    };

    ftxui::Element OnRender() override
    {
        return M_menuContainer->Render() | ftxui::xflex;
    }

    bool OnEvent( ftxui::Event event ) override
    {
        return M_menuContainer->OnEvent( event );
    }

private:
    ftxui::Component M_menuContainer;
};


ftxui::Component Menu()
{
    return std::make_shared<MenuComponent>();
}


}

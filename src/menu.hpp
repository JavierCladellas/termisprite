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

    ftxui::Element OnRender() override;

private:
    std::string M_label;
    std::function<void()> M_onClick;
    ftxui::Component M_button;
};

ftxui::Component MenuItem( std::string const& label, std::function<void()> onClick = [](){} );


class MenuComponent
    : public ftxui::ComponentBase
{
public:
    MenuComponent();

    ftxui::Element OnRender() override;

private:
    ftxui::Component M_menuContainer;
};


std::shared_ptr<MenuComponent> Menu();


}

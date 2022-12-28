/*
paw_gui_resource_dropdown.cpp
This file is part of:
GAME PENCIL ENGINE
https://www.pawbyte.com/gamepencilengine
Copyright (c) 2014-2020 Nathan Hurde, Chase Lee.

Copyright (c) 2014-2020 PawByte LLC.
Copyright (c) 2014-2020 Game Pencil Engine contributors ( Contributors Page )

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-Game Pencil Engine <https://www.pawbyte.com/gamepencilengine>


*/

#include "paw_gui_resource_dropdown.h"


GPE_DropDown_Resouce_Menu::GPE_DropDown_Resouce_Menu( std::string name, GPE_GeneralResourceContainer * cTree,int id,bool selectable )
{
    guiListTypeName = "resourcedropdown";
    widget_box.x = 0;
    widget_box.y = 0;
    widget_box.w = 196;
    //int nameMinSize = name.size()*
    widget_box.h = 32;
    displayedResult = dropdownName = opName = name;
    containerTree = cTree;
    opId = id;
    isSelectable = selectable;
    selectedId = -1;
    isOpen = false;
    isClicked = false;
    justActivated = false;
    selectedResource= NULL;
}

GPE_DropDown_Resouce_Menu::~GPE_DropDown_Resouce_Menu()
{

}

std::string GPE_DropDown_Resouce_Menu::get_data()
{
    if( containerTree!=NULL)
    {
        std::string dataString = guiListTypeName+":"+dropdownName+"==|||==";
        dataString+= containerTree->get_name()+",,,";
        if( selectedResource!=NULL)
        {
            dataString+=stg_ex::int_to_string(selectedResource->get_global_id() )+",";
        }
        else
        {
            dataString+="-1,";
        }
        return dataString;
    }
    return "";
}

void GPE_DropDown_Resouce_Menu::load_data(std::string dataString)
{
    if( (int)dataString.size() > 0)
    {
        std::string projectResourceFolder = "";
        int projectselectedContainer = -1;
        projectResourceFolder = stg_ex::split_first_string(dataString,",,,");

        if( containerTree!=NULL )
        {
            GPE_GeneralResourceContainer * foundResContainer  = containerTree->find_resource_from_name(projectResourceFolder, false );
            if( foundResContainer!=NULL)
            {
                containerTree = foundResContainer;
                projectselectedContainer = stg_ex::split_first_int(dataString,',');
                set_selection( projectselectedContainer);
            }
            else
            {
                gpe::error_log->report("Can't find project resource container["+projectResourceFolder+"] using...["+project_current_name+"]");
            }
        }
        else
        {
            gpe::error_log->report("Can't find project GPE resource container using...["+project_current_name+"]");
        }
    }
}

void GPE_DropDown_Resouce_Menu::add_folder_contents(GPE_GeneralResourceContainer * fFolder, GPE_PopUpMenu_Option * fOptionFolder)
{
    if( fFolder!=NULL && fOptionFolder!=NULL)
    {
        GPE_GeneralResourceContainer * cContainer = NULL;
        GPE_PopUpMenu_Option * cOption = NULL;
        for( int i = 0; i < fFolder->get_size(); i++)
        {
            cContainer = fFolder->get_resource_at(i);
            if( cContainer!=NULL)
            {
                cOption = fOptionFolder->add_menu_option(cContainer->get_name(),cContainer->get_global_id(),cContainer->get_resource_texture(),0,cContainer->get_resource_animation(),false,true,true);
                cOption->isResourceOption = true;
                if( cContainer->is_folder() )
                {
                    cOption->isFolderOption = true;
                    add_folder_contents(cContainer,cOption);
                }
            }
        }
    }
}


GPE_GeneralResourceContainer * GPE_DropDown_Resouce_Menu::get_selected_container()
{
    if( containerTree!=NULL)
    {
        return containerTree->find_resource_from_id(selectedId,true,false);
    }
    return NULL;
}

std::string GPE_DropDown_Resouce_Menu::get_selected_name()
{
    if( containerTree!=NULL)
    {
        GPE_GeneralResourceContainer * fRes = containerTree->find_resource_from_id(selectedId,true,false);
        if( fRes!=NULL)
        {
            std::string fStr = fRes->get_name();
            if( stg_ex::is_alnum( fStr, false, true) )
            {
                return fStr;
            }
            else
            {
                //defaults to the object's id instead.
                return stg_ex::int_to_string(fRes->get_global_id() );
            }
        }
    }
    return "";
}

std::string GPE_DropDown_Resouce_Menu::get_plain_string()
{
    if( selectedResource!=NULL)
    {
        if( selectedResource->get_resource_type()== gpe::resource_type_function)
        {
            return ""+selectedResource->get_name()+"()";
        }
        else
        {
            return selectedResource->get_name();
        }
    }
    return "''";
}

int GPE_DropDown_Resouce_Menu::get_selected_id()
{
    return selectedId;
}

bool GPE_DropDown_Resouce_Menu::just_activated()
{
    return justActivated;
}

void GPE_DropDown_Resouce_Menu::process_self( gpe::shape_rect * viewedSpace, gpe::shape_rect * cam)
{
    viewedSpace = gpe::camera_find(viewedSpace);
    cam = gpe::camera_find(cam);
    isHovered = false;
    justActivated = false;
    GPE_GeneralGuiElement::process_self(viewedSpace,cam);
    if( isHovered || isInUse)
    {
        main_OVERLAY->update_tooltip(dropdownName);
    }
    if( isInUse &&( gpe::input->check_kb_down( kb_enter ) || gpe::input->check_kb_down( kb_space )  ) )
    {
        isClicked = true;
    }

    if( isClicked && cam!=NULL && viewedSpace!=NULL && containerTree!=NULL )
    {
        if( RESOURCE_TO_DRAG!=NULL)
        {
            if( RESOURCE_TO_DRAG->is_folder()==false && RESOURCE_TO_DRAG->projectParentFileName.compare(project_current_name)==0 )
            {
                set_selection(RESOURCE_TO_DRAG->get_global_id() );
                isOpen = false;
                isClicked = false;
                justActivated = true;
            }
            RESOURCE_TO_DRAG = NULL;
        }
        else if( main_CONTEXT_MENU!=NULL)
        {
            if( main_CONTEXT_MENU->subMenuIsOpen == false)
            {
                isOpen = true;
                GPE_open_context_menu(viewedSpace->x+widget_box.x-cam->x, viewedSpace->y+widget_box.y+widget_box.h-cam->y);
                int estimatedMenuSize = containerTree->get_options_width()+GENERAL_ICON_WIDTH_AND_PADDING ;
                int dropdownNameWidth = 0;
                int dropdownNameHeight = 0;
                if( (int)opName.size()>0 && FONT_TOOLBAR!=NULL)
                {
                    FONT_TOOLBAR->get_metrics(dropdownName,&dropdownNameWidth, &dropdownNameHeight);
                }
                dropdownNameWidth+=GENERAL_ICON_WIDTH_AND_PADDING+GENERAL_GPE_GUI_PADDING;
                estimatedMenuSize = std::max( estimatedMenuSize, dropdownNameWidth );
                main_CONTEXT_MENU->set_width( estimatedMenuSize );
                main_CONTEXT_MENU->add_menu_option(dropdownName,-1,NULL,-1,NULL,false,true);
                GPE_GeneralResourceContainer * cContainer = NULL;
                GPE_PopUpMenu_Option * cOption = NULL;
                if(containerTree!=NULL)
                {
                    for( int i = 0; i < containerTree->get_size(); i++)
                    {
                        cContainer = containerTree->get_resource_at(i);
                        if( cContainer!=NULL)
                        {
                            cOption = main_CONTEXT_MENU->add_menu_option(cContainer->get_name(),cContainer->get_global_id(),cContainer->get_resource_texture(),cContainer->get_resource_image_frame(),cContainer->get_resource_animation(),false,true);
                            cOption->isResourceOption = true;
                            if( cContainer->is_folder() )
                            {
                                add_folder_contents(cContainer,cOption);
                                cOption->isFolderOption = true;
                            }
                        }
                    }
                }
                isOpen = false;
                isClicked = false;
                justActivated = true;
                int foundResult = GPE_Get_Context_Result();
                if( foundResult>=0)
                {
                    selectedId = foundResult;
                    selectedResource = containerTree->find_resource_from_id(selectedId);
                    displayedResult = GPE_Action_Message;
                }
                else
                {
                    selectedId=-1;
                    displayedResult=dropdownName;
                }
                GPE_close_context_menu();
            }
        }
    }
}

void GPE_DropDown_Resouce_Menu::render_self( gpe::shape_rect * viewedSpace, gpe::shape_rect * cam )
{
    viewedSpace = gpe::camera_find(viewedSpace);
    cam = gpe::camera_find(cam);
    if( cam!=NULL && viewedSpace!=NULL)
    {
        gpe::gcanvas->render_rectangle( widget_box.x-cam->x,widget_box.y-cam->y,widget_box.x+widget_box.w-cam->x,widget_box.y+widget_box.h-cam->y,theme_main->input_color,false);
        if( selectedId >= 0)
        {
            selectedResource = containerTree->find_resource_from_id(selectedId);
            if( selectedResource!=NULL)
            {
                gpe::gfs->render_text_resized( widget_box.x+32+GENERAL_GPE_GUI_PADDING-cam->x,widget_box.y+widget_box.h/2-cam->y,selectedResource->get_name(),theme_main->input_font_color,FONT_POPUP,gpe::fa_left,gpe::fa_middle,widget_box.w-widget_box.h-12,-1);
                selectedResource->render_image( widget_box.x,widget_box.y,widget_box.h-2,widget_box.h-2,viewedSpace,cam);
            }
            else
            {
                gpe::gfs->render_text_resized( widget_box.x-cam->x,widget_box.y+widget_box.h/2-cam->y,opName,theme_main->input_font_color,FONT_POPUP,gpe::fa_left,gpe::fa_middle,widget_box.w-widget_box.h-12,-1);
            }
        }
        else
        {
            gpe::gfs->render_text_resized( widget_box.x+GENERAL_GPE_GUI_PADDING-cam->x,widget_box.y+widget_box.h/2-cam->y,opName,theme_main->input_font_color,FONT_POPUP,gpe::fa_left,gpe::fa_middle,widget_box.w-widget_box.h-12,-1);
        }
        //gpe::gcanvas->render_rectangle( widget_box.x+widget_box.w-widget_box.h*3/4-cam->x,widget_box.y-cam->y,widget_box.x+widget_box.w-cam->x,widget_box.y+widget_box.h-cam->y,theme_main->input_outline_color ,false);
        if( GPE_DROPDOWN_ARROW!=NULL )
        {
            GPE_DROPDOWN_ARROW->render_tex_resized(  widget_box.x+widget_box.w-widget_box.h-cam->x,widget_box.y - cam->y,widget_box.h,widget_box.h, NULL, theme_main->input_font_color );
        }
        if( isInUse)
        {
            gpe::gcanvas->render_rectangle( widget_box.x-cam->x,widget_box.y-cam->y,widget_box.x+widget_box.w-cam->x,widget_box.y+widget_box.h-cam->y,theme_main->input_highlight_outline_color,true);
        }
        else if( isHovered)
        {
            gpe::gcanvas->render_rectangle( widget_box.x-cam->x,widget_box.y-cam->y,widget_box.x+widget_box.w-cam->x,widget_box.y+widget_box.h-cam->y,theme_main->input_highlight_alt_color,true);
        }
        else
        {
            gpe::gcanvas->render_rectangle( widget_box.x-cam->x,widget_box.y-cam->y,widget_box.x+widget_box.w-cam->x,widget_box.y+widget_box.h-cam->y,theme_main->input_outline_color,true);
        }
    }
}

void GPE_DropDown_Resouce_Menu::set_name(std::string new_name)
{
    opName = new_name;
}

void GPE_DropDown_Resouce_Menu::set_selection(int new_id)
{
    if( new_id>0 && containerTree!=NULL)
    {
        GPE_GeneralResourceContainer * tRes = containerTree->find_resource_from_id(new_id,true);
        if( tRes!=NULL)
        {
            selectedId = new_id;
            displayedResult = tRes->get_name();
            selectedResource = tRes;
        }
    }
    else
    {
        selectedId = -1;
        displayedResult = opName = dropdownName;
        selectedResource = NULL;
    }
}

void GPE_DropDown_Resouce_Menu::set_selected_target(std::string new_name)
{
    int new_id = stg_ex::string_to_int(new_name, -1);
    if( new_id > 0 && containerTree!=NULL )
    {
        GPE_GeneralResourceContainer * tRes = containerTree->find_resource_from_id(new_id,true);
        if( tRes!=NULL)
        {
            selectedId = new_id;
            displayedResult = tRes->get_name();
            selectedResource = tRes;
        }
    }
    else if(  containerTree!=NULL )
    {
        GPE_GeneralResourceContainer * tRes = containerTree->find_resource_from_name(new_name,true);
        if( tRes!=NULL)
        {
            selectedId = tRes->get_global_id();
            displayedResult = tRes->get_name();
            selectedResource = tRes;
        }
        else
        {
            selectedId = -1;
            displayedResult = opName = dropdownName;
            selectedResource = NULL;
        }
    }
    else
    {
        selectedId = -1;
        displayedResult = opName = dropdownName;
        selectedResource = NULL;
    }
}


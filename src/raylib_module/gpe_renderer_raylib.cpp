/*
gpe_renderer_raylib.cpp
This file is part of:
GAME PENCIL ENGINE
https://www.pawbyte.com/gamepencilengine
Copyright (c) 2014-2021 Nathan Hurde, Chase Lee.

Copyright (c) 2014-2021 PawByte LLC.
Copyright (c) 2014-2021 Game Pencil Engine contributors ( Contributors Page )

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

#include "gpe_renderer_raylib.h"
#include "../gpe/gpe_constants.h"
#include "../gpe/gpe_globals.h"
#include "../gpe/gpe_settings.h"
#include "../other_libs/stg_ex.h"

namespace gpe
{
    renderer_system_raylib * renderer_main_raylib = NULL;

    renderer_system_raylib::renderer_system_raylib( int rId, int wWidth, int wHeight )
    {
        r_type = "raylib";
        r_name = "raylib_renderer";

        render_blend_mode = blend_mode_blend;
        last_rendered_width = 0;
        last_rendered_height = 0;
        rendered_once = false;

        cleared_this_frame = false;
        r_width = wWidth;
        r_height = wHeight;
        last_screenshot_id = 0;

        ClearBackground( BLACK );
        EndBlendMode();
        BeginBlendMode( BLEND_ALPHA );
    }

    renderer_system_raylib::~renderer_system_raylib()
    {

    }

    bool renderer_system_raylib::disable_scaling()
    {
        renderer_scaling = false;
        return false;
    }

    bool renderer_system_raylib::enable_scaling()
    {
        return true;
    }

    int renderer_system_raylib::get_blend_mode()
    {
        return render_blend_mode;
    }



    void renderer_system_raylib::reset_input()
    {
        cleared_this_frame = false;
    }

    void renderer_system_raylib::reset_viewpoint()
    {
        EndScissorMode();
        render_sub_rectangle->x = 0;
        render_sub_rectangle->y = 0;
        render_sub_rectangle->w = r_width;
        render_sub_rectangle->h = r_height;

    }

    void renderer_system_raylib::resize_renderer(int newW, int newH )
    {
        if( newW == r_width || newH == r_height)
        {
            return;
        }
    }

    void renderer_system_raylib::set_viewpoint( shape_rect * newViewPoint)
    {
        EndScissorMode();
        if( newViewPoint!=NULL)
        {
            render_sub_rectangle->x = newViewPoint->x;
            render_sub_rectangle->y = newViewPoint->y;
            render_sub_rectangle->w = newViewPoint->w;
            render_sub_rectangle->h = newViewPoint->h;
            BeginScissorMode( render_sub_rectangle->x, render_sub_rectangle->y, render_sub_rectangle->w, render_sub_rectangle->h );
        }
    }

    std::string renderer_system_raylib::get_renderer_name()
    {
        return "raylib";
    }

    std::string renderer_system_raylib::get_renderer_type()
    {
        return r_type;
    }


    void renderer_system_raylib::clear_renderer( bool windowIsMinimized )
    {
        if( windowIsMinimized )
        {
            return;
        }
        BeginDrawing();
        ClearBackground( BLACK );
        BeginBlendMode( BLEND_ALPHA );
        return;

        uint32_t sTicks = time_keeper->get_ticks();
        set_render_blend_mode( blend_mode_blend );

        uint32_t eTicks = time_keeper->get_ticks();
        error_log->log_ms_action("renderer_system_raylib::clear_renderer()",eTicks - sTicks,10 );
    }

    bool renderer_system_raylib::render_circle_color( int16_t x, int16_t y, int16_t rad, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if( rad < 1)
        {
            return false;
        }
        render_current_color.r = r;
        render_current_color.g = g;
        render_current_color.b = b;
        render_current_color.a = a;
        DrawCircle( x, y, rad, render_current_color);
        return true;
    }

    void renderer_system_raylib::render_horizontal_line(int y, int x1, int x2)
    {
        DrawLine( x1, y, x2, y, render_current_color );
    }

    void renderer_system_raylib::render_horizontal_line_color( int y, int x1, int x2,  uint8_t r, uint8_t g, uint8_t b, uint8_t a )
    {
        render_current_color.r = r;
        render_current_color.g = g;
        render_current_color.b = b;
        render_current_color.a = a;
        DrawLine( x1, y, x2, y, render_current_color );

    }

    std::string renderer_system_raylib::save_screenshot(std::string file_location)
    {
        TakeScreenshot( file_location.c_str() );
        return file_location;
    }

    bool renderer_system_raylib::scale_renderer( int s_width, int s_height, bool scale_int )
    {
        return false;
    }

    bool renderer_system_raylib::scale_renderer_factor( float s_width, float s_height, bool scale_int )
    {
        return false;
    }

    bool renderer_system_raylib::screen_was_cleared()
    {
        return cleared_this_frame;
    }

    void renderer_system_raylib::set_render_blend_mode( int blend_mode_new )
    {
        if( render_blend_mode!=blend_mode_new)
        {
            render_blend_mode = blend_mode_new;
            EndBlendMode();
            switch( blend_mode_new)
            {
                case blend_mode_add:
                    BeginBlendMode( BLEND_ADDITIVE );
                break;

                case blend_mode_mod:
                    BeginBlendMode( BLEND_ADD_COLORS );
                break;

                case blend_mode_mul:
                    BeginBlendMode( BLEND_MULTIPLIED );
                break;

                case blend_mode_none:
                    BeginBlendMode( BLEND_ADD_COLORS );
                break;

                default:
                    BeginBlendMode( BLEND_ALPHA );
                    render_blend_mode = blend_mode_blend;
                break;
            }
        }
    }

    void renderer_system_raylib::update_renderer( bool windowIsMinimized )
    {
        uint32_t sTicks = time_keeper->get_ticks();
        last_rendered_width = r_width;
        last_rendered_height = r_height;
        if( windowIsMinimized == false )
        {
            set_render_blend_mode( blend_mode_blend );

            EndDrawing();

            cleared_this_frame = false;
            rendered_once = true;
        }
        uint32_t eTicks =  time_keeper->get_ticks();
        error_log->log_ms_action("renderer_system_raylib::update_renderer()",eTicks - sTicks,10 );
    }
}
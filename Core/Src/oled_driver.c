/*
 * oled_driver.c
 *
 *  Created on: Jul 8, 2025
 *      Author: UnikoZera
 */

#include "oled_driver.h"

OLED_MenuState CurrentMenu = MAIN_MENU;
uint8_t SelectCursor = 1;
uint8_t PageRank = 1;
static bool is_first_run = true;
bool MutiKeyUp, MutiKeyDown, MutiKeyLeft, MutiKeyRight, MutiKeyEnter; // using api
bool keyUP, keyDOWN, keyLEFT, keyRIGHT, keyENTER; // using api
static uint8_t CurrentKey = 1;

uint16_t cps = 0;
uint64_t msg_counter = 0; // 用于动画计时

// UI Settings
#define OLED_UI_START_X -105
#define OLED_UI_END_X 2
#define OLED_UI_START_Y -9
#define OLED_UI_GAP_Y 11
#define OLED_UI_TWEEN_DURATION 300
#define OLED_UI_TWEEN_IN EASE_INOUT_CUBIC
#define OLED_UI_TWEEN_OUT EASE_INOUT_CUBIC

// Icon Settings
#define OLED_ICON_Start_X -5
#define OLED_ICON_End_X 30
#define OLED_ICON_GAP_X 50
#define OLED_ICON_Start_Y -43
#define OLED_ICON_End_Y 6
#define OLED_ICON_TWEEN_DURATION 300
#define OLED_ICON_TWEEN_IN EASE_INOUT_CIRC
#define OLED_ICON_TWEEN_OUT EASE_INOUT_CUBIC

// Title Animation Settings
#define OLED_TITLE_Start_Y 70
#define OLED_TITLE_End_Y 54
#define OLED_TITLE_TWEEN_DURATION 300
#define OLED_TITLE_TWEEN_IN EASE_INOUT_CUBIC
#define OLED_TITLE_TWEEN_OUT EASE_INOUT_CUBIC

// Cursor Animation Settings
#define OLED_CURSOR_TWEEN_DURATION 400
#define OLED_CURSOR_TWEEN_POSITION EASE_INOUT_CUBIC
#define OLED_CURSOR_TWEEN_SCALE EASE_INOUT_CUBIC

void OLED_UI_Init(void)
{
    OLED_InitAnimationManager(&g_Icon_AnimationManager);
    OLED_InitAnimationManager(&g_Title_AnimationManager);
    OLED_InitAnimationManager(&g_AnimationManager);

    keyDOWN = keyUP = keyLEFT = keyRIGHT = keyENTER = false;
    MutiKeyUp = MutiKeyDown = MutiKeyLeft = MutiKeyRight = MutiKeyEnter = false;
}

void Move_Menu_Elements(bool enable)
{
    if (enable)
    {
        if (CurrentMenu == RGB_MENU)
        {
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconMenu", OLED_ICON_Start_X + OLED_ICON_GAP_X * 0, OLED_ICON_Start_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_OUT);
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconRGB", OLED_ICON_Start_X + OLED_ICON_GAP_X * 1, OLED_ICON_Start_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_OUT);
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconSettings", OLED_ICON_Start_X + OLED_ICON_GAP_X * 2, OLED_ICON_Start_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_OUT);

            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleMenu", (OLED_WIDTH - strlen("Main Menu") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_IN);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleRGB", (OLED_WIDTH - strlen("RGB Settings") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("Keyboard Settings") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_OUT);
        }
        else if (CurrentMenu == MAIN_MENU)
        {
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconMenu", OLED_ICON_Start_X + OLED_ICON_GAP_X * 1, OLED_ICON_Start_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_OUT);
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconRGB", OLED_ICON_Start_X + OLED_ICON_GAP_X * 2, OLED_ICON_Start_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_OUT);
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconSettings", OLED_ICON_Start_X + OLED_ICON_GAP_X * 3, OLED_ICON_Start_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_OUT);

            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleMenu", (OLED_WIDTH - strlen("Main Menu") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_IN);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleRGB", (OLED_WIDTH - strlen("RGB Settings") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("Keyboard Settings") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_OUT);
        }
        else if (CurrentMenu == SETTINGS_MENU)
        {
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconMenu", OLED_ICON_Start_X + OLED_ICON_GAP_X * -1, OLED_ICON_Start_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_OUT);
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconRGB", OLED_ICON_Start_X + OLED_ICON_GAP_X * 0, OLED_ICON_Start_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_OUT);
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconSettings", OLED_ICON_Start_X + OLED_ICON_GAP_X * 1, OLED_ICON_Start_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_OUT);

            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleMenu", (OLED_WIDTH - strlen("Main Menu") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_IN);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleRGB", (OLED_WIDTH - strlen("RGB Settings") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("Keyboard Settings") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_OUT);
        }
    }
    else
    {
        if (CurrentMenu == RGB_MENU)
        {
            OLED_DoTweenObject(&g_Title_AnimationManager, "RGBStyle", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "RGBSpeed", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "RGBBrightness", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_OUT);
            OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_OUT);
        }
        else if (CurrentMenu == SETTINGS_MENU)
        {
            OLED_DoTweenObject(&g_Title_AnimationManager, "ShowCPS", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "FastMode", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TriggerThre", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "CurrentKey", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "PressSpeed", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 5, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "ReleseSpeed", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 6, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_OUT);
            OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 7, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_OUT);
        }
    }
}

void Move_Sub_Title(bool enable)
{
    if (enable)
    {
        OLED_DoTweenObject(&g_AnimationManager, "SquarePos", 48, 14, 500, OLED_UI_TWEEN_IN);
        OLED_DoTweenObject(&g_AnimationManager, "SquareSca", 76, 42, 500, OLED_UI_TWEEN_IN);
    }
    else
    {
        OLED_DoTweenObject(&g_AnimationManager, "SquarePos", OLED_WIDTH + 5, 14, 500, OLED_UI_TWEEN_OUT);
        OLED_DoTweenObject(&g_AnimationManager, "SquareSca", 0, 0, 500, OLED_UI_TWEEN_OUT);
    }
}

void Get_OLED_Input(void)
{
    keyboard_state.TouchButton_2 ? (keyENTER = true) : (keyENTER = false);

    if (keyboard_settings.keyboard_mode == 2)
    {
        if (keyboard_state.KEY_1_info.key_state == KEY_PRESSED)
        {
            keyLEFT = true;
        }
        else if (keyboard_state.KEY_1_info.key_state == KEY_RELEASED)
        {
            keyLEFT = false;
        }
        if (keyboard_state.KEY_2_info.key_state == KEY_PRESSED)
        {
            keyUP = true;
        }
        else if (keyboard_state.KEY_2_info.key_state == KEY_RELEASED)
        {
            keyUP = false;
        }
        if (keyboard_state.KEY_3_info.key_state == KEY_PRESSED)
        {
            keyDOWN = true;
        }
        else if (keyboard_state.KEY_3_info.key_state == KEY_RELEASED)
        {
            keyDOWN = false;
        }
        if (keyboard_state.KEY_4_info.key_state == KEY_PRESSED)
        {
            keyRIGHT = true;
        }
        else if (keyboard_state.KEY_4_info.key_state == KEY_RELEASED)
        {
            keyRIGHT = false;
        }
    }
}

void OLED_Drawer(void)
{
    float x, y, x1, y1;
    char buffer[20];
    OLED_GetObjectPosition(&g_Title_AnimationManager, "TitleMenu", &x, &y);
    OLED_DisplayString(x, y, "Main Menu");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "TitleRGB", &x, &y);
    OLED_DisplayString(x, y, "RGB Settings");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "TitleSettings", &x, &y);
    OLED_DisplayString(x, y, "Keyboard Settings");

    OLED_GetObjectPosition(&g_Icon_AnimationManager, "IconMenu", &x, &y);
    OLED_DrawIcon(x, y, ICON_MENU);
    OLED_GetObjectPosition(&g_Icon_AnimationManager, "IconRGB", &x, &y);
    OLED_DrawIcon(x, y, ICON_RGB);
    OLED_GetObjectPosition(&g_Icon_AnimationManager, "IconSettings", &x, &y);
    OLED_DrawIcon(x, y, ICON_SETTINGS);

    OLED_GetObjectPosition(&g_Title_AnimationManager, "RGBStyle", &x, &y);
    sprintf(buffer, "RGB Style:%d", keyboard_settings.rgb_style);
    OLED_DisplayString(x, y, buffer);
    OLED_GetObjectPosition(&g_Title_AnimationManager, "RGBSpeed", &x, &y);
    sprintf(buffer, "RGB Speed:%d", keyboard_settings.rgb_speed);
    OLED_DisplayString(x, y, buffer);
    OLED_GetObjectPosition(&g_Title_AnimationManager, "RGBBrightness", &x, &y);
    sprintf(buffer, "RGB Brightness:%d", keyboard_settings.rgb_brightness);
    OLED_DisplayString(x, y, buffer);
    OLED_GetObjectPosition(&g_AnimationManager, "BackButton", &x, &y);
    OLED_DisplayString(x, y, "Back");

    OLED_GetObjectPosition(&g_Title_AnimationManager, "ShowCPS", &x, &y);
    sprintf(buffer, "Show CPS:%s", keyboard_settings.showCPS ? "ON" : "OFF");
    OLED_DisplayString(x, y, buffer);
    OLED_GetObjectPosition(&g_Title_AnimationManager, "FastMode", &x, &y);
    sprintf(buffer, "Fast Mode:%s", keyboard_settings.enable_quick_trigger ? "ON" : "OFF");
    OLED_DisplayString(x, y, buffer);
    OLED_GetObjectPosition(&g_Title_AnimationManager, "TriggerThre", &x, &y);
    OLED_DisplayString(x, y, "Trigger Threshold");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "CurrentKey", &x, &y);

    sprintf(buffer, "Current Key:%d", CurrentKey);
    OLED_DisplayString(x, y, buffer);
    OLED_GetObjectPosition(&g_Title_AnimationManager, "PressSpeed", &x, &y);
    OLED_DisplayString(x, y, "Press Speed");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "ReleseSpeed", &x, &y);
    OLED_DisplayString(x, y, "Release Speed");

    OLED_GetObjectPosition(&g_AnimationManager, "SquarePos", &x, &y);
    OLED_GetObjectPosition(&g_AnimationManager, "SquareSca", &x1, &y1);
    OLED_DrawFilledRectangle(x, y, x1, y1, 0);
    OLED_DrawRectangle(x, y, x1, y1);

    uint16_t keys_threshold_oled[12] = 
    {
        keyboard_settings._1trigger_position_threshold,
        keyboard_settings._2trigger_position_threshold,
        keyboard_settings._3trigger_position_threshold,
        keyboard_settings._4trigger_position_threshold,

        keyboard_settings._1trigger_speed_threshold,
        keyboard_settings._2trigger_speed_threshold,
        keyboard_settings._3trigger_speed_threshold,
        keyboard_settings._4trigger_speed_threshold,

        keyboard_settings._1release_speed_threshold,
        keyboard_settings._2release_speed_threshold,
        keyboard_settings._3release_speed_threshold,
        keyboard_settings._4release_speed_threshold
    };

    uint16_t keys_info_speed_oled[4] = 
    {
        keyboard_state.KEY_1_info.press_speed,
        keyboard_state.KEY_2_info.press_speed,
        keyboard_state.KEY_3_info.press_speed,
        keyboard_state.KEY_4_info.press_speed
    };


    if (CurrentMenu == SETTINGS_MENU && PageRank > 2 && SelectCursor == 4)
    {
        OLED_DisplayString(x+2, y+2, "CurrADC:"); // 最多放下12个字符
        OLED_DisplayInteger(x+2, y+12, filter_adc_data[CurrentKey - 1]);
        OLED_DisplayString(x+2, y+22, "TrigThre:");
        OLED_DisplayInteger(x+2, y+32, keys_threshold_oled[CurrentKey - 1]);
    }
    else if (CurrentMenu == SETTINGS_MENU && PageRank > 2 && SelectCursor == 5)
    {
        OLED_DisplayString(x+2, y+2, "CurrSpeed:");
        OLED_DisplayFloat(x+2, y+12, keys_info_speed_oled[CurrentKey - 1]);
        OLED_DisplayString(x+2, y+22, "PresSpeed:");
        OLED_DisplayFloat(x+2, y+32, keys_threshold_oled[CurrentKey + 3]);
    }
    else if (CurrentMenu == SETTINGS_MENU && PageRank > 2 && SelectCursor == 6)
    {
        OLED_DisplayString(x+2, y+2, "CurrSpeed:");
        OLED_DisplayFloat(x+2, y+12, keys_info_speed_oled[CurrentKey - 1]);
        OLED_DisplayString(x+2, y+22, "ReleSpeed:");
        OLED_DisplayFloat(x+2, y+32, keys_threshold_oled[CurrentKey + 7]);
    }
    else if (CurrentMenu == RGB_MENU && PageRank > 2 && SelectCursor == 2)
    {
        OLED_DisplayString(x+2, y+12, "RGBSpeed:");
        OLED_DisplayInteger(x+2, y+24, keyboard_settings.rgb_speed);
    }
    else if (CurrentMenu == RGB_MENU && PageRank > 2 && SelectCursor == 3)
    {
        OLED_DisplayString(x+2, y+12, "RGBBright:");
        OLED_DisplayInteger(x+2, y+24, keyboard_settings.rgb_brightness);
    }

    OLED_GetObjectPosition(&g_AnimationManager, "CursorPos", &x, &y);
    OLED_GetObjectPosition(&g_AnimationManager, "CursorSca", &x1, &y1);
    OLED_InvertArea(x - 2, y - 2, x1, y1);
}

void OLED_StateMachine(void)
{
    switch (CurrentMenu)
    {
    case MAIN_MENU:
        if (PageRank == 1)
        {
            OLED_DoTweenObject(&g_AnimationManager, "CursorPos", (OLED_WIDTH - strlen("Main Menu") * 6)/2 - 5, OLED_TITLE_End_Y - 3, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_POSITION);
            OLED_DoTweenObject(&g_AnimationManager, "CursorSca", strlen("Main Menu") * 6 + 14, 2, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);

            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconMenu", OLED_ICON_Start_X + OLED_ICON_GAP_X * 1, OLED_ICON_End_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconRGB", OLED_ICON_Start_X + OLED_ICON_GAP_X * 2, OLED_ICON_End_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconSettings", OLED_ICON_Start_X + OLED_ICON_GAP_X * 3, OLED_ICON_End_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);

            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleMenu", (OLED_WIDTH - strlen("Main Menu") * 6)/2, OLED_TITLE_End_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_IN);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleRGB", (OLED_WIDTH - strlen("RGB Settings") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("Keyboard Settings") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_OUT);
        }
        else if (PageRank == 2)
        {
            OLED_DoTweenObject(&g_AnimationManager, "CursorPos", OLED_WIDTH/2, OLED_TITLE_End_Y - 3, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_POSITION);
            OLED_DoTweenObject(&g_AnimationManager, "CursorSca", 0, 2, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);
            SelectCursor = 1;
        }
        break;
    case RGB_MENU:
        if (PageRank == 1)
        {
            OLED_DoTweenObject(&g_AnimationManager, "CursorPos", (OLED_WIDTH - strlen("RGB Settings") * 6)/2 - 5, OLED_TITLE_End_Y - 3, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_POSITION);
            OLED_DoTweenObject(&g_AnimationManager, "CursorSca", strlen("RGB Settings") * 6 + 14, 2, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);

            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconRGB", OLED_ICON_Start_X + OLED_ICON_GAP_X * 1, OLED_ICON_End_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconSettings", OLED_ICON_Start_X + OLED_ICON_GAP_X * 2, OLED_ICON_End_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconMenu", OLED_ICON_Start_X + OLED_ICON_GAP_X * 0, OLED_ICON_End_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);

            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleMenu", (OLED_WIDTH - strlen("Main Menu") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleRGB", (OLED_WIDTH - strlen("RGB Settings") * 6)/2, OLED_TITLE_End_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_IN);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("Keyboard Settings") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_OUT);
        }
        else if (PageRank == 2)
        {
            if (SelectCursor < 1)
            {
                SelectCursor = 4;
            }
            else if (SelectCursor > 4)
            {
                SelectCursor = 1;
            }

            if (SelectCursor == 1)
            {
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", (strlen("RGB Style: ")*6+6), 10, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);
            }
            else if (SelectCursor == 2)
            {
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", (strlen("RGB Speed")*6+6), 10, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);
            }
            else if (SelectCursor == 3)
            {
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", (strlen("RGB Brightness")*6+6), 10, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);
            }
            else if (SelectCursor == 4)
            {
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", (strlen("Back")*6+6), 10, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);
            }
            
            OLED_DoTweenObject(&g_AnimationManager, "CursorPos", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y + 1, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_POSITION);

            OLED_DoTweenObject(&g_Title_AnimationManager, "RGBStyle", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - SelectCursor) , OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
            OLED_DoTweenObject(&g_Title_AnimationManager, "RGBSpeed", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - SelectCursor), OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
            OLED_DoTweenObject(&g_Title_AnimationManager, "RGBBrightness", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - SelectCursor), OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
            OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (5 - SelectCursor), OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
        }
        break;
    case SETTINGS_MENU:
        if (PageRank == 1)
        {
            OLED_DoTweenObject(&g_AnimationManager, "CursorPos", (OLED_WIDTH - strlen("Keyboard Settings") * 6)/2 - 5, OLED_TITLE_End_Y - 3, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_POSITION);
            OLED_DoTweenObject(&g_AnimationManager, "CursorSca", strlen("Keyboard Settings") * 6 + 14, 2, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);

            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconSettings", OLED_ICON_Start_X + OLED_ICON_GAP_X * 1, OLED_ICON_End_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconRGB", OLED_ICON_Start_X + OLED_ICON_GAP_X * 0, OLED_ICON_End_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);
            OLED_DoTweenObject(&g_Icon_AnimationManager, "IconMenu", OLED_ICON_Start_X + OLED_ICON_GAP_X * -1, OLED_ICON_End_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);

            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleMenu", (OLED_WIDTH - strlen("Main Menu") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleRGB", (OLED_WIDTH - strlen("RGB Settings") * 6)/2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_OUT);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("Keyboard Settings") * 6)/2, OLED_TITLE_End_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_OUT);
        }
        else if (PageRank == 2)
        {
            if (SelectCursor < 1)
            {
                SelectCursor = 7;
            }
            else if (SelectCursor > 7)
            {
                SelectCursor = 1;
            }

            if (SelectCursor == 1)
            {
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", (strlen("Show CPS:OFF")*6+6), 10, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);
            }
            else if (SelectCursor == 2)
            {
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", (strlen("Fast Mode:OFF")*6+6), 10, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);
            }
            else if (SelectCursor == 3)
            {
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", (strlen("Current Key: ")*6+6), 10, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);
            }
            else if (SelectCursor == 4)
            {
                
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", (strlen("Trigger Threshold")*6+6), 10, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);
            }
            else if (SelectCursor == 5)
            {
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", (strlen("Press Speed")*6+6), 10, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);
            }
            else if (SelectCursor == 6)
            {
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", (strlen("Release Speed")*6+6), 10, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);
            }
            else if (SelectCursor == 7)
            {
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", (strlen("Back")*6+6), 10, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_SCALE);
            }

            OLED_DoTweenObject(&g_AnimationManager, "CursorPos", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y + 1, OLED_CURSOR_TWEEN_DURATION, OLED_CURSOR_TWEEN_POSITION);

            OLED_DoTweenObject(&g_Title_AnimationManager, "ShowCPS", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - SelectCursor), OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
            OLED_DoTweenObject(&g_Title_AnimationManager, "FastMode", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - SelectCursor), OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
            OLED_DoTweenObject(&g_Title_AnimationManager, "CurrentKey", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - SelectCursor), OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TriggerThre", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (5 - SelectCursor), OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
            OLED_DoTweenObject(&g_Title_AnimationManager, "PressSpeed", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (6 - SelectCursor), OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
            OLED_DoTweenObject(&g_Title_AnimationManager, "ReleseSpeed", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (7 - SelectCursor), OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
            OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (8 - SelectCursor), OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
        }
        break;
    default:
        break;
    }

#pragma region KEY Handle
    if (keyRIGHT && !MutiKeyRight)
    {
        MutiKeyRight = true;
        if (PageRank == 1)
        {
            if (CurrentMenu == MAIN_MENU)
            {
                CurrentMenu = RGB_MENU;
            }
            else if (CurrentMenu == RGB_MENU)
            {
                CurrentMenu = SETTINGS_MENU;
            }
            else if (CurrentMenu == SETTINGS_MENU)
            {
                CurrentMenu = MAIN_MENU;
            }
        }
    }

    if (keyLEFT && !MutiKeyLeft)
    {
        MutiKeyLeft = true;
        if (PageRank == 1)
        {
            if (CurrentMenu == MAIN_MENU)
            {
                CurrentMenu = SETTINGS_MENU;
            }
            else if (CurrentMenu == RGB_MENU)
            {
                CurrentMenu = MAIN_MENU;
            }
            else if (CurrentMenu == SETTINGS_MENU)
            {
                CurrentMenu = RGB_MENU;
            }
        }
    }

    if (keyUP && !MutiKeyUp)
    {
        MutiKeyUp = true;
        if (PageRank == 2)
        {
            SelectCursor--;
        }
    }

    if (keyDOWN && !MutiKeyDown)
    {
        MutiKeyDown = true;
        if (PageRank == 2)
        {
            SelectCursor++;
        }
    }

    if (keyENTER && !MutiKeyEnter)
    {
        MutiKeyEnter = true;
        if (CurrentMenu == MAIN_MENU && PageRank == 1)
        {
            PageRank = 2;
            keyboard_settings.keyboard_mode = 1;
            Move_Menu_Elements(true);
        }
        else if (CurrentMenu == MAIN_MENU && PageRank == 2)
        {
            PageRank = 1;
            keyboard_settings.keyboard_mode = 2; // 设置模式
            Move_Menu_Elements(false);
        }

        if (CurrentMenu == RGB_MENU && PageRank == 1)
        {
            PageRank = 2;
            Move_Menu_Elements(true);
        }
        else if (CurrentMenu == RGB_MENU && PageRank == 2 && SelectCursor == 4)
        {
            PageRank = 1;
            SelectCursor = 1;
            Move_Menu_Elements(false);
        }
        else if (CurrentMenu == RGB_MENU && SelectCursor == 2) // dangerous behavour
        {
            PageRank++;
            if (PageRank > 3)
            {
                PageRank = 2;
                Move_Sub_Title(false);
            }
            if (PageRank == 3)
            {
                Move_Sub_Title(true);
                OLED_DoTweenObject(&g_AnimationManager, "CursorPos", 51, 27, 500, OLED_UI_TWEEN_IN);
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", strlen("RGBSpeed:")*6+2, 10, 500, OLED_CURSOR_TWEEN_SCALE);
            }
        }
        else if (CurrentMenu == RGB_MENU && SelectCursor == 3) // dangerous behavour
        {
            PageRank++;
            if (PageRank > 3)
            {
                PageRank = 2;
                Move_Sub_Title(false);
            }
            if (PageRank == 3)
            {
                Move_Sub_Title(true);
                OLED_DoTweenObject(&g_AnimationManager, "CursorPos", 51, 27, 500, OLED_UI_TWEEN_IN);
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", strlen("RGBBright:")*6+2, 10, 500, OLED_CURSOR_TWEEN_SCALE);
            }
        }

        if (CurrentMenu == SETTINGS_MENU && PageRank == 1)
        {
            PageRank = 2;
            Move_Menu_Elements(true);
        }
        else if (CurrentMenu == SETTINGS_MENU && PageRank == 2 && SelectCursor == 7)
        {
            PageRank = 1;
            SelectCursor = 1;
            Move_Menu_Elements(false);
        }
        else if (CurrentMenu == SETTINGS_MENU && SelectCursor == 4) // dangerous behavour
        {
            PageRank++;
            if (PageRank > 4)
            {
                PageRank = 2;
                Move_Sub_Title(false);
            }
            if (PageRank == 3)
            {
                Move_Sub_Title(true);
                OLED_DoTweenObject(&g_AnimationManager, "CursorPos", 51, 17, 500, OLED_UI_TWEEN_IN);
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", strlen("TrigThre:")*6+2, 10, 500, OLED_CURSOR_TWEEN_SCALE);
            }
            else if (PageRank == 4)
            {
                Move_Sub_Title(true);
                OLED_DoTweenObject(&g_AnimationManager, "CursorPos", 51, 37, 500, OLED_UI_TWEEN_OUT);
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", strlen("CurrThre:")*6+2, 10, 500, OLED_CURSOR_TWEEN_SCALE);
            }
        }
        else if (CurrentMenu == SETTINGS_MENU && SelectCursor == 5) // dangerous behavour
        {
            PageRank++;
            if (PageRank > 4)
            {
                PageRank = 2;
                Move_Sub_Title(false);
            }
            if (PageRank == 3)
            {
                Move_Sub_Title(true);
                OLED_DoTweenObject(&g_AnimationManager, "CursorPos", 51, 17, 500, OLED_UI_TWEEN_IN);
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", strlen("PresSpeed:")*6+2, 10, 500, OLED_CURSOR_TWEEN_SCALE);
            }
            else if (PageRank == 4)
            {
                Move_Sub_Title(true);
                OLED_DoTweenObject(&g_AnimationManager, "CursorPos", 51, 37, 500, OLED_UI_TWEEN_OUT);
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", strlen("CurrSpeed:")*6+2, 10, 500, OLED_CURSOR_TWEEN_SCALE);
            }
        }
        else if (CurrentMenu == SETTINGS_MENU && SelectCursor == 6) // dangerous behavour
        {
            PageRank++;
            if (PageRank > 4)
            {
                PageRank = 2;
                Move_Sub_Title(false);
            }
            if (PageRank == 3)
            {
                Move_Sub_Title(true);
                OLED_DoTweenObject(&g_AnimationManager, "CursorPos", 51, 17, 500, OLED_UI_TWEEN_IN);
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", strlen("ReleSpeed:")*6+2, 10, 500, OLED_CURSOR_TWEEN_SCALE);
            }
            else if (PageRank == 4)
            {
                Move_Sub_Title(true);
                OLED_DoTweenObject(&g_AnimationManager, "CursorPos", 51, 37, 500, OLED_UI_TWEEN_OUT);
                OLED_DoTweenObject(&g_AnimationManager, "CursorSca", strlen("CurrSpeed:")*6+2, 10, 500, OLED_CURSOR_TWEEN_SCALE);
            }
        }
    }

    if (!keyENTER)
    {
        MutiKeyEnter = false;
    }
    if (!keyRIGHT)
    {
        MutiKeyRight = false;
    }
    if (!keyLEFT)
    {
        MutiKeyLeft = false;
    }
    if (!keyUP)
    {
        MutiKeyUp = false;
    }
    if (!keyDOWN)
    {
        MutiKeyDown = false;
    }
#pragma endregion
}

void OLED_UI_Loop(void)
{
    if (is_first_run)
    {
        CurrentMenu = MAIN_MENU;
        OLED_MoveObject(&g_Title_AnimationManager, "TitleMenu", (OLED_WIDTH - strlen("Main Menu") * 6)/2, OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("Main Menu") * 6)/2, OLED_TITLE_End_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_IN);
        OLED_MoveObject(&g_Title_AnimationManager, "TitleRGB", OLED_WIDTH / 2, OLED_TITLE_Start_Y, OLED_WIDTH / 2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_IN);
        OLED_MoveObject(&g_Title_AnimationManager, "TitleSettings", OLED_WIDTH / 2, OLED_TITLE_Start_Y, OLED_WIDTH / 2, OLED_TITLE_Start_Y, OLED_TITLE_TWEEN_DURATION, OLED_TITLE_TWEEN_IN);

        OLED_MoveObject(&g_Icon_AnimationManager, "IconMenu", OLED_ICON_Start_X + OLED_ICON_GAP_X * 1, OLED_ICON_Start_Y, OLED_ICON_Start_X + OLED_ICON_GAP_X * 1, OLED_ICON_End_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);
        OLED_MoveObject(&g_Icon_AnimationManager, "IconRGB", OLED_ICON_Start_X + OLED_ICON_GAP_X * 2, OLED_ICON_Start_Y, OLED_ICON_Start_X + OLED_ICON_GAP_X * 2, OLED_ICON_End_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);
        OLED_MoveObject(&g_Icon_AnimationManager, "IconSettings", OLED_ICON_Start_X + OLED_ICON_GAP_X * 3, OLED_ICON_Start_Y, OLED_ICON_Start_X + OLED_ICON_GAP_X * 3, OLED_ICON_End_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);

        OLED_MoveObject(&g_AnimationManager, "CursorPos", OLED_WIDTH/2, OLED_TITLE_End_Y, OLED_WIDTH/2, OLED_TITLE_End_Y, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);
        OLED_MoveObject(&g_AnimationManager, "CursorSca", 0, 2, 0, 2, OLED_ICON_TWEEN_DURATION, OLED_ICON_TWEEN_IN);
        OLED_MoveObject(&g_AnimationManager, "BackButton", OLED_UI_START_X, OLED_UI_START_Y, OLED_UI_START_X, OLED_UI_START_Y, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);


        OLED_MoveObject(&g_Title_AnimationManager, "RGBStyle", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
        OLED_MoveObject(&g_Title_AnimationManager, "RGBSpeed", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
        OLED_MoveObject(&g_Title_AnimationManager, "RGBBrightness", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);

        OLED_MoveObject(&g_Title_AnimationManager, "ShowCPS", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
        OLED_MoveObject(&g_Title_AnimationManager, "FastMode", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
        OLED_MoveObject(&g_Title_AnimationManager, "TriggerThre", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
        OLED_MoveObject(&g_Title_AnimationManager, "CurrentKey", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
        OLED_MoveObject(&g_Title_AnimationManager, "PressSpeed", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 5, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 5, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
        OLED_MoveObject(&g_Title_AnimationManager, "ReleseSpeed", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 6, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 6, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);

        OLED_MoveObject(&g_AnimationManager, "SquarePos", OLED_WIDTH + 5, 10, OLED_WIDTH + 5, 10, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);
        OLED_MoveObject(&g_AnimationManager, "SquareSca", 0, 0, 0, 0, OLED_UI_TWEEN_DURATION, OLED_UI_TWEEN_IN);

        is_first_run = false;
    }
    OLED_Drawer();
    OLED_StateMachine();
}

void OLED_DisplayUI(keyboard_settings_t *settings, keyboard_state_t *state)
{
    OLED_ClearBuffer(); // 清除OLED缓冲区
    Get_OLED_Input();
    OLED_UI_Loop();
    
    // OLED_OptimizedDisplayFPS(84, 45);
    OLED_UpdateAnimationManager(&g_Icon_AnimationManager); // 更新动画管理器
    OLED_UpdateAnimationManager(&g_Title_AnimationManager); // 更新标题动画管理器
    OLED_UpdateAnimationManager(&g_AnimationManager); // 更新全局动画管理器
    OLED_SmartUpdate(); // 使用智能更新模式
}

void CPS_Counter(void)
{
    static uint64_t last_counter = 0;

    if (msg_counter - last_counter >= 100) // 每100ms计算一次CPS
    {
        cps = (uint16_t) ((msg_counter - last_counter) * 10);
        last_counter = msg_counter;
    }
}

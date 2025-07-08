/*
 * keyboard_controller.c
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */

#include "keyboard_controller.h"
#include "oled_driver.h"

keyboard_settings_t keyboard_settings;
keyboard_state_t keyboard_state;

#define FLASH_START_ADDRESS 0x08000000 // 这是STM32G4的Flash起始地址
#define FLASH_SIZE_PER_SECTOR 0x800 // 每个扇区的大小为2KB

#define KEYBOARD_SETTINGS_FLASH_PAGE_NUM (63)
#define KEYBOARD_SETTINGS_FLASH_START_ADDRESS (FLASH_START_ADDRESS + FLASH_SIZE_PER_SECTOR * KEYBOARD_SETTINGS_FLASH_PAGE_NUM)

#define BYTES_TO_WORDS(bytes) (((bytes) + 3) / 4) // 猜猜我为什么要写这个?
//* G431的核心是MXC4F,需要使用字节对齐来存储数据,这里保证他即使不是4的倍数也能正确存储


static uint8_t keyboard_update_flag = 0; // 键盘更新标志位

// USB HID的协议是第0位为特殊按键(Left Ctrl, Left Shift, Left Alt, Left GUI, Right Ctrl, Right Shift, Right Alt, Right GUI)
// 第1位为保留位0x00
// 第2-7位为普通按键(0x00-0xFF) 这里做的是4key键盘的协议(外加ESC)
uint8_t hid_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void Keyboard_Init(void)
{
    ADC_Init();
    HAL_Delay(100);
    RGB_Init();
    HAL_Delay(100);

    OLED_EnableDiffMode(1);
    OLED_EnableFastUpdate(1);
    OLED_Init();
    // USB已经在main中初始化

    keyboard_state.KEY_1 = false;
    keyboard_state.KEY_2 = false;
    keyboard_state.KEY_3 = false;
    keyboard_state.KEY_4 = false;

    keyboard_state.TouchButton_1 = false;
    keyboard_state.TouchButton_2 = false;
    keyboard_settings.keyboard_mode = 1;
}

void Keyboard_Read_Input(keyboard_settings_t *settings, keyboard_state_t *state)
{
    ADC_Filter();

    state->TouchButton_1 = (HAL_GPIO_ReadPin(ESC_Buttom_GPIO_Port, ESC_Buttom_Pin) == GPIO_PIN_RESET);
    state->TouchButton_2 = (HAL_GPIO_ReadPin(Mode_Buttom_GPIO_Port, Mode_Buttom_Pin) == GPIO_PIN_RESET);

    if (settings->enable_quick_trigger)
    {
        // TODO: 快速触发模式


    }
    else
    {
        // 普通触发模式
        state->KEY_1 = (filter_adc_data[0] > settings->trigger_threshold);
        state->KEY_2 = (filter_adc_data[1] > settings->trigger_threshold);
        state->KEY_3 = (filter_adc_data[2] > settings->trigger_threshold);
        state->KEY_4 = (filter_adc_data[3] > settings->trigger_threshold);
    }
}

void Handle_Mode_Switch(keyboard_settings_t *settings, keyboard_state_t *state)
{
    static bool last_mode_button_state = false;
    if (state->TouchButton_2 && !last_mode_button_state) // 检测按钮下降沿
    {
        settings->keyboard_mode++;
        if (settings->keyboard_mode > 3)
        {
            settings->keyboard_mode = 1;
        }
    }
    last_mode_button_state = state->TouchButton_2;
}

void Keyboard_Updater(keyboard_settings_t *settings, keyboard_state_t *state)
{
    Keyboard_Read_Input(settings, state);
    Handle_Mode_Switch(settings, state);
    hid_buffer[0] = KEYBOARD_BUTTON_NONE; // 特殊按键位
    hid_buffer[1] = KEYBOARD_BUTTON_NONE; // 保留位
    hid_buffer[7] = KEYBOARD_BUTTON_NONE; // 这是padding

    hid_buffer[2] = state->KEY_1 ? KEYBOARD_BUTTON_D : KEYBOARD_BUTTON_NONE;
    hid_buffer[3] = state->KEY_2 ? KEYBOARD_BUTTON_F : KEYBOARD_BUTTON_NONE;
    hid_buffer[4] = state->KEY_3 ? KEYBOARD_BUTTON_J : KEYBOARD_BUTTON_NONE;
    hid_buffer[5] = state->KEY_4 ? KEYBOARD_BUTTON_K : KEYBOARD_BUTTON_NONE;
    hid_buffer[6] = state->TouchButton_1 ? KEYBOARD_BUTTON_ESC : KEYBOARD_BUTTON_NONE;
    
    // 这里的TouchButton_2是模式切换按钮,不需要发送到USB HID
    // 发送USB HID报告
    if (settings->keyboard_mode == 1)
    {
        USBD_HID_SendReport(&hUsbDeviceFS, hid_buffer, sizeof(hid_buffer));
    }
    else
    {
        OLED_DisplayUI(&keyboard_settings, &keyboard_state);
    }
    
    keyboard_update_flag = 0; // 清除更新标志位
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6 && !keyboard_update_flag) // TIM6用于扫描&更新键盘状态
    {
        keyboard_update_flag = 1;
        Keyboard_Updater(&keyboard_settings, &keyboard_state);
    }

    if (htim->Instance == TIM7) // TIM7用于OLED动画计时
    {
        msg_counter++;
        CPS_Counter(); // 计算CPS
    }
}



#pragma region EEPROM Simulation

void Keyboard_Settings_Read(void)
{
    uint32_t* p_settings_in_flash = (uint32_t*)KEYBOARD_SETTINGS_FLASH_START_ADDRESS;

    // 计算Flash中存储的数据的CRC值，并与数据自身存储的CRC值进行比较
    uint32_t crc_check = HAL_CRC_Calculate(&hcrc, p_settings_in_flash, BYTES_TO_WORDS(sizeof(keyboard_settings_t)) - 1);

    // 从Flash中读取CRC值
    uint32_t stored_crc = *(p_settings_in_flash + BYTES_TO_WORDS(sizeof(keyboard_settings_t)) - 1);

    // 比较CRC值
    if (crc_check == stored_crc)
    {
        memcpy(&keyboard_settings, (void*)KEYBOARD_SETTINGS_FLASH_START_ADDRESS, sizeof(keyboard_settings_t));
    }
    else
    {
        keyboard_settings.trigger_threshold = 2000.0f;
        keyboard_settings.trigger_slope = 1.2f;
        keyboard_settings.enable_quick_trigger = 0;
        keyboard_settings.rgb_style = 1;
        keyboard_settings.keyboard_mode = 1; // 默认是键盘输出模式
        keyboard_settings.padding[0] = 0;
        keyboard_settings.padding[1] = 0;
        keyboard_settings.padding[2] = 0;
        keyboard_settings.padding[3] = 0;

        Keyboard_Settings_Save();
    }
}

void Keyboard_Settings_Save(void)
{
    // 计算当前设置的CRC校验和，并存入结构体
    keyboard_settings.crc32 = HAL_CRC_Calculate(&hcrc, (uint32_t*)&keyboard_settings, BYTES_TO_WORDS(sizeof(keyboard_settings_t)) - 1);

    HAL_FLASH_Unlock();

    // 擦除
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = KEYBOARD_SETTINGS_FLASH_PAGE_NUM;
    EraseInitStruct.NbPages     = 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
        // 擦除失败!
        HAL_GPIO_WritePin(SysWork_Light_GPIO_Port, SysWork_Light_Pin, GPIO_PIN_SET);

        HAL_FLASH_Lock();
        return; //! fvv这都搞不好
    }

    // 写入数据 STM32G4是按64位
    uint64_t* data_ptr = (uint64_t*)&keyboard_settings;
    uint32_t write_addr = KEYBOARD_SETTINGS_FLASH_START_ADDRESS;
    uint32_t num_of_64bit_words = (sizeof(keyboard_settings_t) + 7) / 8; // 按64位对齐

    for (int i = 0; i < num_of_64bit_words; i++)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FAST, write_addr, data_ptr[i]) != HAL_OK)
        {
            break; //! fvv这都搞不好
        }
        write_addr += 8;
    }

    HAL_FLASH_Lock();
}

#pragma endregion
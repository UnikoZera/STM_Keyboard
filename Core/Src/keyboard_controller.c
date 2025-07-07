/*
 * keyboard_controller.c
 *
 *  Created on: Jun 28, 2025
 *      Author: UnikoZera
 */

#include "keyboard_controller.h"

keyboard_settings_t keyboard_settings;

#define FLASH_START_ADDRESS 0x08000000 // 这是STM32G4的Flash起始地址
#define FLASH_SIZE_PER_SECTOR 0x800 // 每个扇区的大小为2KB

#define KEYBOARD_SETTINGS_FLASH_PAGE_NUM (63)
#define KEYBOARD_SETTINGS_FLASH_START_ADDRESS (FLASH_START_ADDRESS + FLASH_SIZE_PER_SECTOR * KEYBOARD_SETTINGS_FLASH_PAGE_NUM)

#define BYTES_TO_WORDS(bytes) (((bytes) + 3) / 4) // 猜猜我为什么要写这个?
//* G431的核心是MXC4F,需要使用字节对齐来存储数据,这里保证他即使不是4的倍数也能正确存储

static uint8_t keyboard_mode = 1; // 1是键盘输出模式、2是RGB灯光模式、3是调整键盘阈值大小和开启快速触发模式
static uint8_t keyboard_update_flag = 0; // 键盘更新标志位

// USB HID的协议是第0位为特殊按键(Left Ctrl, Left Shift, Left Alt, Left GUI, Right Ctrl, Right Shift, Right Alt, Right GUI)
// 第1位为保留位0x00
// 第2-6位为普通按键(0x00-0xFF) 这里做的是4key盘的协议
uint8_t hid_buffer[6] = {0};

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
}

void Keyboard_Update(void)
{
    // 读取ADC数据
    ADC_Filter();

    // 读取mode按钮
    if (HAL_GPIO_ReadPin(Mode_Buttom_GPIO_Port, Mode_Buttom_Pin) == GPIO_PIN_RESET)
    {
        HAL_Delay(50); // 防抖
        if (HAL_GPIO_ReadPin(Mode_Buttom_GPIO_Port, Mode_Buttom_Pin) == GPIO_PIN_RESET)
        {
            keyboard_mode++;
            if (keyboard_mode > 3) // 模式切换，1为键盘模式，2为RGB灯光模式，3为调整键盘阈值大小和开启快速触发模式
            {
                keyboard_mode = 1;
            }
        }
        while (HAL_GPIO_ReadPin(Mode_Buttom_GPIO_Port, Mode_Buttom_Pin) == GPIO_PIN_RESET);// 等待按键释放
    }

    if (keyboard_mode == 1)
    {
        // 读取按键状态
        hid_buffer[0] = KEYBOARD_BUTTON_NONE; // 特殊按键
        hid_buffer[1] = KEYBOARD_BUTTON_NONE; // 保留位

        if (filter_adc_data[0] > keyboard_settings.trigger_threshold)
        {
            hid_buffer[2] = KEYBOARD_BUTTON_D;
        }
        else
        {
            hid_buffer[2] = KEYBOARD_BUTTON_NONE;
        }

        if (filter_adc_data[1] > keyboard_settings.trigger_threshold)
        {
            hid_buffer[3] = KEYBOARD_BUTTON_F;
        }
        else
        {
            hid_buffer[3] = KEYBOARD_BUTTON_NONE;
        }
        
        if (filter_adc_data[2] > keyboard_settings.trigger_threshold)
        {
            hid_buffer[4] = KEYBOARD_BUTTON_J;
        }
        else
        {
            hid_buffer[4] = KEYBOARD_BUTTON_NONE;
        }

        if (filter_adc_data[3] > keyboard_settings.trigger_threshold)
        {
            hid_buffer[5] = KEYBOARD_BUTTON_K;
        }
        else
        {
            hid_buffer[5] = KEYBOARD_BUTTON_NONE;
        }

        if (keyboard_settings.enable_quick_trigger)
        {
            if (filter_adc_data[0] / last_adc_data[0] > keyboard_settings.trigger_slope)
            {
                hid_buffer[2] = KEYBOARD_BUTTON_D;
            }
            else if (last_adc_data[0] / filter_adc_data[0] > keyboard_settings.trigger_slope)
            {
                hid_buffer[2] = KEYBOARD_BUTTON_NONE;
            }

            if (filter_adc_data[1] / last_adc_data[1] > keyboard_settings.trigger_slope)
            {
                hid_buffer[3] = KEYBOARD_BUTTON_F;
            }
            else if (last_adc_data[1] / filter_adc_data[1] > keyboard_settings.trigger_slope)
            {
                hid_buffer[3] = KEYBOARD_BUTTON_NONE;
            }

            if (filter_adc_data[2] / last_adc_data[2] > keyboard_settings.trigger_slope)
            {
                hid_buffer[4] = KEYBOARD_BUTTON_J;
            }
            else if (last_adc_data[2] / filter_adc_data[2] > keyboard_settings.trigger_slope)
            {
                hid_buffer[4] = KEYBOARD_BUTTON_NONE;
            }

            if (filter_adc_data[3] / last_adc_data[3] > keyboard_settings.trigger_slope)
            {
                hid_buffer[5] = KEYBOARD_BUTTON_K;
            }
            else if (last_adc_data[3] / filter_adc_data[3] > keyboard_settings.trigger_slope)
            {
                hid_buffer[5] = KEYBOARD_BUTTON_NONE;
            }
        }

        USBD_HID_SendReport(&hUsbDeviceFS, hid_buffer, sizeof(hid_buffer));
    }
    else if (keyboard_mode == 2)
    {
        OLED_DisplayString(0, 0, "Mode:RGB Stylers");
        if (filter_adc_data[0] > keyboard_settings.trigger_threshold)
        {
            keyboard_settings.rgb_style--;
        }

        if (filter_adc_data[1] > keyboard_settings.trigger_threshold)
        {
            // UP
        }

        if (filter_adc_data[2] > keyboard_settings.trigger_threshold)
        {
            // DOWN
        }

        if (filter_adc_data[3] > keyboard_settings.trigger_threshold)
        {
            keyboard_settings.rgb_style++;
        }
    }
    else if (keyboard_mode == 3)
    {
        OLED_DisplayString(0, 0, "Mode:Keyboard Set");
    }

    keyboard_update_flag = 0; // 清除更新标志位
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6 && !keyboard_update_flag) // TIM6用于扫描&更新键盘状态
    {
        keyboard_update_flag = 1; // 设置标志位，表示需要更新
        Keyboard_Update();
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
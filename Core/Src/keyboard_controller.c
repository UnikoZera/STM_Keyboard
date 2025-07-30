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
#define CALCULATE_PRESS_SPEED(adc_value, last_adc_value, time_diff) \
    ((adc_value - last_adc_value) * 1000 / time_diff) // 计算按下速度,单位是ADC值/ms

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
    OLED_UI_Init();
    // USB已经在main中初始化

    // 初始化键盘状态
    Keyboard_Settings_Read();

    HAL_Delay(100);

    magnetic_key_info_t *keys_info_init[4] = 
    {
        &keyboard_state.KEY_1_info,
        &keyboard_state.KEY_2_info,
        &keyboard_state.KEY_3_info,
        &keyboard_state.KEY_4_info
    };

    uint16_t keys_threshold_init[12] = 
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

    for (int i = 0; i < 4; i++)
    {
        keys_info_init[i]->key_state = KEY_RELEASED;
        keys_info_init[i]->trigger_position_threshold = keys_threshold_init[i]; // 触发位置阈值
        keys_info_init[i]->trigger_speed_threshold = keys_threshold_init[i + 4]; // 触发速度阈值
        keys_info_init[i]->release_speed_threshold = keys_threshold_init[i + 8]; // 释放速度阈值
        keys_info_init[i]->press_time = HAL_GetTick(); // 初始化按下时间戳
        keys_info_init[i]->press_speed = 0; // 初始化按下速度
    }

    keyboard_state.TouchButton_1 = false;
    keyboard_state.TouchButton_2 = false;
    keyboard_settings.keyboard_mode = 1;
}

// Helper Func
static void update_key_state(magnetic_key_info_t *key_info, float current_adc, uint16_t pos_threshold, int16_t speed_trigger, int16_t speed_release)
{
    switch (key_info->key_state)
    {
        case KEY_RELEASED:
            if (current_adc >= pos_threshold)
            {
                key_info->key_state = KEY_PRESSED;
                key_info->press_time = HAL_GetTick();
            }
            else if (key_info->press_speed >= speed_trigger)
            {
                key_info->key_state = KEY_PRESSING;
                key_info->press_time = HAL_GetTick();
            }
            break;
        case KEY_PRESSING:
            if (current_adc >= pos_threshold)
            {
                key_info->key_state = KEY_PRESSED;
                key_info->press_time = HAL_GetTick();
            }
            else if (key_info->press_speed < speed_trigger) // 可能是有点抖动造成的
            {
                key_info->key_state = KEY_RELEASED;
                key_info->press_time = HAL_GetTick();
            }
            else if (key_info->press_speed < speed_release)
            {
                key_info->key_state = KEY_RELEASING;
                key_info->press_time = HAL_GetTick();
            }
            break;
        case KEY_PRESSED:
            if (current_adc < pos_threshold)
            {
                key_info->key_state = KEY_RELEASED;
                key_info->press_time = HAL_GetTick();
            }
            else if (key_info->press_speed < speed_release)
            {
                key_info->key_state = KEY_RELEASING;
                key_info->press_time = HAL_GetTick();
            }
            break;
        case KEY_RELEASING:
            if (current_adc < pos_threshold)
            {
                key_info->key_state = KEY_RELEASED;
                key_info->press_time = HAL_GetTick();
            }
            else if (key_info->press_speed > speed_release)
            {
                key_info->key_state = KEY_PRESSED;
                key_info->press_time = HAL_GetTick();
            }
            else if (key_info->press_speed >= speed_trigger)
            {
                key_info->key_state = KEY_PRESSING;
                key_info->press_time = HAL_GetTick();
            }
            break;

    default:
        key_info->key_state = KEY_RELEASED;
        key_info->press_time = HAL_GetTick();
        break;
    }
}

void Keyboard_Read_Input(keyboard_settings_t *settings, keyboard_state_t *state)
{
    ADC_Filter();

    state->TouchButton_1 = (HAL_GPIO_ReadPin(ESC_Buttom_GPIO_Port, ESC_Buttom_Pin) == GPIO_PIN_SET);
    state->TouchButton_2 = (HAL_GPIO_ReadPin(Mode_Buttom_GPIO_Port, Mode_Buttom_Pin) == GPIO_PIN_SET);

    // 状态机部分
    magnetic_key_info_t *keys[4] = 
    {
        &state->KEY_1_info,
        &state->KEY_2_info,
        &state->KEY_3_info,
        &state->KEY_4_info
    };

    uint16_t pos_thresholds[4] = 
    {
        settings->_1trigger_position_threshold,
        settings->_2trigger_position_threshold,
        settings->_3trigger_position_threshold,
        settings->_4trigger_position_threshold
    };

    int16_t speed_triggers[4] = 
    {
        settings->_1trigger_speed_threshold,
        settings->_2trigger_speed_threshold,
        settings->_3trigger_speed_threshold,
        settings->_4trigger_speed_threshold
    };

    int16_t speed_releases[4] = 
    {
        settings->_1release_speed_threshold,
        settings->_2release_speed_threshold,
        settings->_3release_speed_threshold,
        settings->_4release_speed_threshold
    };

    for (int i = 0; i < 4; i++)
    {
        keys[i]->press_speed = CALCULATE_PRESS_SPEED(filter_adc_data[i], last_adc_data[i], 1);
        update_key_state(keys[i], filter_adc_data[i], pos_thresholds[i], speed_triggers[i], speed_releases[i]);
    }
}

void Keyboard_Updater(keyboard_settings_t *settings, keyboard_state_t *state)
{
    Keyboard_Read_Input(settings, state);
    hid_buffer[0] = KEYBOARD_BUTTON_NONE; // 特殊按键位
    hid_buffer[1] = KEYBOARD_BUTTON_NONE; // 保留位

    if (settings->enable_quick_trigger)
    {
        if (state->KEY_1_info.key_state == KEY_PRESSED || state->KEY_1_info.key_state == KEY_PRESSING)
        {
            hid_buffer[2] = KEYBOARD_BUTTON_D;
        }
        else if (state->KEY_1_info.key_state == KEY_RELEASED || state->KEY_1_info.key_state == KEY_RELEASING)
        {
            hid_buffer[2] = KEYBOARD_BUTTON_NONE;
        }

        if (state->KEY_2_info.key_state == KEY_PRESSED || state->KEY_2_info.key_state == KEY_PRESSING)
        {
            hid_buffer[3] = KEYBOARD_BUTTON_F;
        }
        else if (state->KEY_2_info.key_state == KEY_RELEASED || state->KEY_2_info.key_state == KEY_RELEASING)
        {
            hid_buffer[3] = KEYBOARD_BUTTON_NONE;
        }

        if (state->KEY_3_info.key_state == KEY_PRESSED || state->KEY_3_info.key_state == KEY_PRESSING)
        {
            hid_buffer[4] = KEYBOARD_BUTTON_J;
        }
        else if (state->KEY_3_info.key_state == KEY_RELEASED || state->KEY_3_info.key_state == KEY_RELEASING)
        {
            hid_buffer[4] = KEYBOARD_BUTTON_NONE;
        }

        if (state->KEY_4_info.key_state == KEY_PRESSED || state->KEY_4_info.key_state == KEY_PRESSING)
        {
            hid_buffer[5] = KEYBOARD_BUTTON_K;
        }
        else if (state->KEY_4_info.key_state == KEY_RELEASED || state->KEY_4_info.key_state == KEY_RELEASING)
        {
            hid_buffer[5] = KEYBOARD_BUTTON_NONE;
        }
    }
    else
    {
        if (state->KEY_1_info.key_state == KEY_PRESSED)
        {
            hid_buffer[2] = KEYBOARD_BUTTON_D;
        }
        else if (state->KEY_1_info.key_state == KEY_RELEASED)
        {
            hid_buffer[2] = KEYBOARD_BUTTON_NONE;
        }

        if (state->KEY_2_info.key_state == KEY_PRESSED)
        {
            hid_buffer[3] = KEYBOARD_BUTTON_F;
        }
        else if (state->KEY_2_info.key_state == KEY_RELEASED)
        {
            hid_buffer[3] = KEYBOARD_BUTTON_NONE;
        }

        if (state->KEY_3_info.key_state == KEY_PRESSED)
        {
            hid_buffer[4] = KEYBOARD_BUTTON_J;
        }
        else if (state->KEY_3_info.key_state == KEY_RELEASED)
        {
            hid_buffer[4] = KEYBOARD_BUTTON_NONE;
        }

        if (state->KEY_4_info.key_state == KEY_PRESSED)
        {
            hid_buffer[5] = KEYBOARD_BUTTON_K;
        }
        else if (state->KEY_4_info.key_state == KEY_RELEASED)
        {
            hid_buffer[5] = KEYBOARD_BUTTON_NONE;
        }

    }
    
    hid_buffer[6] = state->TouchButton_1 ? KEYBOARD_BUTTON_ESC : KEYBOARD_BUTTON_NONE;
    hid_buffer[7] = KEYBOARD_BUTTON_NONE; // 这是padding

    // 这里的TouchButton_2是模式切换按钮,不需要发送到USB HID
    // 发送USB HID报告
    if (settings->keyboard_mode == 1)
    {
        USBD_HID_SendReport(&hUsbDeviceFS, hid_buffer, sizeof(hid_buffer));
    }

    keyboard_update_flag = 0; // 清除更新标志位
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) // 这里是1000hz的定时器中断
{
    if (htim->Instance == TIM6 && !keyboard_update_flag) // TIM6用于扫描&更新键盘状态
    {
        keyboard_update_flag = 1;
        Keyboard_Updater(&keyboard_settings, &keyboard_state);
    }

    if (htim->Instance == TIM6)
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

        keyboard_settings.keyboard_mode = 1; // 默认模式
        keyboard_settings.rgb_style = 1; // 默认RGB风格
        keyboard_settings.enable_quick_trigger = 0; // 默认不开启快速触发
        keyboard_settings._1trigger_position_threshold = DEFAULT_TRIGGER_POSITION_THRESHOLD;
        keyboard_settings._1trigger_speed_threshold = DEFAULT_TRIGGER_SPEED_THRESHOLD; // 默认触发速度阈值
        keyboard_settings._1release_speed_threshold = DEFAULT_RELEASE_SPEED_THRESHOLD; // 默认释放速度

        keyboard_settings._2trigger_position_threshold = DEFAULT_TRIGGER_POSITION_THRESHOLD;
        keyboard_settings._2trigger_speed_threshold = DEFAULT_TRIGGER_SPEED_THRESHOLD; // 默认触发速度
        keyboard_settings._2release_speed_threshold = DEFAULT_RELEASE_SPEED_THRESHOLD; // 默认释放速度

        keyboard_settings._3trigger_position_threshold = DEFAULT_TRIGGER_POSITION_THRESHOLD;
        keyboard_settings._3trigger_speed_threshold = DEFAULT_TRIGGER_SPEED_THRESHOLD; // 默认触发速度
        keyboard_settings._3release_speed_threshold = DEFAULT_RELEASE_SPEED_THRESHOLD; // 默认释放速度

        keyboard_settings._4trigger_position_threshold = DEFAULT_TRIGGER_POSITION_THRESHOLD;
        keyboard_settings._4trigger_speed_threshold = DEFAULT_TRIGGER_SPEED_THRESHOLD; // 默认触发速度
        keyboard_settings._4release_speed_threshold = DEFAULT_RELEASE_SPEED_THRESHOLD; // 默认释放速度

        // Keyboard_Settings_Save(); // Debug时候可以关闭
    }
}

void Keyboard_Settings_Save(void)
{
    // 更改当前settings
    keyboard_settings._1trigger_position_threshold = keyboard_state.KEY_1_info.trigger_position_threshold;
    keyboard_settings._1trigger_speed_threshold = keyboard_state.KEY_1_info.trigger_speed_threshold;
    keyboard_settings._1release_speed_threshold = keyboard_state.KEY_1_info.release_speed_threshold;

    keyboard_settings._2trigger_position_threshold = keyboard_state.KEY_2_info.trigger_position_threshold;
    keyboard_settings._2trigger_speed_threshold = keyboard_state.KEY_2_info.trigger_speed_threshold;
    keyboard_settings._2release_speed_threshold = keyboard_state.KEY_2_info.release_speed_threshold;

    keyboard_settings._3trigger_position_threshold = keyboard_state.KEY_3_info.trigger_position_threshold;
    keyboard_settings._3trigger_speed_threshold = keyboard_state.KEY_3_info.trigger_speed_threshold;
    keyboard_settings._3release_speed_threshold = keyboard_state.KEY_3_info.release_speed_threshold;

    keyboard_settings._4trigger_position_threshold = keyboard_state.KEY_4_info.trigger_position_threshold;
    keyboard_settings._4trigger_speed_threshold = keyboard_state.KEY_4_info.trigger_speed_threshold;
    keyboard_settings._4release_speed_threshold = keyboard_state.KEY_4_info.release_speed_threshold;

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
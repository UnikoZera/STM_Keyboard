/*
 * icon.h
 *
 *  Created on: May 30, 2025
 *      Author: UnikoZera
 */

#ifndef INC_ICON_H_
#define INC_ICON_H_


//水平扫描，从上往下，从左往右 36X36分辨率
extern unsigned char IMG_MENU_DATA[180]; 
extern unsigned char IMG_TOOL_DATA[180];
extern unsigned char IMG_RGB_DATA[180];
extern unsigned char IMG_SETTINGS_DATA[180];

typedef enum
{
    ICON_MENU,
    ICON_TOOL,
    ICON_RGB,
    ICON_SETTINGS,
} IconType_t;

#endif /* INC_ICON_H_ */

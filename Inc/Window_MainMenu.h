/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WINDOW_MAINMENU_H
#define __WINDOW_MAINMENU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include system header files -----------------------------------------------*/
// Include user header files
#include "UserCommon.h"
#include "bmp_rgb565.h"

// Callee of this window
//#include "Window_Templete.h"

/* Exported macro ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define MAINMENU_WINDOW_TITLE	        "MAIN MENU"
#define MAINMENU_uGUI_OBJECTS_NUM       10
#define MAINMENU_UPDATE_MS              50
#define MAINMENU_BUTTON_GAP             5

#define BMP_Xpos                        250
#define BMP_Ypos                        50
#define BMP_WIDTH                       100     /* width must be a multiple of 4 */
#define BMP_HEIGHT                      100

/* Exported function macro ---------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported enum tag ---------------------------------------------------------*/
/* Exported struct/union tag -------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
// FreeRTOS
TaskHandle_t xHandle_MainMenu;

UG_WINDOW wnd_MainMenu;

/* Exported function prototypes ----------------------------------------------*/
void createMainMenuWindow(void);

#ifdef __cplusplus
}
#endif

#endif /* __WINDOW_MAINMENU_H */

/***************************************************************END OF FILE****/

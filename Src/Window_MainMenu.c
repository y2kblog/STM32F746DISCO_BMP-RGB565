/* Include system header files -----------------------------------------------*/
/* Include user header files -------------------------------------------------*/
#include "Window_MainMenu.h"

/* Imported variables --------------------------------------------------------*/
/* Private function macro ----------------------------------------------------*/
/* Private enum tag ----------------------------------------------------------*/
static enum BTN_ID_TAG
{
    BTN_ID_Test = BTN_ID_0
};

static enum TXB_ID_TAG
{
    TXB_ID_Test = TXB_ID_0
};

/* Private variables ---------------------------------------------------------*/
/* "this" pointer */
static TaskHandle_t* pthis_xHandle = &xHandle_MainMenu;
static UG_WINDOW*    pthis_wnd     = &wnd_MainMenu;

// uGUI
static UG_OBJECT    obj_this_wnd[MAINMENU_uGUI_OBJECTS_NUM];
static UG_BUTTON    btn_Test;
static UG_TEXTBOX   txb_Test;

/* thread control */
static bool needFinalize;  // This flag is used in "WindowControlThread" and "window_callback" function

/* BMP */
static uint8_t* pBMP;

/* Private function prototypes -----------------------------------------------*/
static void WindowControlThread(void const *argument);
static void window_callback( UG_MESSAGE* msg );
static void initialize(void);
static void execute(void);
static void draw(void);
static void finalize(void);

/* Exported functions --------------------------------------------------------*/
/* ---------------------------------------------------------------- */
/* -- Create Window                                              -- */
/* ---------------------------------------------------------------- */
void createMainMenuWindow(void)
{
	UG_U8 id_buf;

    UG_WindowCreate(pthis_wnd, obj_this_wnd, COUNTOF(obj_this_wnd), window_callback);
    UG_WindowSetTitleText(pthis_wnd, MAINMENU_WINDOW_TITLE);
    UG_WindowSetTitleTextFont(pthis_wnd, &FONT_12X20);
	
    // Create buttons
    id_buf = BTN_ID_Test;
    UG_ButtonCreate(pthis_wnd, &btn_Test, id_buf,
            UG_WindowGetInnerWidth(pthis_wnd)  * 0 / 2 + MAINMENU_BUTTON_GAP,
            UG_WindowGetInnerHeight(pthis_wnd) * 1 / 4 + MAINMENU_BUTTON_GAP,
            UG_WindowGetInnerWidth(pthis_wnd)  * 1 / 2 - MAINMENU_BUTTON_GAP,
            UG_WindowGetInnerHeight(pthis_wnd) * 2 / 4 - MAINMENU_BUTTON_GAP );
    UG_ButtonSetFont(pthis_wnd, id_buf, &FONT_12X20);
    UG_ButtonSetText(pthis_wnd, id_buf, "Toggle LED1");
	
    // Create textboxes
    id_buf = TXB_ID_Test;
    UG_TextboxCreate(pthis_wnd, &txb_Test, id_buf,
            UG_WindowGetInnerWidth(pthis_wnd)  * 0 / 2 + 0,
            UG_WindowGetInnerHeight(pthis_wnd) * 0 / 4 + 0,
            UG_WindowGetInnerWidth(pthis_wnd)  * 1 / 2 - 0,
            UG_WindowGetInnerHeight(pthis_wnd) * 1 / 4 - 0 );
    UG_TextboxSetFont(pthis_wnd, id_buf, &FONT_12X20);
    UG_TextboxSetText(pthis_wnd, id_buf, "Hello uGUI!");
    UG_TextboxSetAlignment(pthis_wnd, id_buf, ALIGN_CENTER_LEFT);
    
	UG_WindowShow(pthis_wnd);
    
    xTaskCreate( (TaskFunction_t)WindowControlThread, "MainMenuTask",
    		configMINIMAL_STACK_SIZE+100, NULL, Priority_Low, pthis_xHandle);
}


/* Private functions ---------------------------------------------------------*/

/* ---------------------------------------------------------------- */
/* -- Thread                                                     -- */
/* ---------------------------------------------------------------- */
static void WindowControlThread(void const *argument)
{
#ifdef PRINTF_DEBUG_MDOE
    printf("%s thread start\r\n", pcTaskGetName(*pthis_xHandle) );
#endif
    
	/* Variables initialization ------------------------------------*/
    bool shouldSuspend  = false;    /* Main Menu thread must not be suspended */
    bool needInitialize = false;
    	 needFinalize   = false;    /* only "needFinalize" flag is changed from "window_callback" function */
	
    if (!shouldSuspend)
        initialize();
    
    while (1)
    {
    	/* Thread flow */
    	vTaskDelay(MAINMENU_UPDATE_MS);
		
    	if(shouldSuspend)
    	{
#ifdef PRINTF_DEBUG_MDOE
    	    printf("%s thread suspend\r\n", pcTaskGetName(*pthis_xHandle) );
#endif
    		vTaskSuspend(NULL);     // <- start here when resume
#ifdef PRINTF_DEBUG_MDOE
    		printf("%s thread resume\r\n", pcTaskGetName(*pthis_xHandle) );
#endif
    		shouldSuspend  = false;
    		needInitialize = true;
    	}

        if(needInitialize)
        {
        	initialize();
        	needInitialize = false;
        }

        execute();
        draw();

        if(needFinalize)
        {
        	finalize();
        	needFinalize  = false;
        	shouldSuspend = true;
        }
    }
}

/* ---------------------------------------------------------------- */
/* -- Callback function ( called from "UG_Update()" )            -- */
/* ---------------------------------------------------------------- */
void window_callback(UG_MESSAGE* msg)
{
    if (msg->type == MSG_TYPE_OBJECT)
    {
        if (msg->id == OBJ_TYPE_BUTTON)
        {
            switch (msg->sub_id)
            {
            case BTN_ID_Test:
#ifdef PRINTF_DEBUG_MDOE
                printf("Push Toggle button\r\n");
#endif
                BSP_LED_Toggle(LED1);
				//needFinalize = true;  // <- Finalize and make "WindowControlThread()" state susupend
				//vTaskResume(xHandle_Templete);
                break;
                
            default:
#ifdef PRINTF_DEBUG_MDOE
                printf("%s thread callback error\r\n", pcTaskGetName(*pthis_xHandle) );
#endif
                break;
            }
        }
    }
}


/* ---------------------------------------------------------------- */
/* -- Initialize                                                 -- */
/* ---------------------------------------------------------------- */
static void initialize(void)
{
	/* Variables Initialization */
    pBMP = BMP_565_Create(BMP_WIDTH, BMP_HEIGHT);
    if (pBMP == NULL)
    {
#ifdef PRINTF_DEBUG_MDOE
        printf("BMP memory allocation error\r\n");
#endif
    }
    
    /* Show this window */
    UG_WindowShow(pthis_wnd);
}

/* ---------------------------------------------------------------- */
/* -- Execute                                                    -- */
/* ---------------------------------------------------------------- */
static void execute(void)
{
    
}

/* ---------------------------------------------------------------- */
/* -- Draw                                                       -- */
/* ---------------------------------------------------------------- */
static void draw(void)
{
    // Gradation
    for (uint32_t x = 0; x < BMP_WIDTH; x++)
    {
        for (uint32_t y = 0; y < BMP_HEIGHT; y++)
        {
            uint8_t r, g, b;
            r = 0xFF - x;
            g = 0xFF - y;
            b = 0xFF;
            //b = 0xFF - (xTaskGetTickCount() / 50);
            BMP_565_SetPixelRGB(pBMP, x, y, r, g, b);
        }
    }
    
    // Draw Line
    BMP_565_DrawLineRGB(pBMP, 0, BMP_HEIGHT/2, BMP_WIDTH/2, 0, 0, 0, 0);
    BMP_565_DrawLineRGB(pBMP, BMP_WIDTH/2, 0, BMP_WIDTH-1, BMP_HEIGHT/2, 0, 0, 0);
    BMP_565_DrawLineRGB(pBMP, BMP_WIDTH-1, BMP_HEIGHT/2, BMP_WIDTH/2, BMP_HEIGHT-1, 0, 0, 0);
    BMP_565_DrawLineRGB(pBMP, BMP_WIDTH/2, BMP_HEIGHT-1, 0, BMP_HEIGHT/2, 0, 0, 0);
    
    // Draw rectangle
    BMP_565_DrawRectRGB(pBMP, BMP_WIDTH/2 - 10, BMP_HEIGHT/2 - 10, BMP_WIDTH/2 + 10, BMP_HEIGHT/2 + 10, 0xFF, 0xFF, 0);
    
    
    // Display
    BSP_LCD_DrawBitmap(BMP_Xpos, BMP_Ypos, pBMP);
}

/* ---------------------------------------------------------------- */
/* -- Finalize                                                   -- */
/* ---------------------------------------------------------------- */
static void finalize(void)
{
    /* Variables Finalization */
    BMP_565_Free(pBMP);
}

/***************************************************************END OF FILE****/

#ifndef _BMP_RGB565_H_
#define _BMP_RGB565_H_

#include <stdint.h>

/* Macro */
#define COLOR_R(_C_COLOR_)      (uint8_t)((_C_COLOR_)>>16)
#define COLOR_G(_C_COLOR_)      (uint8_t)((_C_COLOR_)>>8)
#define COLOR_B(_C_COLOR_)      (uint8_t)(_C_COLOR_)
#define COL_RGB_SET(_C_COLOR_)  COLOR_R(_C_COLOR_),COLOR_G(_C_COLOR_),COLOR_B(_C_COLOR_)

/*********************************** Public methods **********************************/
uint8_t*    BMP_565_Create      (uint32_t width, uint32_t height);
void        BMP_565_Free        (uint8_t* pbmp);
uint32_t    BMP_565_GetWidth    (uint8_t* pbmp);
uint32_t    BMP_565_GetHeight   (uint8_t* pbmp);
uint32_t    BMP_565_GetFileSize (uint8_t* pbmp);
uint32_t    BMP_565_GetImageSize(uint8_t* pbmp);
void        BMP_565_SetPixelRGB (uint8_t* pbmp, uint32_t x, uint32_t y, uint8_t  r, uint8_t  g, uint8_t  b );
void        BMP_565_GetPixelRGB (uint8_t* pbmp, uint32_t x, uint32_t y, uint8_t* r, uint8_t* g, uint8_t* b );
void        BMP_565_DrawLineRGB (uint8_t* pbmp, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t  r, uint8_t  g, uint8_t  b );
void        BMP_565_DrawRectRGB (uint8_t* pbmp, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint8_t  r, uint8_t  g, uint8_t  b );
void        BMP_565_FillRGB     (uint8_t* pbmp, uint8_t  r, uint8_t  g, uint8_t  b );
void        BMP_565_Copy        (uint8_t* pbmp_Dst, uint8_t* pbmp_Src);

#endif  // _BMP_RGB565_H_

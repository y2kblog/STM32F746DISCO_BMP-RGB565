#include "bmp_rgb565.h"
#include <stdlib.h>
#include <string.h>


static const uint32_t FileHeaderSize  = 14; /* = 0x0E */
static const uint32_t InfoHeaderSize  = 40; /* = 0x28 */
static const uint32_t BitFieldSize    = 16;
static const uint32_t AllHeaderOffset = 70; /* FileHeaderSize + InfoHeaderSize + BitFieldSize */

/* Private function prototypes */
static inline uint16_t convertRGBtoRGB565(uint8_t r, uint8_t g, uint8_t b);
static inline uint32_t Get_bytes_per_row(uint32_t width);
static uint32_t Read_uint32_t(uint8_t* pSrc);
static uint16_t Read_uint16_t(uint8_t* pSrc);
static void Write_uint32_t(uint32_t Src, uint8_t* pDst);
static void Write_uint16_t(uint16_t Src, uint8_t* pDst);


uint8_t* BMP_565_Create(uint32_t width, uint32_t height)
{
    uint8_t* pbmp;
    uint32_t bytes_per_row = Get_bytes_per_row(width);
    uint32_t image_size = bytes_per_row * height;
    uint32_t data_size = AllHeaderOffset + image_size;

    /* Allocate the bitmap data */
    pbmp = calloc( data_size, sizeof( uint8_t ) );
    if (pbmp == NULL)
        return NULL;

    // Set header's default values
    uint8_t* tmp = pbmp;
    *(tmp  +  0) = 0x42;                            // 'B' : Magic number
    *(tmp  +  1) = 0x4D;                            // 'M' : Magic number
    Write_uint32_t(data_size        , tmp + 0x02);  // File Size
    Write_uint16_t(0                , tmp + 0x06);  // Reserved1
    Write_uint16_t(0                , tmp + 0x08);  // Reserved2
    Write_uint32_t(AllHeaderOffset  , tmp + 0x0A);  // Offset
    tmp += FileHeaderSize;    // Next

    // Info header
    Write_uint32_t( InfoHeaderSize + BitFieldSize, tmp + 0x00);   // HeaderSize
    Write_uint32_t( width           , tmp + 0x04);  // width
    Write_uint32_t( height          , tmp + 0x08);  // height
    Write_uint16_t( 1               , tmp + 0x0C);  // planes
    Write_uint16_t( 16              , tmp + 0x0E);  // Bit count
    Write_uint32_t( 3               , tmp + 0x10);  // Bit compression
    Write_uint32_t( image_size      , tmp + 0x14);  // Image size
    Write_uint32_t( 0               , tmp + 0x18);  // X pixels per meter
    Write_uint32_t( 0               , tmp + 0x1C);  // Y pixels per meter
    Write_uint32_t( 0               , tmp + 0x20);  // Color index
    Write_uint32_t( 0               , tmp + 0x24);  // Important index
    tmp += InfoHeaderSize;    // Next

    // Bit field
    Write_uint32_t( 0x0000F800      , tmp + 0x00);  // red
    Write_uint32_t( 0x000007E0      , tmp + 0x04);  // green
    Write_uint32_t( 0x0000001F      , tmp + 0x08);  // blue
    Write_uint32_t( 0x00000000      , tmp + 0x0C);  // reserved

    return pbmp;
}


void BMP_565_Free(uint8_t* pbmp)
{
    free(pbmp);
}


uint32_t BMP_565_GetWidth(uint8_t* pbmp)
{
    return Read_uint32_t(pbmp + FileHeaderSize + 0x04);
}


uint32_t BMP_565_GetHeight(uint8_t* pbmp)
{
    return Read_uint32_t(pbmp + FileHeaderSize + 0x08);
}


uint32_t BMP_565_GetFileSize(uint8_t* pbmp)
{
    return Read_uint32_t(pbmp + 0x02);
}

uint32_t BMP_565_GetImageSize(uint8_t* pbmp)
{
    return Read_uint32_t(pbmp + FileHeaderSize + 0x14);
}


void BMP_565_SetPixelRGB(uint8_t* pbmp, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t width  = BMP_565_GetWidth (pbmp);
    uint32_t height = BMP_565_GetHeight (pbmp);

    if(pbmp == NULL || x >= width || y >= height)
        return;

    uint32_t bytes_per_row = Get_bytes_per_row(width);
    uint16_t col = convertRGBtoRGB565(r, g, b);

    //Write_uint16_t( col, pbmp + AllHeaderOffset + bytes_per_row * (height - y - 1) + x * 2);
    Write_uint16_t( col, pbmp + AllHeaderOffset + bytes_per_row * (height - y - 1) + (x << 1));
}

void BMP_565_GetPixelRGB(uint8_t* pbmp, uint32_t x, uint32_t y, uint8_t* r, uint8_t* g, uint8_t* b)
{
    uint32_t width  = BMP_565_GetWidth(pbmp);
    uint32_t height = BMP_565_GetHeight(pbmp);

    if(pbmp == NULL || x >= width || y >= height)
        return;

    uint32_t bytes_per_row = Get_bytes_per_row(width);
    uint16_t col = Read_uint16_t(pbmp + bytes_per_row * (height - y - 1) + (x << 1));
    *r = (uint8_t)(col >> 11) << 3;
    *g = (uint8_t)(col >>  5) << 2;
    *b = (uint8_t) col        << 3;
    /*
    *r = (col & 0xF800) >> 8;
    *g = (col & 0x07E0) >> 3;
    *b = (col & 0x001F) << 3;
    */
}


// Bresenham's line algorithm
// ref : https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
// ref : https://ja.wikipedia.org/wiki/%E3%83%96%E3%83%AC%E3%82%BC%E3%83%B3%E3%83%8F%E3%83%A0%E3%81%AE%E3%82%A2%E3%83%AB%E3%82%B4%E3%83%AA%E3%82%BA%E3%83%A0#.E6.9C.80.E9.81.A9.E5.8C.96
void BMP_565_DrawLineRGB(uint8_t* pbmp, int32_t x0, int32_t y0, int32_t x1, int32_t y1,
        uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t width  = BMP_565_GetWidth (pbmp);
    uint32_t height = BMP_565_GetHeight (pbmp);

    if(pbmp == NULL || x0 < 0 || x0 >= width || x1 < 0 || x1 >= width || y0 < 0 || y0 >= height || y1 < 0 || y1 >= height)
        return;

    uint32_t bytes_per_row = Get_bytes_per_row(width);
    uint16_t col = convertRGBtoRGB565(r, g, b);

    int32_t dx = x1 - x0 > 0 ? x1 - x0 : x0 - x1;
    int32_t sx = x0 < x1 ? 1 : -1;
    int32_t dy = y1 - y0 > 0 ? y1 - y0 : y0 - y1;
    int32_t sy = y0 < y1 ? 1 : -1;
    int32_t err = dx - dy;
    int32_t e2;

    uint8_t* pbmp_data = pbmp + AllHeaderOffset;

    for (;;)
    {
        Write_uint16_t( col, pbmp_data + bytes_per_row * (height - y0 - 1) + x0 * 2);

        if (x0 == x1 && y0 == y1)
            break;

        e2 = 2*err;
        if (e2 > -dy) {err -= dy;   x0 += sx;}
        if (e2 <  dx) {err += dx;   y0 += sy;}
    }
}


void BMP_565_DrawRectRGB(uint8_t* pbmp, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,
        uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t width = BMP_565_GetWidth(pbmp);
    uint32_t height = BMP_565_GetHeight(pbmp);

    if (pbmp == NULL || x0 >= width || x1 >= width || y0 >= height || y1 >= height)
        return;

    uint32_t swap;
    if(x0 > x1)
    {
        swap = x0;
        x0 = x1;
        x1 = swap;
    }
    if (y0 > y1)
    {
        swap = y0;
        x0 = y1;
        y1 = swap;
    }

    uint32_t bytes_per_row = Get_bytes_per_row(width);
    uint16_t col = convertRGBtoRGB565(r, g, b);

    uint8_t* pbmp_data = pbmp + AllHeaderOffset;

    uint32_t y_addr_start = bytes_per_row*(height - y0 - 1);
    uint32_t y_addr_end   = bytes_per_row*(height - y1 - 1);
    uint32_t x_addr_start = x0 * 2;
    uint32_t x_addr_end   = x1 * 2;
    for(int32_t y_addr = y_addr_start; y_addr >= y_addr_end; y_addr -= bytes_per_row)
    {
        for(int32_t x_addr = x_addr_start; x_addr <= x_addr_end; x_addr += 2)
            Write_uint16_t( col, pbmp_data + y_addr + x_addr);
    }
}

void BMP_565_FillRGB(uint8_t* pbmp, uint8_t r, uint8_t g, uint8_t b)
{
    if (pbmp == NULL)
        return;

    BMP_565_DrawRectRGB(pbmp, 0, 0, BMP_565_GetWidth(pbmp)-1, BMP_565_GetHeight(pbmp)-1, r, g, b);
}


void BMP_565_Copy(uint8_t* pbmp_Dst, uint8_t* pbmp_Src)
{
    uint32_t image_size = BMP_565_GetImageSize(pbmp_Src);
    for(uint32_t i = AllHeaderOffset; i < image_size; i++)
        *(pbmp_Dst+i) = *(pbmp_Src+i);
}


/*********************************** Private methods **********************************/

static inline uint16_t convertRGBtoRGB565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint16_t) (r >> 3) << 11)
          | ((uint16_t) (g >> 2) << 5)
          |  (uint16_t) (b >> 3);
}

// Calculate the number of bytes used to store a single image row.
// This is always rounded up to the next multiple of 4.
static inline uint32_t Get_bytes_per_row(uint32_t width)
{
    if (width & 0x00000001)
        return ((width + 1) << 1);
    else
        return  (width << 1);

    /*
    uint32_t bytes_per_row = width * 2;
    bytes_per_row += (bytes_per_row % 4 ? 4 - bytes_per_row % 4 : 0 );
     */
}


/**************************************************************
    Reads a little-endian unsigned int from the file.
    Returns non-zero on success.
**************************************************************/
static uint32_t Read_uint32_t(uint8_t* pSrc)
{
    uint32_t retval = 0x00000000;
    retval |= (uint32_t)*(pSrc + 3) << 24;
    retval |= (uint32_t)*(pSrc + 2) << 16;
    retval |= (uint32_t)*(pSrc + 1) <<  8;
    retval |= (uint32_t)*(pSrc    );
    return retval;
}

/**************************************************************
    Reads a little-endian unsigned int from the file.
    Returns non-zero on success.
**************************************************************/
static uint16_t Read_uint16_t(uint8_t* pSrc)
{
    uint16_t retval = 0x0000;
    retval |= (uint16_t)*(pSrc + 1) <<  8;
    retval |= (uint16_t)*pSrc;
    return retval;
}

/**************************************************************
    Writes a little-endian unsigned short int to the file.
    Returns non-zero on success.
**************************************************************/
static void Write_uint32_t(uint32_t Src, uint8_t* pDst)
{
    *(pDst + 3) = (uint8_t)( ( Src & 0xff000000 ) >> 24 );
    *(pDst + 2) = (uint8_t)( ( Src & 0x00ff0000 ) >> 16 );
    *(pDst + 1) = (uint8_t)( ( Src & 0x0000ff00 ) >> 8  );
    *pDst       = (uint8_t)  ( Src & 0x000000ff )        ;
}

/**************************************************************
	Writes a little-endian unsigned short int to the file.
	Returns non-zero on success.
**************************************************************/
static void Write_uint16_t(uint16_t Src, uint8_t* pDst)
{
    *(pDst + 1) = (uint8_t)( ( Src & 0xff00 ) >> 8 );
	*pDst       = (uint8_t)  ( Src & 0x00ff )       ;
}

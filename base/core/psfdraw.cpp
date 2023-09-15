/*
Draws text using psf font
Not totally stealed from wiki.osdev.org
:)
*/

#include <libc.hpp>

#define PSF_FONT_MAGIC 0x864ab572
 
typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;

/* import our font that's in the object file we've created above */
extern char _binary_font_psf_start;
extern char _binary_font_psf_end;
 
uint16_t *unicode;

extern int fx, fy;

void psf_init()
{
    uint16_t glyph = 0;
    /* cast the address to PSF header struct */
    PSF_font *font = (PSF_font*)&_binary_font_psf_start;
    /* is there a unicode table? */
    if (font->flags == 0) {
        unicode = 0;
        return; 
    }
 
    /* get the offset of the table */
    char *s = (char *)(
    (unsigned char*)&_binary_font_psf_start +
      font->headersize +
      font->numglyph * font->bytesperglyph
    );
    fx = font->width;
    fy = font->height;
    unicode = (uint16_t *)malloc(65535*2);
    while((int)s>(int)&_binary_font_psf_end) {
        uint16_t uc = (uint16_t)((unsigned char *)s[0]);
        if(uc == 0xFF) {
            glyph++;
            s++;
            continue;
        } else if(uc & 128) {
            /* UTF-8 to unicode */
            if((uc & 32) == 0 ) {
                uc = ((s[0] & 0x1F)<<6)+(s[1] & 0x3F);
                s++;
            } else
            if((uc & 16) == 0 ) {
                uc = ((((s[0] & 0xF)<<6)+(s[1] & 0x3F))<<6)+(s[2] & 0x3F);
                s+=2;
            } else
            if((uc & 8) == 0 ) {
                uc = ((((((s[0] & 0x7)<<6)+(s[1] & 0x3F))<<6)+(s[2] & 0x3F))<<6)+(s[3] & 0x3F);
                s+=3;
            } else
                uc = 0;
        }
        /* save translation */
        unicode[uc] = glyph;
        s++;
    }
}

/* the linear framebuffer */
extern char *fb;
/* number of bytes in each line, it's possible it's not screen width * bytesperpixel! */
extern int scanline;
 
#define PIXEL uint32_t   /* pixel pointer */
void putpixel(int x, int y, uint32_t color);
void putchar(
    /* note that this is int, not char as it's a unicode character */
    unsigned short int c,
    /* cursor position on screen, in characters not in pixels */
    int cx, int cy,
    /* foreground and background colors, say 0xFFFFFF and 0x000000 */
    uint32_t fg, uint32_t bg)
{
    /* cast the address to PSF header struct */
    PSF_font *font = (PSF_font*)&_binary_font_psf_start;
    /* we need to know how many bytes encode one row */
    int bytesperline=(font->width+7)/8;
    /* unicode translation */
    if(unicode != 0) {
        c = unicode[c];
    }
    /* get the glyph for the character. If there's no
       glyph for a given character, we'll display the first glyph. */
    unsigned char *glyph =
     (unsigned char*)&_binary_font_psf_start +
     font->headersize +
     (c>0&&c<font->numglyph?c:0)*font->bytesperglyph;
    /* calculate the upper left corner on screen where we want to display.
       we only do this once, and adjust the offset later. This is faster. */
    int offs =
        (cy * font->height * scanline) +
        (cx * (font->width + 1) * sizeof(PIXEL));
    /* finally display pixels according to the bitmap */
    int x,y, line,mask;
    for(y=0;y<font->height;y++){
        /* save the starting position of the line */
        //line=offs;
        mask=1<<(font->width-1);
        /* display a row */
        for(x=0;x<font->width;x++){
            //*((PIXEL*)(fb + line)) = *((unsigned int*)glyph) & mask ? fg : bg;
            putpixel(x + cx, y + cy, *((unsigned int*)glyph) & mask ? fg : bg);
            /* adjust to the next pixel */
            mask >>= 1;
            line += sizeof(PIXEL);
        }
        /* adjust to the next line */
        glyph += bytesperline;
        offs  += scanline;
    }
}
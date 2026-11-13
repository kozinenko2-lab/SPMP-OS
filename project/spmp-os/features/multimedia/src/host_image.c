/* HOST ONLY: optional libjpeg/libpng/giflib reference decoders. Not ARM HAL. */
#include "spmp_mm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <jpeglib.h>
#include <png.h>
#include <gif_lib.h>
static uint16_t rgb565(unsigned r,unsigned g,unsigned b){return (uint16_t)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));}
static int fit(unsigned w,unsigned h,unsigned *dw,unsigned *dh,unsigned *ox,unsigned *oy){
 if(!w||!h||w>SPMP_MM_MAX_PHOTO_SIDE||h>SPMP_MM_MAX_PHOTO_SIDE||
    (uint64_t)w*h>SPMP_MM_MAX_PHOTO_PIXELS)return -1;
 *dw=SPMP_MM_W;*dh=(unsigned)((uint64_t)h*SPMP_MM_W/w);
 if(*dh>SPMP_MM_H){*dh=SPMP_MM_H;*dw=(unsigned)((uint64_t)w*SPMP_MM_H/h);}
 if(!*dw)*dw=1;
 if(!*dh)*dh=1;
 *ox=(SPMP_MM_W-*dw)/2;*oy=(SPMP_MM_H-*dh)/2;
 return 0;
}
static void blitrow(uint16_t *fb,const uint8_t *rgb,unsigned w,unsigned h,
                    unsigned sy,unsigned dw,unsigned dh,unsigned ox,unsigned oy,
                    unsigned ch){
 /* Covers all target rows mapped to this source row (up/downscale). */
 for(unsigned y=0;y<dh;y++)if((unsigned)((uint64_t)y*h/dh)==sy){
  for(unsigned x=0;x<dw;x++){
   unsigned ix=(unsigned)((uint64_t)x*w/dw);const uint8_t *q=rgb+ix*ch;
   fb[(oy+y)*SPMP_MM_W+ox+x]=rgb565(q[0],ch==1?q[0]:q[1],ch==1?q[0]:q[2]);
  }
 }
}
struct jpeg_error_safe {struct jpeg_error_mgr pub;jmp_buf jmp;};
static void jpeg_failed(j_common_ptr arg){struct jpeg_error_safe *e=(struct jpeg_error_safe*)arg->err;longjmp(e->jmp,1);}
static int jpeg_photo(FILE *f,uint16_t *fb){
 struct jpeg_decompress_struct c;struct jpeg_error_safe e;uint8_t * volatile row=NULL;
 volatile int ret=-1;unsigned dw,dh,ox,oy;
 memset(&c,0,sizeof c);c.err=jpeg_std_error(&e.pub);e.pub.error_exit=jpeg_failed;
 if(setjmp(e.jmp)){free(row);jpeg_destroy_decompress(&c);return -1;}
 jpeg_create_decompress(&c);jpeg_stdio_src(&c,f);
 if(jpeg_read_header(&c,TRUE)!=JPEG_HEADER_OK)goto finish;
 c.out_color_space=JCS_RGB;
 if(fit(c.image_width,c.image_height,&dw,&dh,&ox,&oy))goto finish;
 jpeg_start_decompress(&c);
 if(c.output_components!=3||c.output_width>4096)goto finish;
 row=malloc((size_t)c.output_width*3);
 if(!row)goto finish;
 while(c.output_scanline<c.output_height){
  unsigned sy=c.output_scanline;JSAMPROW rows[]={row};
  if(jpeg_read_scanlines(&c,rows,1)!=1)goto finish;
  blitrow(fb,row,c.output_width,c.output_height,sy,dw,dh,ox,oy,3);
 }
 jpeg_finish_decompress(&c);ret=0;
 finish:free(row);jpeg_destroy_decompress(&c);return ret;
}
static void png_err(png_structp png,png_const_charp msg){(void)msg;longjmp(png_jmpbuf(png),1);}
static int png_photo(FILE *f,uint16_t *fb){
 png_structp p=NULL;png_infop info=NULL;uint8_t * volatile row=NULL;
 unsigned dw=0,dh=0,ox=0,oy=0;volatile int ret=-1;
 p=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,png_err,NULL);
 if(!p)return -1;
 info=png_create_info_struct(p);if(!info){png_destroy_read_struct(&p,NULL,NULL);return -1;}
 if(setjmp(png_jmpbuf(p))){free(row);png_destroy_read_struct(&p,&info,NULL);return -1;}
 png_init_io(p,f);png_read_info(p,info);
 unsigned w=png_get_image_width(p,info),h=png_get_image_height(p,info);
 if(fit(w,h,&dw,&dh,&ox,&oy))goto finish;
 /* Indexed/gray/16-bit/alpha images become RGB 8-bit. Progressive interlace
    needs multi-pass buffering, and is intentionally rejected for this stage. */
 if(png_get_interlace_type(p,info)!=PNG_INTERLACE_NONE)goto finish;
 if(png_get_bit_depth(p,info)==16)png_set_strip_16(p);
 if(png_get_color_type(p,info)==PNG_COLOR_TYPE_PALETTE)png_set_palette_to_rgb(p);
 if(png_get_color_type(p,info)==PNG_COLOR_TYPE_GRAY&&png_get_bit_depth(p,info)<8)png_set_expand_gray_1_2_4_to_8(p);
 if(png_get_valid(p,info,PNG_INFO_tRNS))png_set_tRNS_to_alpha(p);
 if(png_get_color_type(p,info)==PNG_COLOR_TYPE_GRAY||png_get_color_type(p,info)==PNG_COLOR_TYPE_GRAY_ALPHA)png_set_gray_to_rgb(p);
 if((png_get_color_type(p,info)&PNG_COLOR_MASK_ALPHA)||png_get_valid(p,info,PNG_INFO_tRNS))png_set_strip_alpha(p);
 png_read_update_info(p,info);
 if(png_get_channels(p,info)!=3||png_get_rowbytes(p,info)>12288)goto finish;
 row=malloc(png_get_rowbytes(p,info));if(!row)goto finish;
 for(unsigned y=0;y<h;y++){
  png_read_row(p,row,NULL);blitrow(fb,row,w,h,y,dw,dh,ox,oy,3);
 }
 png_read_end(p,NULL);ret=0;
 finish:free(row);png_destroy_read_struct(&p,&info,NULL);return ret;
}
static int gif_photo(const char *name,uint16_t *fb){
 int err=0;GifFileType *gif=DGifOpenFileName(name,&err);
 if(!gif)return -1;
 unsigned dw=0,dh=0,ox=0,oy=0;volatile int ret=-1;
 uint8_t *line=NULL,*rgb=NULL;
 if(fit((unsigned)gif->SWidth,(unsigned)gif->SHeight,&dw,&dh,&ox,&oy))goto done;
 GifRecordType type;
 /* Decode first frame only; GIF animation/compositing is future work. */
 while(DGifGetRecordType(gif,&type)==GIF_OK){
  if(type==IMAGE_DESC_RECORD_TYPE){
   if(DGifGetImageDesc(gif)!=GIF_OK)goto done;
   GifImageDesc *d=&gif->Image;
   if(d->Width<=0||d->Height<=0||d->Width>4096||
      d->Top<0||d->Left<0||d->Left+d->Width>gif->SWidth||d->Top+d->Height>gif->SHeight)goto done;
   ColorMapObject *pal=d->ColorMap?d->ColorMap:gif->SColorMap;
   if(!pal)goto done;
   line=malloc((size_t)d->Width);rgb=malloc((size_t)gif->SWidth*3);
   if(!line||!rgb)goto done;
   GifColorType bg={0,0,0};
   int bg_index=gif->SBackGroundColor;
   if(gif->SColorMap&&bg_index>=0&&bg_index<gif->SColorMap->ColorCount)
    bg=gif->SColorMap->Colors[bg_index];
   for(int x=0;x<gif->SWidth;x++){
    rgb[x*3]=bg.Red;rgb[x*3+1]=bg.Green;rgb[x*3+2]=bg.Blue;
   }
   for(int y=0;y<gif->SHeight;y++)
    blitrow(fb,rgb,gif->SWidth,gif->SHeight,(unsigned)y,dw,dh,ox,oy,3);
   /* GIF LZW lines arrive in interlace-pass order: map each directly to its
      canvas row, still using only 1-row memory. */
   static const unsigned start[4]={0,4,2,1},step[4]={8,8,4,2};
   unsigned passes=d->Interlace?4:1;
   for(unsigned pass=0;pass<passes;pass++){
    unsigned y0=d->Interlace?start[pass]:0;
    unsigned dy=d->Interlace?step[pass]:1;
    for(unsigned y=y0;y<(unsigned)d->Height;y+=dy){
     if(DGifGetLine(gif,line,d->Width)!=GIF_OK)goto done;
     for(int x=0;x<gif->SWidth;x++){
      rgb[x*3]=bg.Red;rgb[x*3+1]=bg.Green;rgb[x*3+2]=bg.Blue;
     }
     for(int x=0;x<d->Width;x++){
      int idx=line[x];if(idx>=pal->ColorCount)goto done;
      GifColorType c=pal->Colors[idx];int target=(d->Left+x)*3;
      rgb[target]=c.Red;rgb[target+1]=c.Green;rgb[target+2]=c.Blue;
     }
     blitrow(fb,rgb,gif->SWidth,gif->SHeight,(unsigned)d->Top+y,dw,dh,ox,oy,3);
    }
   }
   ret=0;break;
  } else if(type==EXTENSION_RECORD_TYPE){
   int code;GifByteType *b;
   if(DGifGetExtension(gif,&code,&b)!=GIF_OK)goto done;
   while(b){if(DGifGetExtensionNext(gif,&b)!=GIF_OK)goto done;}
  }else if(type==TERMINATE_RECORD_TYPE)break;
 }
 done:free(rgb);free(line);DGifCloseFile(gif,&err);return ret;
}
int mm_host_photo(const char *filename,mm_type type,uint16_t *fb){
 if(!filename||!fb)return -1;
 for(unsigned i=0;i<SPMP_MM_W*SPMP_MM_H;i++)fb[i]=0;
 if(type==MM_GIF)return gif_photo(filename,fb);
 FILE *f=fopen(filename,"rb");if(!f)return -1;
 int rc=-1;
 if(type==MM_JPEG)rc=jpeg_photo(f,fb);
 else if(type==MM_PNG)rc=png_photo(f,fb);
 fclose(f);return rc;
}

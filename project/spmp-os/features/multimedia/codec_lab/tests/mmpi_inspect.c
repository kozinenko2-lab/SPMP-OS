#include "spmp_media_probe.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char **argv){
 FILE *file;long size;uint8_t *data;spmp_mmpi image;
 if(argc!=2){fprintf(stderr,"usage: %s module.IMG\n",argv[0]);return 2;}
 file=fopen(argv[1],"rb");if(!file){perror("open");return 2;}
 if(fseek(file,0,SEEK_END)!=0 || (size=ftell(file))<52 || size>64*1024*1024 ||
    fseek(file,0,SEEK_SET)!=0){fclose(file);fputs("invalid file size\n",stderr);return 2;}
 data=(uint8_t *)malloc((size_t)size);if(!data){fclose(file);return 2;}
 if(fread(data,1,(size_t)size,file)!=(size_t)size){free(data);fclose(file);return 2;}
 fclose(file);
 if(spmp_mmpi_inspect(data,(size_t)size,&image)!=0){free(data);fputs("invalid MMPi record chain\n",stderr);return 3;}
 free(data);
 printf("bytes=%u descriptors=%u segments=%u payload=%u entry=%s%08x modes=%08x checksums=%08x/%08x\n",
  image.total_size,image.descriptor_count,image.segment_count,image.payload_bytes,
  image.has_entry?"":"none/",image.selected_entry,image.descriptor_mode_mask,
  image.checksum_a,image.checksum_b);
 return 0;
}

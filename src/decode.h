#ifndef DECODE_H
#define DECODE_H

#include "stdio.h"
#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding output file and store message to another file
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _DecodeInfo
{
   /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;

    /* Destination file info */
    char dest_fname[25];
    FILE *fptr_dest_file;

    int magic_str_len;
    char magic_string[100];
    int extn_len;
    char extn[20];
    int file_size;
    char *file_data;

}DecodeInfo;

/* Decoding Function Prototype */

/* Read and Validate Decoding args from argv */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);

/* Perform the Decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File Pointers for I/P and O/P Files */
Status open_file(DecodeInfo *decInfo);

/* Skip the .bmp Header */
Status skip_bmp_header(FILE *fptr_src_image);

/* Decode Magic String Length */
Status decode_magic_string_len(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(int length, DecodeInfo *decInfo);

/* Decode Secret File Extension Size */
Status decode_file_extn_size(DecodeInfo *decInfo);

/* Decode Secret File Extension */
Status decode_file_extn(int length, DecodeInfo *decInfo);

/* Open Destination File */
Status open_dest_file(DecodeInfo *decInfo);

/* Decode File Size */
Status decode_file_size(DecodeInfo *decInfo);

/* Decode File Data */
Status decode_file_data(int length, DecodeInfo *decInfo);

/* Decode LSB of Image Data Array to Byte */
char lsb_to_byte(char *image_buffer);

/* Decode LSB of Image Data Array to int Data */
int lsb_to_size(char *image_buffer);

#endif

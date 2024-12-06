#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "string.h"
#include "stdlib.h"
#include "common.h"

 
/* Get File pointers for i/p and o/p file
 * Inputs: Src Image file
 * Output: FILE pointer
 * Return Value: e_success or e_failure, on file errors
 */
Status open_file(DecodeInfo *decInfo)
{
    //Src Image File
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");
    //Do Error handling
    if(decInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr,"ERROR: Unable to open file %s\n",decInfo->src_image_fname);
        return e_failure;
    }
    return e_success;
}

/* Validate the command line arguments
 * Input : Command Line vector and count, structure
 * Output: Stores the file names in struct members
 * Description: Checks the command line arguments if true then store it in struct members
 */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    if(argc >= 3 && argc <= 4)
    {
        //validate all arguments
        if(strstr(argv[2], ".bmp"))//third argument
        {
            decInfo->src_image_fname = argv[2];
            printf("\n\033[1mINFO: Checked %s \033[0m\n",decInfo->src_image_fname);
            if(argv[3] == NULL)//fourth argument, if present or not
            {
                strcpy(decInfo->dest_fname, "output");
                nibble.flag3 = 1;
                
                return e_success;
            }
            else 
            {
                strcpy(decInfo->dest_fname, argv[3]);
                printf("\n\033[1mINFO: Checked %s\033[0m\n",decInfo->dest_fname);
                return e_success;
            }
        }
        else
        {
            printf("\n\033[1mINFO: Checked %s Incorrect File\033[0m\n",argv[2]);
            goto exit;
        }
    }
    else goto exit;
    exit:
    return e_failure;
}

/* Decode data from.bmp fle
 * Input : Structure contains all file related members
 * Output: Returns status if decoding is completed or not
 * Description: decode each data based on the structure,.bmp and store only the secret data into another file
 */
Status do_decoding(DecodeInfo *decInfo)
{
    //Open Source file
    nibble.flag2 = open_file(decInfo);
    if(!nibble.flag2)   return e_failure;

    //Skipping header contents from .bmp file
    printf("\n\033[1mINFO: Skipping Image Header\033[0m\n");
    nibble.flag2 = skip_bmp_header(decInfo->fptr_src_image);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: Image Header notskipped\033[0m\n");      return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");

    //Decoding Magic String Length
    printf("\n\033[1mINFO: Decoding Magic String Length\033[0m\n");
    nibble.flag2 = decode_magic_string_len(decInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: Magic String Length not decoded\033[0m\n");      return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");

    //Decoding Magic String 
    printf("\n\033[1mINFO: Decoding Magic String\033[0m\n");
    nibble.flag2 = decode_magic_string(decInfo->magic_str_len,decInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: Magic String not decoded\033[0m\n");     return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");

    //Comapre Magic Strings
    char *temp = malloc(100 * sizeof(char));
    printf("\n\033[1mPlease Enter the Magic String : \033[0m");
    scanf("%[^\n]",temp);
    if((!(strcmp(decInfo->magic_string, temp))) == 0)
    {
        printf("\n\033[1;37;41mINFO: Magic String not matched\033[0m\n");     return e_failure;
    }
    else    printf("\n\033[1mINFO: Magic String matched\033[0m\n");

    free(temp);
    //Decoding File Extension Size
    printf("\n\033[1mINFO: Decoding Output File Extension Length\033[0m\n");
    nibble.flag2 = decode_file_extn_size(decInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: Output File Extension Length not decoded\033[0m\n");    return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");

    //Decoding File Extension
    printf("\n\033[1mINFO: Decoding Output File Extension\033[0m\n");
    nibble.flag2 = decode_file_extn(decInfo->extn_len, decInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: Output File Extension not decoded\033[0m\n");   return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");

    //Opening Destination File
    printf("\n\033[1mINFO: Opening All Required File\033[0m\n");
    nibble.flag2 = open_dest_file(decInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: Files not opened\033[0m\n");      return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");

    //Decoding File Size
    printf("\n\033[1mINFO: Decoding %s File Size\033[0m\n",decInfo->dest_fname);
    nibble.flag2 = decode_file_size(decInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: %s File Size not decoded\033[0m\n",decInfo->dest_fname);     return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");

    //Decoding File Data
    printf("\n\033[1mINFO: Decoding %s Secret Data\033[0m\n",decInfo->dest_fname);
    nibble.flag2 = decode_file_data(decInfo->file_size, decInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: %s Secret Data not decoded\033[0m\n",decInfo->dest_fname);      return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");
    return e_success;
}

/* Skipping image file header
 * Input : file poniter of source .bmp file
 * Output: returns function status based on offset position on .bmp file
 * Description: skips the first 54 bytes(Header properties) from output.bmp file
 */
Status skip_bmp_header(FILE *fptr_src_image)
{
    //Skip 54 bytes
    fseek(fptr_src_image ,54 ,SEEK_SET);

    uint position1 = ftell(fptr_src_image);

    return (!(position1 ^ 54)) ? e_success : e_failure;
}

/* Decoding magic string length
 * Input : structure members
 * Output: returns function status based on offset positions on .bmp file
 * Description: Read next 32 bytes from original .bmp file and decode the magic string length 
 * and store it in structure member
 */
Status decode_magic_string_len(DecodeInfo *decInfo)
{
    char buffer[32];
    decInfo->magic_str_len = 0;

    fread(buffer, 32, 1, decInfo->fptr_src_image);

    decInfo->magic_str_len  = lsb_to_size(buffer);

    return !(decInfo->magic_str_len) ? e_failure : e_success;
}

/* Decoding magic string 
 * Input : magic string length, structure members
 * Output: returns function status based on magic_string member on structure
 * Description: Read next n bytes(based on magic string length) from original .bmp file and decode the magic string
 * and store it in  structure member
 */
Status decode_magic_string(int length, DecodeInfo *decInfo)
{
    char buffer[8], data;
    for(int i = 0 ; i < length; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_src_image);

        data = lsb_to_byte(buffer);

        decInfo->magic_string[i] = data;
    }
    decInfo->magic_string[length] = '\0';

    return (decInfo->magic_string[0] != '\0' && length != 0) ? e_success : e_failure;
}

/* Decoding File extension length(secert.txt)
 * Input : structure members
 * Output: returns function status based on file extn size struture member
 * Description: Read next 32 bytes from original .bmp file and decode the file extension length 
 * and store it in structure member
 */
Status decode_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    decInfo->extn_len = 0;

    fread(buffer, 32, 1, decInfo->fptr_src_image);

    decInfo->extn_len  = lsb_to_size(buffer);

    return !(decInfo->extn_len) ? e_failure : e_success;
}

/* Decoding File Extension
 * Input : magic string length, structure members
 * Output: returns function status based on file extension member on structure
 * Description: Read next n bytes(based on file extension length) from original .bmp file and decode the file extension
 * and store it in  structure member
 */
Status decode_file_extn(int length, DecodeInfo *decInfo)
{
    char buffer[8], data;
    for(int i = 0 ; i < length; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_src_image);

        data = lsb_to_byte(buffer);

        decInfo->extn[i] = data;
    }
    decInfo->extn[length] = '\0';
    return (decInfo->extn[0] != '\0' && length != 0) ? e_success : e_failure;
}

/* Open output file 
 * Inputs: Structure members
 * Output: FILE pointer
 * Return Value: e_success
 */
Status open_dest_file(DecodeInfo *decInfo)
{
    if(nibble.flag3)    printf("\n\033[1mINFO: Output File not mentioned. Creating output as default\033[0m\n");
    if(strchr(decInfo->dest_fname, '.'))    goto open_file;

    else    strcat(decInfo->dest_fname, decInfo->extn);

    open_file:
    decInfo->fptr_dest_file = fopen(decInfo->dest_fname, "w");
    return e_success;           
}

/* Decoding File Size(secert.txt)
 * Input : structure members
 * Output: returns function status based on file size struture member
 * Description: Read next 32 bytes from original .bmp file and decode the file size
 * and store it in structure member
 */
Status decode_file_size(DecodeInfo *decInfo)
{
    char buffer[32];    decInfo->file_size = 0;

    fread(buffer, 32, 1, decInfo->fptr_src_image);

    decInfo->file_size  = lsb_to_size(buffer);

    return !(decInfo->file_size) ? e_failure : e_success;
}

/* Decoding File Data
 * Input : File size, structure members
 * Output: returns function status based on file data member on structure
 * Description: Read next n bytes(based on file data length) from original .bmp file and decode the file data
 * and store it in a output file(output.txt)
 */
Status decode_file_data(int length, DecodeInfo *decInfo)
{
    char buffer[8];
    char data = 0;
    decInfo->file_data = malloc(length * sizeof(char));
    for (int i = 0; i < length; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_src_image);

        data = lsb_to_byte(buffer);

        decInfo->file_data[i] = data; 
    }
    decInfo->file_data[length] = '\0';
    fprintf(decInfo->fptr_dest_file,"%s",decInfo->file_data);
    free(decInfo->file_data);
    return e_success;
}

/* Decoding data from image_buffer
 * Input : data(character) and image_buffer(i.e had bytes from original .bmp file) 
 * Output: returns char data
 * Description: Decode the char data from the image buffer elements
 */
char lsb_to_byte(char *image_buffer)
{
    char data = 0;
    for(int i = 7, ind = 0; i >= 0; i--, ind++)
    {
        int ret = image_buffer[i] & 1;
        
        data = data | (ret << ind);
    }
    return data;
}

/* Decoding data from image_buffer
 * Input : data(int) and image_buffer(i.e had bytes from original .bmp file) 
 * Output: returns int data
 * Description: Decode the int data from the image buffer elements
 */
int lsb_to_size(char *image_buffer)
{
    int data = 0;
    for(int i = 31, ind = 0; i >= 0; i--, ind++)
    {
        int ret = image_buffer[i] & 1;
        
        data = data | (ret << ind);
    }
    return data;
}
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "string.h"
#include "stdlib.h"
#include "common.h"
/* Function Definitions */

/* Get image size
 * Input : Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/* Check Operation type
 * Input : Command line vector
 * Output: returns operation type which is encoding or decoding
 * Description: Checks the 2nd argument from command line and returns the suitable operation type
 */
OperationType check_operation_type(char *argv[])
{
    if(!(strcmp(argv[1], "-e")))    return e_encode;

    else if(!(strcmp(argv[1], "-d")))    return e_decode;

    else    return e_unsupported;
}

/* Validate the command line arguments
 * Input : Command Line vector and count, structure
 * Output: Stores the file names in struct members
 * Description: Checks the command line arguments if true then store it in struct members
 */
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
    if(argc >= 4 && argc <= 5)
    {
        //validate all arguments
        if(strstr(argv[2], ".bmp"))//third argument
        {
            encInfo->src_image_fname = argv[2];
            printf("\n\033[1mINFO: Checked %s\033[0m\n",encInfo->src_image_fname);
           
            if((strstr(argv[3],".c")) || (strstr(argv[3],".txt")) || (strstr(argv[3],".sh")))//Fourth argument
            {
                encInfo->secret_fname = argv[3];
                printf("\n\033[1mINFO: Checked %s\033[0m\n",encInfo->secret_fname);
                //Assigning secret file's extension to a struct member
                (strstr(argv[3],".c")) ? strcpy(encInfo->extn_secret_file, ".c") : (strstr(argv[3],".txt")) ? strcpy(encInfo->extn_secret_file, ".txt") : (strstr(argv[3],".sh")) ? strcpy(encInfo->extn_secret_file, ".sh") : strcpy(encInfo->extn_secret_file, "Invalid Extension");

                if(argv[4] == NULL)
                {
                    encInfo->stego_image_fname = "stego_image.bmp";
                    
                    printf("\n\033[1mINFO: Checked %s\033[0m\n",encInfo->stego_image_fname);
                    
                    return e_success;
                }
                else 
                {
                    encInfo->stego_image_fname = argv[4];
                    if(strstr(argv[4], ".bmp"))
                    {
                        printf("\n\033[1mINFO: Checked %s\033[0m\n",encInfo->stego_image_fname);
                        return e_success;
                    }
                    else 
                    {
                        nibble.flag3 = 1;
                        printf("\n\033[1mINFO: Checked %s !!!Invalid File!!!\033[0m\n",argv[4]);
                        goto exit;
                    }
                }
            }
            else 
            {
                printf("\n\033[1mINFO: Checked %s !!!Invalid File!!!\033[0m\n",argv[3]);
                goto exit;
            }
        }
        else
        {
           printf("\n\033[1mINFO: Checked %s !!!Invalid File!!!\033[0m\n",argv[2]);
            goto exit;
        } 
    }
    else goto exit;
    exit:
    return e_failure;
}

/* Check offset position
 * Input : source and destination file pointers
 * Output: Returns status based on offset positions
 * Description: In every encoding will return status based on this function 
 */
Status chk_offset_positions(FILE *fptr_src, FILE *fptr_dest)
{
    uint position1 = ftell(fptr_src);
    uint position2 = ftell(fptr_dest);
    return (!(position1 ^ position2)) ? e_success : e_failure;
}

/* Encoding data to .bmp fle
 * Input : Structure contains all file related members
 * Output: Returns status if encoding is completed or not
 * Description: Encode each data based on the structure,.bmp and secret file into the new .bmp file
 */
Status do_encoding(EncodeInfo *encInfo)
{
    //Open files
    nibble.flag2 = open_files(encInfo);
    if(!nibble.flag2)   return e_failure;

    //READ MAGIC STRING FROM USER
    printf("\n\033[1mPlease Enter the Magic String : \033[0m");
    scanf("%[^\n]",encInfo->magic_string);

    //check for secret.txt size
    printf("\n\033[1mINFO: Checking for %s size\033[0m\n",encInfo->secret_fname);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if(encInfo->size_secret_file != 0)      printf("\n\033[1mINFO: Done. Not Empty \033[0m\n");
    
    else    
    {
        printf("\n\033[1mINFO: Done. %s is Empty \033[0m\n",encInfo->secret_fname);     return e_failure;
    }

    //check capacity
    printf("\n\033[1mINFO: Checking for %s capacity to handle %s\033[0m\n",encInfo->src_image_fname,encInfo->secret_fname);
    nibble.flag2 =  check_capacity(encInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: %s has low capacity to handle further operations\033[0m\n",encInfo->src_image_fname);    return e_failure;
    }
    printf("\n\033[1mINFO: Done. Found OK\033[0m\n");

    //Print statement if user didn't mention output .bmp file on command line   

    //Copying header contents from Original.bmp file to Output .bmp file
    printf("\n\033[1mINFO: Copying Image Header\033[0m\n");
    nibble.flag2 = copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: Image Header not copied\033[0m\n");      return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");

    //Encoding Magic String Length
    printf("\n\033[1mINFO: Encoding Magic String Length\033[0m\n");
    nibble.flag2 = encode_magic_string_len(strlen(encInfo->magic_string), encInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: Magic String Length not encoded\033[0m\n");      return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");

    //Encoding Magic String 
    printf("\n\033[1mINFO: Encoding Magic String Signature\033[0m\n");
    nibble.flag2 = encode_magic_string(encInfo->magic_string, encInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: Magic String Signature not encoded\033[0m\n");   return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");

    //Encoding Secret File Extension Size
    printf("\n\033[1mINFO: Encoding %s File Extension Size\033[0m\n",encInfo->secret_fname);
    nibble.flag2 = encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: %s File Extension Size not encoded\033[0m\n",encInfo->secret_fname);     return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");
   //Encoding Secret.txt File Extension 
    printf("\n\033[1mINFO: Encoding %s File Extension\033[0m\n",encInfo->secret_fname);
    nibble.flag2 = encode_secret_file_extn(encInfo->extn_secret_file,encInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: %s File Extension not encoded\033[0m\n",encInfo->secret_fname);  return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");
    
    //Encoding Secret.txt File Size
    printf("\n\033[1mINFO: Encoding %s File Size\033[0m\n",encInfo->secret_fname);
    nibble.flag2 = encode_secret_file_size(encInfo->size_secret_file,encInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: %s File Size not encoded\033[0m\n",encInfo->secret_fname);   return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");
    
    //Encoding Secret.txt File Data
    printf("\n\033[1mINFO: Encoding %s File Data\033[0m\n",encInfo->secret_fname);
    nibble.flag2 = encode_secret_file_data(encInfo);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: %s File Data not encoded\033[0m\n",encInfo->secret_fname);   return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");

    //Coyping Left Over Data
    printf("\n\033[1mINFO: Copying Left Over Data\033[0m\n");
    nibble.flag2 =  copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if(!nibble.flag2)   
    {
        printf("\n\033[1mINFO: Left Over Data Not Copied\033[0m\n");    return e_failure;
    }
    printf("\n\033[1mINFO: Done\033[0m\n");

    //Closing all files
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_stego_image);
    fclose(encInfo->fptr_secret);
    return e_success;
}

/* Checking capacity for data encode
 * Input : structure contains all file related variables
 * Output: returns function status based on capacities
 * Description: Checks the capacity of original .bmp file and encoding data total size
 */
Status check_capacity(EncodeInfo *encInfo)
{
    //Header size on BMP file
    int header_bytes_bmp = 54;

    //Getting size of the .bmp file
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    //Getting size of the secret file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    
    uint secret_capacity = (header_bytes_bmp + (((strlen(MAGIC_STRING)) + (sizeof(int)) + (strlen(encInfo->extn_secret_file)) + (sizeof(int)) + encInfo->size_secret_file) * 8));

    return (encInfo->image_capacity > secret_capacity) ? e_success : e_failure;
}

/* get file size
 * Input : file pointer
 * Output: EoF position in afile
 * Description: Calculating the file size by using fseek function
 */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    uint position = (ftell(fptr));
    rewind(fptr);
    return position;
}

/* Copying image file header
 * Input : file poniters of original and output .bmp files
 * Output: returns function status based on offset positions on two files
 * Description: Copies the first 54 bytes(Header properties) from original .bmp file to output.bmp file
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image); char temp[54];

    fread(temp, 54, 1, fptr_src_image);

    fwrite(temp, 54, 1, fptr_dest_image);  

    return chk_offset_positions(fptr_src_image, fptr_dest_image);
}

/* Encoding magic string length
 * Input : magic string length, structure members
 * Output: returns function status based on offset positions on two files
 * Description: Read next 32 bytes from original .bmp file and encode the with magic string length 
 * and write into output.bmp file
 */
Status encode_magic_string_len(int length, EncodeInfo *encInfo)
{
    fread(encInfo->image_data, 32, 1, encInfo->fptr_src_image);

    nibble.flag2 = encode_size_to_lsb(length, encInfo->image_data);

    fwrite(encInfo->image_data, 32, 1, encInfo->fptr_stego_image);

    return chk_offset_positions(encInfo->fptr_src_image, encInfo->fptr_stego_image);
}

/* Encoding magic string 
 * Input : magic string, structure members
 * Output: returns function status as success
 * Description: Read next n bytes(based on magic string length) from original .bmp file and encode the with magic string
 * and write into output.bmp file
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    nibble.flag2 =  encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);

    return !(nibble.flag2) ? e_failure : e_success;
}

/* Encoding File extension length(secert.txt)
 * Input : File extension length, structure members
 * Output: returns function status based on offset positions on two files
 * Description: Read next 32 bytes from original .bmp file and encode the with file extension length 
 * and write into output.bmp file
 */
Status encode_secret_file_extn_size(int length, EncodeInfo *encInfo)
{
    fread(encInfo->image_data, 32, 1, encInfo->fptr_src_image);

    nibble.flag2 = encode_size_to_lsb(length, encInfo->image_data);

    fwrite(encInfo->image_data, 32, 1, encInfo->fptr_stego_image);

    return chk_offset_positions(encInfo->fptr_src_image, encInfo->fptr_stego_image);
}

/* Encoding File extension(secert.txt)
 * Input : File extension, structure members
 * Output: returns function status as success
 * Description: Read next n bytes(based on file extension length) from original .bmp file and encode the with file extension length 
 * and write into output.bmp file
 */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    nibble.flag2 =  encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image);

    return !(nibble.flag2) ? e_failure : e_success;
}

/* Encoding Secret file length(secert.txt)
 * Input : Secret file length, structure members
 * Output: returns function status based on offset positions on two files
 * Description: Read next 32 bytes from original .bmp file and encode the with secret file length 
 * and write into output.bmp file
 */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    fread(encInfo->image_data, 32, 1, encInfo->fptr_src_image);

    nibble.flag2 = encode_size_to_lsb(file_size, encInfo->image_data);

    fwrite(encInfo->image_data, 32, 1, encInfo->fptr_stego_image);

    return chk_offset_positions(encInfo->fptr_src_image, encInfo->fptr_stego_image);
}

/* Encoding Secrret file data
 * Input : structure members
 * Output: returns function status based on offset positions on two files
 * Description: Read next n bytes(based on secret file length) from original .bmp file and encode the with secret file data
 * and write into output.bmp file
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    int i=encInfo->size_secret_file;    char arr[i];

    fread(arr,i,1,encInfo->fptr_secret);

    encode_data_to_image(arr, i, encInfo->fptr_src_image, encInfo->fptr_stego_image);

    return chk_offset_positions(encInfo->fptr_src_image, encInfo->fptr_stego_image);
}

/* Copying remaining image file data 
 * Input : file poniters of original and output .bmp files
 * Output: returns function status based on offset positions on two files
 * Description: Copies the remaining bytes(Upto EOF) from original .bmp file to output.bmp file
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer[1];
    // Read and write one character at a time
    while (fread(buffer, 1, 1, fptr_src) > 0)
    {
        fwrite(buffer, 1, 1, fptr_dest);
    }
    return chk_offset_positions(fptr_src, fptr_dest);
}

/* Encoding data into image_buffer
 * Input : data(character) and image_buffer(i.e had bytes from original .bmp file) 
 * Output: returns function status as success
 * Description: Encode the char data into the image buffer elements
 */
Status encode_byte_to_lsb(char data, char* image_buffer)
{
    int j = 0;
    for(int i = 7, ind = 0; i >= 0; i--, ind++)
    {
        //get a bit from MSB and store it
        int res = data & (1 << i);
        //Move that bit LSb of the result
        res = res >> i;

        //Clear a LSB bit from image_buffer element
        image_buffer[ind] = image_buffer[ind] & (~(1 << 0));
        //Add res and image_buffer element
        image_buffer[ind] = res | image_buffer[ind]; 
    }
    return e_success;
}

/* Encoding data into image_buffer
 * Input : data(integer) and image_buffer(i.e had bytes from original .bmp file) 
 * Output: returns function status as success
 * Description: Encode the int data into the image buffer elements
 */
Status encode_size_to_lsb(int data, char* image_buffer)
{
    for(int i = 31, ind = 0; i >= 0; i--, ind++)
    {
        //get a bit from MSB and store it
        int res = data & (1 << i);
        //Move that bit LSb of the result
        res = res >> i;

        //Clear a LSB bit from image_buffer element
        image_buffer[ind] = image_buffer[ind] & (~(1 << 0));
        //Add res and image_buffer element
        image_buffer[ind] = res | image_buffer[ind]; 
    }
    return e_success;
}

/* Encoding data into output image
 * Input : data(string), size of that data, file pointers of original and output .bmp files
 * Output: returns function status baswd on offset position on two files
 * Description: writes the encoded data into output.bmp file
 */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    for(int  i = 0; i < size; i++)
    {
        fread(buffer, 8, 1, fptr_src_image);

        encode_byte_to_lsb(data[i], buffer);

        fwrite(buffer, 8, 1, fptr_stego_image);
    }
    return chk_offset_positions(fptr_src_image, fptr_stego_image);
}
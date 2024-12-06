#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "string.h"
#include "stdlib.h"
#include "decode.h"
void error_msg(int err)
{
    if(err == 0)//unsupported
    {
        printf("\n\033[1;37;43m./a.out: Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\033[0m\n");
        printf("\n\033[1;37;43m./a.out: Decoding: ./a.out -d <.bmp file> [output file]\033[0m\n");
    }
    if(err == 1)//encode
        printf("\n\033[1;37;43m./a.out: Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\033[0m\n");
    if(err == 2)//decode
        printf("\n\033[137;43m./a.out: Decoding: ./a.out -d <.bmp file> [output file]\033[0m\n");

}

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;//allocating memory
    
    DecodeInfo decInfo;//allocating memory

    if(argv[1] == NULL)
    {
        error_msg(0);
        return 0;
    }
    
    //Validate command line arguments
    nibble.flag1 = check_operation_type(argv);

    if(!(nibble.flag1 ^ 2)) //Unsupported
    {
        error_msg(0);
        return 0;
    }
    else if(nibble.flag1 ^ 0)
    {
        printf("\n\033[1mINFO: Checking required files\033[0m\n");
        //Validate command line arguments
        nibble.flag2 = read_and_validate_decode_args(argc, argv, &decInfo);
        if(nibble.flag2)    
        {//Decode
            printf("\n\033[1mINFO: DONE\033[0m\n");
            printf("\n\033[1;37;44m##Decoding procedure started##\033[0m\n");
            nibble.flag2 = do_decoding(&decInfo);
            if(!nibble.flag2)   error_msg(2);
            else    printf("\n\033[1;37;42m## Decoding Completed ##\033[0m\n");
        }

        else error_msg(2);
    }
    else if(nibble.flag1 ^ 1)
    {
        if(argv[4] == NULL)
        {
            printf("\n\033[1mINFO: Output File not mentioned. Creating stego_image.bmp as default\033[0m\n");
        }
        printf("\n\033[1mINFO: Checking required files\033[0m\n");
        //Validate command line arguments
        nibble.flag2 = read_and_validate_encode_args(argc, argv, &encInfo);
        
        if(nibble.flag2)    
        {//Encode
            printf("\n\033[1mINFO: DONE\033[0m\n");
            printf("\n\033[1;37;44m## Encoding procedure started ##\033[0m\n");
            nibble.flag2 = do_encoding(&encInfo);
            if(!nibble.flag2)   error_msg(1);
            else    printf("\n\033[1;37;42m## Encoding Completed ##\033[0m\n");
        }
        else error_msg(1);
    }
    return 0;
}

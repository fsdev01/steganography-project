/**
   Created by vahuy on 26/4/17.
   readHeader.c contains a single function called 
   "read_header" the reads the header components of a PPM File.
   It is used by hide.c, unhide.c, display.c files.
**/

#include "readHeader.h"

int *read_header(FILE *fp)
{
    /**
     This function reads in the header components of a PPM File and
     returns an array pointer containing the dimension and bandwidth value.
     It also checks whether the magic number is "P6".
     @input: '*fp' is a file pointer to a valid ppm file (image)
     @output: none
     @return: 'dimChan[]' is pointer. dimChan[0] = width  dimChan[1] = height dimChan[2] = max channel value.
     @precondition: (1) assumes the file PPM has the correct header components in the correct order.
     (2) the filepointer is positioned at the start of the ppm file.
     
     THERE ARE THREE(3) HEADER COMPONENTS:
     (1) Magic Number
     (2) Dimension in pixels: width,height
     (3) maximum bandwidth value
     **/
    
    
    
    int headerCount = 0; //No. of header components that have been read in.
    char magicNumber[5]; //Store the Magic Number
    int dim[2]; //Store the dimension pixels: width height
    int maxChan; //Store the max channel value
    
    char aChar; // Store the character obtained from fgetc();
    
    char tempComments[100]; // Store the comments
    
    
    //Loop until all 3 header components have been extracted.
    while(headerCount < 3)
    {
        
        
        aChar = fgetc(fp); //Extract the first character of the n-th(1st/2nd/...) line
        
        
        //Condition 1: Skip over comments
        if(aChar=='#')
        {
            
            while(aChar!='\n')
            {
                aChar = fgetc(fp);
                
            }
            
        }
        //Condition 2: Read in the header component.
        else
        {
            //Move the File Pointer 1 character back.
            fseek(fp,-sizeof(aChar),SEEK_CUR);
            
            
            // Read in 1st header component: Magic Number
            if(headerCount == 0)
            {
                fscanf(fp,"%s\n",magicNumber);
                //printf("Magic Number:%s\n",magicNumber);
                
                // strcmp returns '0' if the "magic Number" is equal to P6.
                int validMagicNumber = strcmp(magicNumber,"P6");
                if(validMagicNumber!=0)
                {
                    fprintf(stderr,"The file is not in P6 Format: %s\n",magicNumber);
                    exit(-1);
                }
                
            }
            // Read in 2nd header component: Dimension: width height
            if(headerCount == 1)
            {
                fscanf(fp,"%d %d\n",&dim[0],&dim[1]);
                //printf("Dimension:%d %d\n",dim[0],dim[1]);
                
            }
            // Read in 3rd header component: Maximum value for the channel
            if(headerCount == 2)
            {
                fscanf(fp,"%d\n",&maxChan);
                //printf("Channel max:%d\n",maxChan);
            }
            
            ++headerCount;
            
        }
        
    }
    
    
    //REMEMBER TO FREE THE ALLOCATED DYNAMIC MEMORY
    int *dimChan = (int *)malloc(sizeof(int)*3);
    dimChan[0] = dim[0]; // width dimension
    dimChan[1] = dim[1]; // height dimension
    dimChan[2] = maxChan; // max. channel value
    
    return dimChan;
    
}


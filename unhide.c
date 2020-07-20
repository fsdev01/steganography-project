/*
 * unhide.c
 * Created by vahuy on 22/3/17.
 * This file aims support the unhide() functionality such that
 * it unhides the secret message contained within a given ppm image.
 * main() is the driver, where it handles the different modes given:
 *  (1) ./unhide image.ppm (unhide a single msg in a single image)
    (2) ./unhide -m output-basename (unhide a single msg using multiple files)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "readHeader.h"
#include "other.h"




int decode(char *fileName,int mode)
{
    
    
    
    /**
     This function decodes the secret message (string) stored in the ppm file. 
     This function checks whether the file has a message using '\0'.

     @input: 'fileName' name of file that should contain a message.
             'mode' if mode=1, display contents of file    if mode == 0, don't display contents of file
     @output: prints out the secret message if one exists.
     @return:  'endOfMessageStatus' is an Integer. A value of "1" means the file contains a \0. A value of "0" if it does not contain "\0".
     @preconditions:  (1) message is in ASCII format.
     **/
    
    FILE *fp = fopen(fileName,"rb");// Open the File
    //Check whether the input file was open sucessfully
    if(fp == NULL)              {
        fprintf(stderr,"ERROR: The file %s cannot be read\n",fileName);
        exit(-1);
    }
    
    
    // Skip Over Header Contents
    read_header(fp);

    

    //VARIABLES FOR SECTION 1(as below):
    int readBlockSucess = 1;
    char eightColorPixels[8]; // Store 8 COLOUR pixels = 1 character - This array is NOT USED AS A STRING.
    char c;
    /* Extra Info for eightColorPixels:
       1 character consumes 8 bits.
       Each Pixel has Red Color, Green Color, Blue Color.
       Use the least significant of each color to store 1 of the 8 bits of the character.
    */
    
    
    
    //VARAIBLES FOR SECTION 2 and 3(as below):
    int j; // Loop counter
    int binaryValue; // Stores the values: 1 or 0.
    char eightBits[9]; // Store the "least significant bits" collected as a string of 1's and 0's.
    int endOfMessageStatus = 0; // Status of "End of Message" - when character is "\0"
    char aByte; // Store single ASCII character to print out
    

    // Loop until (1) reach end of file or (2) message has been read.
    while(  (!feof(fp)) & (endOfMessageStatus==0))
    {
        
        /*----------SECTION 1: EXTRACT BLOCK of 8 COLOR PIXELS. 8 Color pixels = 1 character-------------*/
        for(j=0; j< 8 ; ++j)
        {
            char c = fgetc(fp);  //Get the next character
            //printf(" 0x%02x\n", (unsigned char)c);

            //Check whether EOF Condition has been reached. - Incomplete Message/ Message does not exist
            if(feof(fp)==1)
            {
                readBlockSucess = 0;
            }
            
            eightColorPixels[j] = c;
        }

        
        // If  a block of 8 color pixels cannot be extracted, then set readBlockSuceess = 0
        // and terminate the loop (as there are no more characters to decode/read)
        if(readBlockSucess==0)
        {
            break;
        }
        
        
        /*----------------SECTION 2: CONVERT THE 8 COLOR PIXEL BLOCK INTO A CHARACTER.---------------*/
        //GATHER THE least significant bits and store it into an array.
        for(j=0;j<8;++j)
        {
            //Get the LEAST significant bit of a character(8bit/1byte)
            char v = eightColorPixels[j]& 1; // Mask it by 11111110
            
            // Get the Hexideciaml (Binary 0 or 1) convert to decimal
            binaryValue = (int)v;
            
            //Put the least significant bit into a string.
            if(binaryValue == 1)
            {
                //printf("Binary representation in base 10 Value: 1\n");
                eightBits[j] = '1';
                
            }
            else // binaryValue == 0;
            {
                //printf("Binary representation in base 10  Value: 0\n");
                eightBits[j] = '0';
            }
        }
        eightBits[8] = '\0';

        /*------------------ SECTION 3: DECODE THE GIVEN BYTE / CONVERT 8 BITS to a SINGLE CHARACTER  ------------------- */
        // CONVERT 8-BIT STRING ARRAY  TO ASCII CHARACTER
        aByte = strtol(eightBits, NULL, 2);
        
        //Print out the character
        if(aByte == '\0')
        {
            endOfMessageStatus = 1;
        }
        else  //(aByte is a ASCII character)
        {
            if(mode==1)
            {
                printf("%c",aByte);
            }


        }
        
        
    
    }
    
    fclose(fp); // Close the File
    return endOfMessageStatus;
}


int main(int argc,char *argv[])
{
    /* The main() function is used to handle different modes
     * available for the hide function.
     * There are two posible modes:
     * (1) ./hide output.ppm (no flags) - unhides a message contained in a single image
     * (2) ./hide -m basename (-m flag) - unhides a message contained in several images
     */
    
    // Main function in Assignment 1
    if(argc==2) // No flag given to unhide()
    {
        char *fileName =argv[1];  // argv[1] refers to filename of the input file.
        if(decode(fileName,0)==0) // Check whether the file contains '\0'
        {
            fprintf(stderr,"ERROR: The file '%s'does not have any message\n",fileName);
            exit(-1);
        }
        printf("Secret Message Displayed Below:\n");
        decode(fileName,1);
        printf("\n"); //End of Message.

        
    }
    else if(argc==3) // Task 1 in Assignment 2
    {
        char *outputFileName = argv[2]; // Extract the basename of the file.
        
        if(strcmp(argv[1],"-m")!=0) // Check for an Invalid Flag
        {
            fprintf(stderr,"ERROR:Invalid Flag\n");
            fprintf(stderr,"ERROR:Expected ./unhide -m baseName\n");
            exit(-1);
        }
        

        
        
        
        // Count the number of files with the output-base-name
        int noOfFiles = countValidFiles(outputFileName);
        if(noOfFiles == 0)
        {
            fprintf(stderr,"ERROR: There are zero files with the basename '%s'\n",outputFileName);
            exit(-1);
        }
        //printf("Secret Message Displayed Below:\n");
        // Naming Convention of Files.
        int lengthOutputBase = strlen(outputFileName); // Number characters in output-base-name (excluding \0)
        // size of newBaseOutputName is equal to: "lengthOutputBase" characters + 1 char "-" + 3 digits + 4 chars ".ppm"+  1 Null character
        char newBaseOutputName[lengthOutputBase+9];
        

        
        
        
        // Decode Each File.
        int i; // Loop Purposes
        for(i=0; i< noOfFiles; i++)
        {
            //File-Naming Convention
            sprintf(newBaseOutputName,"%s-%03d.ppm",outputFileName,i); // Generate string of the filename of the image using the base-name.
            //printf("Output File: %s\n",newBaseOutputName);
            
            //ERROR CHECK:Check whether the file contains '\0'
            if(decode(newBaseOutputName,0)==0)
            {
                fprintf(stderr,"ERROR: The file '%s'does not have any message\n",newBaseOutputName);
                exit(-1);
            }
            decode(newBaseOutputName,1); // Decode the Message for the given Output File
            
            
            
        }
        printf("\n"); //End of Message.

        

        
        
        
    }
    else
    {
        fprintf(stderr,"Error: Supplied invalid arguments OR incorrect # of arguments\n");
        exit(-1);
    }

    
    
}

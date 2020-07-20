/* hide.c 
 * last modified: 5/5/17
 * This file aims to provide hide() function that hides
 * a secret message in one or more ppm images.
 * The main() function is the driver of the hide() functionality.
 * main() handles the different possible flags including:
 * (1) no flags - encode a message into a single ppm image by calling modeZero() function.
 * (2) "-m" flag - encode a long message into multiple ppm files by calling modeOne() function.
 * (3) "-p" flag - parallel execution - encode multiple messages at the same time.
 * (4) "-s" flag - display before/after image after encoding message, by calling modeOne() and displayImage().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "readHeader.h"
#include "other.h"
#include "display.h"

int checkPPM(char *fileName)
{
    /** 
     Check whether the fileName(string) has .ppm extension
     @input: "fileName" name of the file that we wish to check.
     @output: none
     @return: "ppmExtension" is a flag.If the file has ".ppm", the flag is true.
               Else, the flag is false.
     @precondition: assume that fileName is a string with \0.
     **/
    
    int length = strlen(fileName);
    int ppmExtension = 0; // Flag: 0= False   1= True
    int dotDetected = 0; 
    
    // Step 1: Determine whether the filename has a "." character
    //Find Index of the '.'
    int i;
    for(i=0; i < length ; i++)
    {
        if(fileName[i]=='.')
        {
            dotDetected = 1;
            break;
        }
        
    }
    
    // Step 2: Determine whether the file has the extension ".ppm"
    if(dotDetected==1)
    {
        if(fileName[i+1]=='p' & fileName[i+2]=='p' & fileName[i+3]=='m')
        {
            ppmExtension = 1;
        }
    }
    
    return ppmExtension;
    
}

int countLines(char *fileName)
{
    /** 
     Count # of Lines in a given File.
     @input: "fileName" name of the file that we wish to read.
     @output: none
     @return:  "count" is the number of lines in a given file
     @preconditions: 
     (1) assume each line ends with '\n' as referenced to the Assignment 2 - Task 2 Notes
     (2) assume that each file has upper limit of 50 lines.
     (3) assume each line has three valid arguments where each argument has no white-space
         (e.g message input.ppm output.ppm)
     (4) assumes the length
     **/
    
    
    FILE *inPtr = fopen(fileName,"rb");
    int count = 0; // Count the # of Lines
    if(inPtr==NULL)
    {
        fprintf(stderr,"ERROR: UNABLE TO OPEN THE FILE '%s'\n",fileName);
        exit(-1);
    }
    
    char temp[3][101]; // Temp: used to store the 3 arguments
    int i = 0; // Index
    
    while(!feof(inPtr))
    {
        
        fscanf(inPtr,"%s %s %s\n",temp[0],temp[1],temp[2]);
        //Check whether 2nd and 3rd argument of a given line "message input.ppm ouput.ppm" have .ppm extension.
        if(checkPPM(temp[2])!=1  | checkPPM(temp[2])!=1) // Check for Invalid PPM files
        {
            fprintf(stderr,"ERROR: Invalid PPM file arguments provided\n");
            exit(-1);
        }
        count++; // Increment Line Count.
    }
    fclose(inPtr);
    
    return count;
    
}



char *subArray(char *msgArray,int startPos,int endPos)
{
    /**
     This function creates a sub-array.Equivalent to Slicing Function.
     @input: "msgArray" array contains the secret message that we wish to slice.
     "startPos" is an integer representing the starting index of an sub-array
     "endPos" is an integer representing the ending index of an subarray
     @output: none
     @return:  "subarray" is an array of characters representing a subarray.
     @precondition: none.
     
     **/
    
    if(msgArray==NULL)
    {
        fprintf(stderr,"ERROR: subArray() - NULL pointer is provided as an array. \n");
        exit(-1);
    }
    
    // Ensure: startPos < endPos
    if(endPos < startPos)
    {
        fprintf(stderr,"ERROR: subArray() - ending Index is greater the starting index \n");
        exit(-1);
    }
    // Ensure: startPos and endPos > 0 (Negative Index Numbers not allowed)
    if( endPos < 0 | startPos < 0)
    {
        fprintf(stderr,"ERROR: subArray() - supplied startPos or endPos Integer cannot be negative\n");
        exit(-1);
    }
    // Ensure: endPos is valid.
    if( endPos >= strlen(msgArray))
    {
        fprintf(stderr,"ERROR: subArray() - invalid endPos Integer Index Value\n");
        exit(-1);
    }
    
    int lengthSubMessage = endPos - startPos + 1; // # of characters of the sub-message/ partial message
    int q; // Used for the Loop
    int r = 0; // Used for the Loop
    char tempArray[lengthSubMessage+1]; // Plus 1 for the Null Terminating character
    
    for(q=startPos;q<=endPos;q++)
    {
        tempArray[r] = msgArray[q];
        r = r + 1;
    }
    tempArray[r] = '\0';
    //printf("Partial Message is: %s with length %d   index r is: %d\n",tempArray,lengthSubMessage,r);
    //printf("Partial Message Length is: %lu\n",strlen(tempArray));
    
    
    // Create an appropriate array to return.
    char *subMsg = (char *)malloc(sizeof(char)*(strlen(tempArray)+1)); // '+1' for the '\0' character.
    if(subMsg==NULL)
    {
        fprintf(stderr,"ERROR: Insufficent Memory for malloc()\n");
        exit(-1);
    }
    strcpy(subMsg,tempArray); // Copy message from static array to dynamic memory allocated memory
    printf("Portion of Secret Message:%s\n",subMsg);
    
    
    
    return subMsg;
    // REMEMBER TO DEALLOCATE MEMORY.
    
}







char *readSecret()
{
    /**
     This function, extracts the "secret" from stdin (keyboard or redirected file) and stores it into a character array.
     If the User decides to provide a secret message using the keyboard, he/she must press "Crt-D" to send a 'EOF' to indicate end of message.
     @input: None
     @output: "secretMsg" is a character array containing the secret message
     @return:  secretMsg" is a character array containing the secret message - Dynamic Memory Allocated !!!!
     @precondition: assumes that the # of characters in the user-defined secret message is less than msgSize.
     
     **/
    
    int msgSize = 4000001; // length of secret message, including an extra'\0' character (i.e. 4mil chars)
    //int msgSize = 5; // Testing purposes with "hello"
    char msg[msgSize]; // Store Secret Message
    int readMsgFinish = 0;// Flag variable. Value of 1 if the MSG is completely read
    
    
    int i; // Loop Counter
    char c; //Store a single character from a given message
    for(i=0; i < msgSize ; i++)
    {
        c = fgetc(stdin);
        
        //User has to Press "Crt-D" to end the message.
        //CASE 1: Finished Reading Secret Message
        if((unsigned)c==EOF)
        {
            msg[i] = '\0'; // Add a '\0' end of string character
            //printf("\n-----REACHED END OF MESSAGE-----\n");
            readMsgFinish = 1;
            break;
        }
        //CASE 2: Copy over a single character of the secret message
        else
        {
            msg[i]=c;
        }
        
    }
    
    // The entire message has not been read in due to it being too large.
    if(readMsgFinish==0)
    {
        fprintf(stderr,"ERROR: Hide() terminated due to message is too large\n");
        exit(-1);
    }
    
    
    //printf("%s\n",msg);
    //printf("Length of Message: %lu\n",strlen(msg));
    
    
    // Create an appropriate sized array to return.
    char *secretMsg = (char *)malloc(sizeof(char)*(strlen(msg)+1)); // '+1' for the '\0' character.
    
    if(secretMsg==NULL) 
    {
        fprintf(stderr,"ERROR: Insufficent Memory for malloc()\n");
        exit(-1);
    }
    strcpy(secretMsg,msg);
    //printf("\nSecret Message:\n%s\n",secretMsg); // !!!!!!!!!!!!!PRINTS OUT THE SECRET MESSAGE !!!!!!!!!!!!!
    //printf("Length of Message: %lu\n",strlen(secretMsg));
    //printf("Last character: %c\n",secretMsg[strlen(msg)+2]);
    
    // Handle Empty Message
    if(strlen(secretMsg)==0)
    {
        fprintf(stderr,"ERROR: User has provided an empty message to hide\n");
        exit(-1);
    }
    
    return secretMsg; // NOTE: Remember to free the secretMsg
    
}

void write_header(int *dimenChannel,FILE *fp)
{
    /**
     Write the ASCII Header for a PPM file.
     It includes the magic number,dimension and channel value.
     @input: '*dimChannel' is an array containing the width,height and channel value.
             '*fp' - file pointer to an empty ppm file.
     @ouput: writes the header components into ppm file.
     @return: none
     **/
    if(fp==NULL)
    {
        fprintf(stderr,"ERROR: write_header() file pointer points to NULL\n");
        exit(-1);
    }
    
    fprintf(fp,"P6\n"); // Magic Number
    // Check values of dimenChannel are all positive integer values. 
    if(dimenChannel[0] <=0 | dimenChannel[1] <=0 | dimenChannel[2] <=0)
    {
        fprintf(stderr,"ERROR: The values (width,height,channel value) must be all > 0 \n");
        exit(-1);
    }
    
    fprintf(fp,"%d %d\n",dimenChannel[0],dimenChannel[1]); // Dimension - width and height.
    fprintf(fp,"%d\n",dimenChannel[2]); // Channel Value

}



int *getBinaryArray(char z)
{
    /**
     This function takes in a single 'character' (e.g. a) and converts it into its binary
     representation stored in a string.
     @input: 'z' is a single character
     @output: none
     @return: 'binaryArray' string containing the binary representation. 1 char = 8 bits.
     
     NOTE: "//DEV1" Comments in this function getBinaryArray, will print out the binary representation
            of an ASCII character if the line is uncommented. - Testing purposes.
     
     **/
    
    int values[8]; // Arrays of 1's and 0's - binary representation of a 'char'
    int bitPos; //Bit Position
    char temp;
    
    //Extracting each digit of the 8-bit binary by shifting each digit to the least-significant position.
    for(bitPos = 0; bitPos < 8 ; ++bitPos)
    {
        temp = z >> bitPos;// Shift the "wanted" digit to the least-significant position(to right-most position)
        values[7-bitPos] = temp & 1; // Mask it by 11111110 to get the least-significant position
    }
    
  
    //Return dynamic allocated memory - MUST DEALLOCATE THE MEMORY
    int *binaryArray;
    binaryArray = (int *) malloc(sizeof(int)*8);
 
    
    //Copy values[] to allocated memory called binaryArray
    for(bitPos = 0; bitPos < 8 ; ++bitPos)
    {
        binaryArray[bitPos] = values[bitPos];
        
    }
    
    
    //CODE BLOCK BELOW: Print out binary representation of the character
    //printf("character %c :",z); // DEV1: TURN BACK ON FOR DEVELOPMENT PURPOSES. 
    for(bitPos = 0; bitPos < 8 ; ++bitPos)
    {
        //printf("%d",binaryArray[bitPos]);  // DEV 1: TURN BACK ON FOR DEVELOPMENT PURPOSES.
        if(bitPos==7){
            //printf("\n"); // DEV1: TURN BACK ON FOR DEVELOPMENT PURPOSES. 
        }
    }

    
    return binaryArray;
}

void binaryEncode(FILE *inPtr,FILE *outPtr,char hiddenMsg[])
{
    /**
     This function does the following:
     (1) encode the secret the message into the output file.
     (2) copy the unmodified binary parts of the image to the output file.
     
     @input: (1)'*inPtr' a pointer to the original ppm file
             (2) '*outPtr' pointer to the output ppm file
             (3) 'hiddenMsg' a string containing the hidden message
     @output: encode message into the output file and copy over unmodified binary parts.
     @return: none
     @precondition: assume the '*ipPtr' points the beginning of the binary section of ppm file
     
     **/
    
    
    
    /* ----------------------- Encode the Message and transfer to output file --------------*/
    int index = 0;
    int msgLength = strlen(hiddenMsg);
    //The targeted byte that is required to be modified in picture(i.e a pixel color = 8 bits = 1 byte)
    unsigned char someByteInPic;    // A Byte from the original input ppm file
    unsigned char someByteInPicMod; // Modified byte destined to be stored in output ppm file.

    
    //Loop through each character of the string containing the hidden message
    for(index = 0; index <=msgLength; ++index)
    {

        
        // Convert the single character to binary form (string representation)
        // Array used to store the binary string representation of the value.
        int *binaryArray;
        binaryArray = getBinaryArray(hiddenMsg[index]);
        
        //AIM: Go through each of the bit in the binary array and hide the message.
        int j;
        for(j=0;j<8;++j)
        {
            
            //Get the Pixel in original ppm to modify
            someByteInPic = fgetc(inPtr);
            //printf("Original Pixel Color : 0x%02x\n",someByteInPic);
            
            //Modify that color pixel.
            if(binaryArray[j] == 1)
            {
                someByteInPicMod = someByteInPic | 1;
                //printf("Modified Pixel Color: 0x%02x\n",someByteInPicMod);

            }
            else // binaryArray[j]==0
            {
                someByteInPicMod = someByteInPic & ~1;
                //printf("Modified Pixel Color: 0x%02x\n",someByteInPicMod);
            }
            
            // Store the modified color pixel into the output ppm file
            fwrite(&someByteInPicMod,1,1,outPtr);
        }
        
        
        // Free the dynamic-allocated space.
        free(binaryArray);
        
    }
    //printf("FINISHED COPYING SECRET MESSAGE into output file\n");


    
    // COPY THE COLOR PIXELS  THAT WERE NOT MODIFIED in the original ppm to OUTPUT.PPM FILE
    while(!feof(inPtr))
    {
        char c = fgetc(inPtr);
        //printf("%x\n",c);
        fwrite(&c,1,1,outPtr);

        
    }

    
}

int msgPixelsRequired(char *hiddenMsg)
{
    /**
     This function, calculates the # of pixels required by the messsage(including \0), in order to hide it.
     
     @input: 'hiddenMsg' - string of the secret Message
     @output: none.
     @return: "pixelsRequired" integer- No. of Pixels required to hide message(incl. \0 character)
     @precondition: none.
     **/
    
    
    /*Number of characters that can be stored in the given Image.
    
     Let pixelsAvail = Pixels Available in Picture.
     Let noChars = Number of characters that can be stored in the picture (including Null Terminating String character)
     Let totalBits = pixelsAvail * 3 ; Each Pixel has RGB values <=> 1 pixel = 3 Colors
     So. noOfChars = (totalBits / 8 bits per color) = # of characters that can be stored in the image
     
     */
    
    //Determine the resource requirements by the Secret Message
    int messageLength = strlen(hiddenMsg); //Count number of characters in string (excluding the null '\0')
    int noOfBits = (messageLength+1) * 8;  //noOfBits:Number of Bits required to store the message (1 extra space for '\0')
    printf("# of Characters in message(excluding null terminator): %d\n",messageLength);
    printf("# of Characters in message(including null terminator): %d\n",messageLength+1);
    printf("# of bits required(including NULL Terminating character): %d\n",noOfBits);
    
    //# of pixels required to store the message.
    int pixelsRequired; //# of pixels required to store the message.
    //Condition 1: If there exist a remainder, then round up  pixels up by 1. (i.e 7.3 to 8)
    if(noOfBits % 3!=0)
    {
        pixelsRequired =(int)(noOfBits/3) + 1;
    }
    //Condition 2: noOfBits/3 has no remainder
    else
    {
        pixelsRequired = noOfBits/3;
    }
    printf("# of Pixels required: %d\n",pixelsRequired);
    
    return pixelsRequired;
    
    
}


void modeZero(char *inputFileName,char *outputFileName)
{
    /**
     This function encodes a message into a SINGLE IMAGE PPM.
     It used by the  optional flags of "-p" parallel execution function.
      and "-s" display images function.
  
     
     @input: (1)'*inputFileName' - string: filename of the input ppm file.
             (2) *outputFileName' - string: filename of the output ppm file,used to hide the secret messsage
     @output: encode message into the output file.
     @return: none.
     @precondition: none.
     **/
    
    
    /*------------- CREATE FILE POINTERS AND OPEN FILES ----------------- */
    // Create FILE Pointers
    FILE  *fpIN;
    FILE  *fpOUT;
    
    //Open the given input and output files.
    fpIN = fopen(inputFileName,"rb");

    //Check whether the input file was open sucessfully
    if(fpIN == NULL)
    {
        fprintf(stderr,"ERROR: The file '%s' cannot be read\n",inputFileName);
        exit(-1);
    }

    fpOUT = fopen(outputFileName,"w+");
    

    
    
    /* ----------------------- READ IN THE HEADER SECTION OF PPM ----------------------------------*/
    //(1) "Parse through header" AND (2) "extract dimension and header of PPM"
    int *dimChan = read_header(fpIN);
    
    
    /* ------------------------ GET USER INPUT FOR HIDDEN MESSAGE ----------------------------------------*/
    // MUST PRESS CRT-D to Send EOF SIGNAL(i.e. the message has ended)
    printf("Type in hidden message: \n");
    char *hiddenMsg = readSecret();
    
        
    printf("\n---- Processing ----\n");
    // PART A: Determine the resource requirements by the Secret Message
    int pixelsRequired = msgPixelsRequired(hiddenMsg);
 

    // PART B: Determine the # of pixels available in the given file.
    int pixelsAvail = dimChan[0] * dimChan[1]; //Calculate the available pixels in the ppm file -> pixels = width *height
    int noOfChars = (pixelsAvail*3)/8;  // Number of characters that can be stored in the image including \0
    printf("# of Characters that can stored in the IMAGE(Note: must reserve 1 char for NULL String Terminator): %d\n",noOfChars);
    printf("# of Pixels available: %d\n",pixelsAvail);
    
    
    if(pixelsRequired > pixelsAvail)
    {
        fprintf(stderr,"ERROR: The Message is too big to store in the image called '%s'\n",inputFileName);
        exit(-1);
    }
    else
    {
        printf("VALID-SUCESS: Message can be stored in the input image '%s' and output file is: %s\n",inputFileName,outputFileName);

    }
    
    
    
    
    /* ---------------------- WRITE THE PPM FILE (i.e encode message) ----------------------------*/
    
    write_header(dimChan,fpOUT);
    binaryEncode(fpIN,fpOUT,hiddenMsg); // Encode the binary values.
    
    
    /* ------------------------ OTHER ------------------------------------ */
    // Free mallocated memory
    free(dimChan);
    free(hiddenMsg);
    
    
    
    // Close the Files
    fclose(fpIN);
    fclose(fpOUT);
    
}

void modeOne(char *inputFileName,char *outputFileName,int noOfFiles)
{
    /**
     This function handles the encoding of the secret message for Task 1 of Assignment 2.
     
     @input: (1)'*inputFileName' - the filename of the ppm file: base-name
             (2) '*outputFileName' - string: filename of the output-base-name.ppm,used to hide the secret messsage
             (3) 'noOfFiles' - Integer: # of files with the base-name(assume not identical) (i.e. kittens-001.ppm)
     @output: encode message into one or multiple output files using the output-base-name.
     @return: none.
     @precondition: none.
     **/
    
    FILE *inPtr; // File Pointer to read images.
    FILE *outPtr; // File Pointer to write images.
    if(inPtr == NULL | outPtr == NULL)
    {
        fprintf(stderr,"ERROR: modeOne() inPtr or outPtr file pointers point to NULL\n");
        exit(-1);
    }
    if(noOfFiles <=0)
    {
        fprintf(stderr,"ERROR: modeOne() - noOfFiles must be > 0\n");
        exit(-1);
    }
    int *dimChan;// Store Header Contents of a IMAGE: Extract "Width Height ChannelValue"

    
    
    /* ------------------------ GET USER INPUT FOR HIDDEN MESSAGE ----------------------------------------*/
    // MUST PRESS CRT-D to Send EOF SIGNAL(i.e. the message has ended)
    printf("Type in hidden message: \n");
    char *hiddenMsg = readSecret();
    // Determine the # of characters required by secret message
    int messageLength = strlen(hiddenMsg); //Count number of characters in string (EXCLUDING the null '\0')
    

    
    /* ------------------------ Check IF THERE ARE ENOUGH SPACE IN IMAGES ----------------------------------------*/
    int totalCharsAvail = 0;  // Total # of characters Available across all images.
    int currentChars = 0; // the # of characters available for a given file.
    int i; // Index for the loop
    int pixelsAvail; // No. of Pixels Available for a given image
    int charsAvailArray[noOfFiles]; // Stores the "No. of characters available(must reserve 1 char for NULL)" for each image stored in an ARRAY.
    
    // Creating arrays with enough space to store the fileNames(string) of the base-name and output-base-name. 
    int lengthBase = strlen(inputFileName); // Number characters in base-name (excluding \0)
    int lengthOutputBase = strlen(outputFileName); // Number characters in output-base-name (excluding \0)
    // size of newBaseName  char: "lengthBase" characters + 1 char "-" + 3 digits + 4 chars ".ppm"+  1 Null character
    char newBaseName[lengthBase+9]; // For the Input File.
    // size of newBaseOutputName  char: "lengthOutputBase" characters + 1 char "-" + 3 digits + 4 chars ".ppm"+  1 Null character
    char newBaseOutputName[lengthOutputBase+9];
    

    
    // For each input image(base-name i.e. image-000.ppm image-001.ppm ...), determine the number of characters available, and store it into an array
    for(i = 0; i < noOfFiles; i++)
    {
        sprintf(newBaseName,"%s-%03d.ppm",inputFileName,i); // File-Naming Convention - Generate fileName of the input-base-name.
        inPtr = fopen(newBaseName,"rb"); // Open the Image
        dimChan = read_header(inPtr); // Read in Header of Image : Extract "Width Height ChannelValue"
        pixelsAvail = dimChan[0] * dimChan[1]; //Calculate the available pixels in the ppm file -> pixels = width *height
        currentChars = (pixelsAvail*3)/8;  // Calculate Number of characters that COULD be stored in the image including \0
        totalCharsAvail = totalCharsAvail + currentChars; // Total characters that can be stored.
        charsAvailArray[i] = currentChars; // Stores the # of characters available for each given Image in an array.
        printf("FileName: %s has pixels:%d  which can store up to %d characters(note: must reserve 1 char for NULL)\n",newBaseName,pixelsAvail,charsAvailArray[i]);
        fclose(inPtr);
    }
    
    // Adjust totalCharsAvail, to reflect that each PPM Image must store '\0' when storing a portion of the secret message
    // (e.g. reserve 1 character '\0' in each image. totalCharsAvail is the # of characters available in total from all images )
    totalCharsAvail = totalCharsAvail - noOfFiles;
    printf("Total Available Characters(taken into account where each image stores NULL character): %d\n",totalCharsAvail);
    printf("The number of characters required(excluding NULL) by message: %d\n",messageLength);
    if(messageLength > totalCharsAvail)
    {
        fprintf(stderr,"ERROR: Message is too big to store in the given image(s)\n");
        exit(-1);
    }
    
    
    /* ------------------------ ENCODING THE MESSAGE ----------------------------------------*/
    //Split the message into separate arrays by using slicing function subArray() in hide.c
    int startPos = 0; // Starting Index of current sub-array
    int finalPos = strlen(hiddenMsg) -1 ; //  Index of final character of HiddenMsg(Ignoring \0)
    int endPos = 0; // Ending index of current sub-array
    int nextPos = 0; // Starting Index of Next Sub-Array
    int charsRead; // # of characters read so far.
    int charsBlock; // # of characters available in a particular image(that can be stored)
    int charsRemaining = messageLength; // # of characters remaining in message ( excluding NULL character)
    int s = 0; // Index Array ( refers to the ith item . e.g.  if s=0 =>  baseNameImage-000.ppm , if s=1 => baseNameImage-001.ppm)
    // the "s" is used as index for the charsAvailArray[] which stores the "No. of characters available(must reserve 1 char for NULL)" for each image.
    // e.g. if s=0  then charsAvailArray[0] refers the # of characters available(must reserve 1 char for NULL) that can be stored in baseNameImage-000.ppm
    //      if s=1, then charsAvailArray[1] refers the # of characters available(must reserve 1 char for NULL) that can be stored in baseNameImage-001.ppm
    i =0; // reset index used for file naming convention for base-name and output-base-name files. (i.e Image-000.ppm Image-001.ppm ...)
    
    char *partMessage;// Array to used to store a portion of the secret message, as a result from slicing
    
    
    printf("\n------------------------PROCESS: ENCODING MESSAGE -------------------------------------\n");
     //Loop until the entire message is encoded. 
    while(charsRemaining > 0)
    {
        charsBlock = charsAvailArray[s] - 1; // -1: take into account, that 1 char is reserved the null character when a message is stored in an image.
        printf("No. of Chars available in Image %d: %d      No. of Chars Remaining: %d\n",s,charsBlock,charsRemaining);
        
        if(charsAvailArray[s]==0) // Terminate the program if one of the images can't store a single character.
        {
            fprintf(stderr,"ERROR: ./hide -m -> Image %s-%03d.ppm cannot store a single character\n",inputFileName,s);
            exit(-1);
        }
        
        // Determine number of characters Read.
        // CONDITION 1: # of characters remaining < # of characters available in the image that could be used  - (used only some of the characters/SPACE available in the image)
        if(charsRemaining < charsBlock)
        {
            charsRead = charsRemaining;
        }
        // CONDITION 2: # of characters remaining >= # of characters available in the image (used up all characters/SPACE in the image)
        else
        {
            charsRead = charsBlock;
        }
        
        
        // Determine the indexes of the sub-arrays and Encode Secret message.
        // CONDITION 1: Image File has space for to store at least 1 character of message (excluding the \0)
        if(charsRead > 0)
        {
            // Part A: Determine the relevant index to extract a portion of the secret message
            startPos = nextPos;  // Starting Index of current sub-array
            nextPos = startPos + charsRead;  // Starting Index of Next Sub-Array
            endPos = nextPos -1 ; // Ending Index of current sub-array
            printf("SubArray Index: startIndex:%d  endIndex:%d \n",startPos,endPos);  // Ending index of current sub-array

            // Part B: Extract a portion of the secret message into an string. - Slicing
            partMessage = subArray(hiddenMsg,startPos,endPos);
            
            // Part C: Generate the file-names of base-name and output-basename and store them.
            sprintf(newBaseName,"%s-%03d.ppm",inputFileName,s); // File-Naming Convention:
            sprintf(newBaseOutputName,"%s-%03d.ppm",outputFileName,s); // File-Naming Convention:
            printf("Input File: %s     Output File: %s\n",newBaseName,newBaseOutputName);
            
            
            // Part D: Open the input-file and output-file.
            inPtr = fopen(newBaseName,"rb"); // Open the Image to Read
            outPtr = fopen(newBaseOutputName,"w+"); // Open Image to Write
            
            // Part E : Encode the message.
            dimChan = read_header(inPtr); // Read in Header of Image : Extract "Width Height ChannelValue"
            write_header(dimChan,outPtr);
            binaryEncode(inPtr,outPtr,partMessage); // Encode the binary values.
            
            // Part F: Close the input-file and output-file.
            fclose(inPtr);
            fclose(outPtr);
            
            // Part G : Clean-up
            free(partMessage);//Deallocate Dynamic Memory
        }
        // CONDITION 2: Image File CAN ONLY STORE 1 character which is '\0'
        else
        {
            // Part A: Generate the file-names of basename and output-basename files.
            sprintf(newBaseName,"%s-%03d.ppm",inputFileName,s); // File-Naming Convention:
            sprintf(newBaseOutputName,"%s-%03d.ppm",outputFileName,s); // File-Naming Convention:
            printf("Input File: %s     Output File: %s\n",newBaseName,newBaseOutputName);
            
            
            // Part B: Open the input-file and output-file.
            inPtr = fopen(newBaseName,"rb"); // Open the Image to Read
            outPtr = fopen(newBaseOutputName,"w+"); // Open Image to Write
            
            // Part C : Encode the message.
            dimChan = read_header(inPtr); // Read in Header of Image : Extract "Width Height ChannelValue"
            write_header(dimChan,outPtr);
            binaryEncode(inPtr,outPtr,""); // Encode an empty string which has \0.
            
            // Part D: Close the input-file and output-file.
            fclose(inPtr);
            fclose(outPtr);  
        }

        charsRemaining  = charsRemaining - charsBlock;
        printf("No. of Chars Read: %d\n\n",charsRead);

        s = s + 1; // Move onto the next Image
    }
    // NOTE THAT: If an image can't store any characters including '\0' is dealt by 
    //              this function and would raise an error and terminate the program. 
    
    /* ------------------------ OTHER ------------------------------------ */
    // Free mallocated memory
    free(dimChan);
    free(hiddenMsg);

    
    // DON'T FORGET CLOSE FILE CONNECTION FILE CONNECTION

    
    
}
    
    



int main(int argc,char *argv[])
{
    /* The main function() deals with the various flags given by the user
       for the hide() function.
       These include:
         (1) ./hide input.ppm output.ppm    (encode a message into a single ppm file)
         (2) ./hide -m number-of-files basename output-base-name (encode a message across multiple files)
         (3) ./hide -p file (encode several messages at the same time - parallel execution)
         (4) ./hide -s input.ppm output.ppm (display the before and after-images,after encoding a message).
     */


    char *inputFile; // file name of base-name.
    char *outputFile; // file name of output-base-name.
    char *file; // file containing the list of commands for "Parallel Execution"
    
    if(argc == 5) // OPTION 1: Deals with the encoding of a message across multiple ppm files.(TASK 1)
    {
        if(strcmp(argv[1],"-m")!=0)    // Check for a valid option flag of '-m'.
        {
            fprintf(stderr,"Error: Supplied invalid flag.\n");
            fprintf(stderr,"Expected Arguments: ./hide -m NoOfFiles basename output-basename \n");
            exit(-1);
        }


        int noOfFiles = atoi(argv[2]); // Number of Files Provided by User

        if(noOfFiles==0) // Handle case when user provide "number-of-files" that is zero or when atoi() fails
        {
            fprintf(stderr,"ERROR: Insufficent number-of-files provided in argument.Provide an integer value > 0\n");
            exit(-1);
        }
         
        inputFile = argv[3]; // basename
        outputFile = argv[4]; // output-basename
        //printf(" noOfFiles: %d basename: %s output-basename: %s\n",noOfFiles,inputFile,outputFile);



        
        // Check whether the specified "number-of-files" matches actual files available in the current directory.
        if(countValidFiles(inputFile) != noOfFiles )
        {
            fprintf(stderr,"ERROR:The specified number-of-files doesn't match actual # of files in the directory\n");
	    fprintf(stderr,"ERROR: Actual number-of-files: %d  Expected number-of-files(user typed in): %d \n",countValidFiles(inputFile),noOfFiles);
            exit(-1);
        }
        
        // Program only supports up to 999 Images.
        if(noOfFiles > 999)
        {
            fprintf(stderr,"ERROR:Exceeded Limit - Program only supports up to 999 Images.\n");
            exit(-1);
        }
        
        modeOne(inputFile,outputFile,noOfFiles); // Encode the secret message.
        
        
    }
    //OPTION 2 and OPTION 3: User provides 3 arguments
    else if (argc ==3)
    {
        // OPTION 2: Support "p" PARALLEL PROCESSING flag. - TASK 2
        if(strcmp(argv[1],"-p")==0) 
        {
            file = argv[2]; //Text File Containing the list of commands
            int lines = countLines(file); // Count # of Lines the given file.
            //printf("Number of Lines: %d \n",lines);

            
            char temp[3][101]; // Hold the three arguments per Line- Asssume length of each arg is <100chars
                              // i.e  message.txt inputImage.ppm outputImage.ppm
            
            FILE *filePtr = fopen(file,"rb"); //Open the File that contains the arguments
            
            //Check the file was opened sucessfully. File containing the names of the message, input and output images.
            if(filePtr==NULL)
            {
                fprintf(stderr,"ERROR: main() with ./hide -p Unable to open the file containing the lines of arguments\n");
                exit(-1);
            }
            

            
            
       /* ---------------------------- Parallel Execution Code ---------------------------------- */
       // Number of Times Fork = Number of Lines. => Let all the children do all the work.
       // The parent watches the children and check whether the children terminate (avoid zombies).
        
            // Rehash basic facts: (self-reminder).
            // fork() returns negative, if unable to create child
            // fork() returns "positive value- the process id of the child" to the PARENT"
            // fork() returns "zero" to the CHILD PROCESS
            
            //---------------------LOOP 1: Create the processes by FORK()---------------------
            
            int i = 0;
            pid_t pid; // Process ID
            for (i = 0; i < lines ; i++)
            {
                
                fscanf(filePtr,"%s %s %s\n",temp[0],temp[1],temp[2]); // Extract arguments from the ith line of the file.
                pid = fork(); // Create a child new process.
                if(pid==0) // Child Process
                {
                    printf("------------------------------------------------------------\n");
                    printf("I'm the child with id:%d with the i value: %d\n",getpid(),i);
                    printf("Arguments are: %s %s %s\n",temp[0],temp[1],temp[2]);
                    
                    
                    FILE *myFile = fopen(temp[0],"r");
                    if(myFile == NULL) // Check whether the "message" file was opened sucessfully.
                    {
                        fprintf(stderr,"ERROR: ./hide -p : Unable to open the message file '%s'\n",temp[0]);
                        exit(-1);
                    }
                    dup2(fileno(myFile),STDIN_FILENO);     // Provided by Assingment Sheet
                    fclose(myFile); // Close the file used by dup2.
                    fclose(filePtr); // Close the file pointer used by the child.

                    execl("./hide","./hide",temp[1],temp[2],(char*)NULL); // From exec() Family of Functions.
                    //Usage Reference for execl: https://linux.die.net/man/3/execl
                    exit(0); // The Child Process-No longer executes or participates in the the LOOP ABOVE & BELOW.
                    
                }
                else if ( pid > 0) // Parent Process
                {
                    // Sequential process: From Week 7 Lab.
                    //printf("I'm the parent with id: %d with the i value: %d\n",getpid(),i);
                    //int status;
                    //waitpid(pid,&status,0); // Wait for the child to finish and capture its value
                    //printf("The result was %d\n",WEXITSTATUS(status));
                }
                else // (pid < 0) ERROR
                {
                    fprintf(stderr,"Error: Unable to create a child process\n");
                    exit(1);
                }
            }
            

            
            // LOOP 2: Wait for child processes to end. Avoid Zombies.
            // Documentation: http://pubs.opengroup.org/onlinepubs/9699919799/functions/wait.html
            // Approach Based on http://stackoverflow.com/questions/876605/multiple-child-process
            
            int j = 0;
            for(j=0; j < lines ; j++)
            {
                int status; // Hold the status of the child process.
                
                
                /*waitpid(): returns processe ID of child process due to status is available
                 *waitpid(): argument "-1" means wait for ANY single child process.
                 *waitpid(): "&status" stores the status when a process termination occurs
                 *waitpid(): zero options/flag */
                
                pid_t childId = waitpid(-1,&status,0); // Request for any single child process
                //printf("Parent Process: %d noticed the process %d has ended\n",getpid(),childId);
            }
            
            fclose(filePtr); // Parent closes the file connection.


        /* -------------------------------------------------------------------------------------- */
  
        }
        // OPTION 3 of Hide: No Flags are selected for ./hide.
        //  Standard Execution from Assingment 1 
        else
        {
            inputFile = argv[1]; // filename of input ppm file.
            outputFile = argv[2]; // filename of the output ppm file.
            modeZero(inputFile,outputFile);
        }
        
    }
    //OPTION 4: Support for the "-s" flag.
    // i.e Display the before and after images, after encoding secret message. 
    else if(argc==4)
    {
        // Check for a valid flag "-s"
        if(strcmp(argv[1],"-s")!=0) // Invalid Flag
        {
            fprintf(stderr,"Error: main()- Supplied invalid flag.\n");
            fprintf(stderr,"Expected Arguments: ./hide -s inputFile.ppm outFile.ppm \n");
            exit(-1);
        }
        inputFile = argv[2]; // filename of input ppm file.
        outputFile = argv[3]; // filename of the output ppm file.
        //printf("FILE NAMES: %s %s \n",inputFile,outputFile);
        modeZero(inputFile,outputFile); // Call default ./hide function without any flags being provided.
        displayImage(inputFile,outputFile); // Display the Image.
        
    }
    else
    {
        fprintf(stderr,"Error: main() - Supplied invalid arguments \n");
        exit(-1);
    }
    
    
    
    return 0;
}

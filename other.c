/*
 *Created by vahuy on 30/4/17.
 * The other.c file is used by hide.c and unhide.c.
 * The main function is "countValidFiles", which counts the #
 * of files in the directory with the base-name.
 * (i.e base-000.ppm base-001.ppm ... base-009.ppm)
 * This is used by "./hide -m" to verify that the argument
 * "number-of-files" with the base-name provided is true.
 * While, "./hide -m" uses the function to determine the
 * the number of files that could contain the secret message.
 */
#include "other.h"


int countValidFiles(char baseName[])
{
    /**
     This function counts the number of files with the base-name.
     @input: "basename[]" is a string of the base-name
     @output: none
     @return:  "count" is an integer -  the # of files with the "base-name"
     @precondition:  The numbering of the base-name counts from 0. (i.e kittens-000.ppm)
     
     
     **/
    int count = 0; // Count is # of files that could be open
    int index = 0; // Assume the basename starts counting from 0.
    int invalidReadDetected = 0; // Flag: 0 = False         1=True
    int lengthBase = strlen(baseName); // Number characters in baseName (excluding \0)
    // for newBaseName the size: "lengthBase" characters + 1 char "-" + 3 digits + 4 chars ".ppm"+  1 Null character
    char newBaseName[lengthBase+9];
    int successCreate; //Check whether sprintf() was a success. if "success" < 0, ERROR
    
    FILE *fpIn; // File Pointer to Images
    
    
    while(invalidReadDetected==0)
    {
        
        // STEP 1: Create the string of the file-name (i.e. base-name000.ppm)
        successCreate = sprintf(newBaseName,"%s-%03d.ppm",baseName,index); // Create the string of the filename
        if(successCreate<0)  //Check whether sprintf() was a success.
        {
            fprintf(stderr,"ERROR: insufficent allocated space for sprintf() in countValidFiles() \n");
            exit(-1);
        }
        //printf("FileName: %s\n",newBaseName);
        //printf("Length Base: %lu\n",strlen(newBaseName));
        
        // STEP 2: Try to open the FILE
        fpIn = fopen(newBaseName,"r");
        // CASE 1: Unable to open the file
        if(fpIn==NULL)
        {
            invalidReadDetected = 1;
            
        }
        // CASE 2: Able to open the File
        else
        {
            //Increment the index used for the base-name.
            index++;
            count++;
            fclose(fpIn);// Close the File Connection.
        }
        
        
    }
    
    
    return count;
}

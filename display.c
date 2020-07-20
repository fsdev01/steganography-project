/**
This file contains two functions that are designed to display a two
ppm images. 
 (1) displayImage() is the main function that displays the image.
 (2) getPixel() supports displayImage() by extracting the individual RGB value
     of a pixel.
The displayImage() function is used by the hide.c file. 

**/
#include "display.h"

char *getPixel(FILE *fpIN)
{
    
    /**
     This function gets the R,G,B values for a given pixel and returns it.
     @input: "fpIn" File pointer to a valid ppm file
     @output: none.
     @return: "rgbaValue" is a character array containing the RGB values of a pixel.
     @precondition: *fpIn is positioned in the binary section of the ppm file
     **/
    
    
    unsigned char pixel[3]; // Store RGB values of a given Pixel
    int i; // Variable used to initalise loop
    for(i = 0 ; i < 3; i++)
    {
            unsigned char c = fgetc(fpIN);
            pixel[i] = c;
        
            //printf("CURRENT BYTE: 0x%02x\n",(unsigned char)c);
            
            if(c==EOF) // Reached End of File because of invaid ppm - Missing RGB Component/ Incomplete RGB Value.
            {
                printf("ERROR: getpixel() - DETECTED END OF FILE: CURRENT BYTE: 0x%02x\n",(unsigned char)c);
                fprintf(stderr,"ERROR: getpixel() - Invalid: Incomplete Pixel - Missing at least 1 color component\n");
                exit(-1);
            }
    }

    // Dynamically Allocate Memory - 3 values (Red,Green,Blue)  // REMEBER TO DEALLOCATE MEMORY
    char *rgbValue = (char *)malloc(sizeof(char)*3);
    rgbValue[0] = pixel[0]; // Red Value in hexadecimal
    rgbValue[1] = pixel[1]; // Green Value in hexadecimal
    rgbValue[2] = pixel[2]; // Blue Value in hexadecimal
    //printf("Values are: 0x%02x 0x%02x 0x%02x\n",(unsigned char)pixel[0],(unsigned char)pixel[1],(unsigned char)pixel[2]);
    return rgbValue;    
    
}


void displayImage(char *fileName1,char *fileName2)
{
    /*
     This function display the two images in separate windows specified by the user.
     @input: "*filename1" - is the fileName of the first ppm file 
             "*filename2" - is the  the filename of the second ppm file
     @output: the images in the "fileName1.ppm"  and "fileName2.ppm" are displayed in two separate
              windows by the user. 
     @return: none
     @precondition: (1) fileName1 and fileName2 references to valid PPM files(including the header contents)
                    (2) getPixels() and readHeader() functions exist
    
     USAGE TIP: Must press "q" via keyboard to close the image window.
     
     Acknowledgement: This function is based on the given lecture codes in Week7: 
                      hello-pixel.c and hello-events.c, which these code were adapted 
                      from http://lazyfoo.net/tutorials/SDL/01_hello_SDL */

    
    FILE  *ptrOne = fopen(fileName1,"rb");// Open the Given File
    FILE  *ptrTwo = fopen(fileName2,"rb");// Open the Given File
    if(ptrOne==NULL || ptrTwo==NULL) // Deal with unsucessful opening of a file.
    {
        fprintf(stderr,"ERROR:displayImage() - You are trying to display image(i.e invalid filename) that does not exist\n");
        exit(-1);
    }

    // Skip over the Header Lines. Position the file pointer to the binary section.
    //dimChan[0] = width  dimChan[1] = height dimChan[2] = max channel value.
    int *dimChanOne = read_header(ptrOne); // For FileName1
    int *dimChanTwo = read_header(ptrTwo); // For FileName2
    
    
    
    

    printf("File %s has Width: %d Height: %d\n",fileName1,dimChanOne[0],dimChanOne[1]);
    printf("File %s has Width: %d Height: %d\n",fileName2,dimChanTwo[0],dimChanTwo[1]);

	SDL_Window *windowOne = NULL; // Create a Window pointed to NULL.
	SDL_Surface *screenSurfaceOne = NULL; // Surface of WindowOne
    SDL_Window *windowTwo = NULL; // Create a  Window pointed to NULL.
	SDL_Surface *screenSurfaceTwo = NULL; // Surface of WindowTwo
    
    
    /*  Processes BELOW:
     *  Initalisation of SDL, Creation of Window, Creating the Image(pixel by pixel), Displaying the Image
     *
     */
    //CONDITION 1: Failed initalisation FUNCTION of SDL Program. - 
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
		printf("ERROR:displayImage() - SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
    //CONDITION 2: Successful initalisation of SDL.
	else 
    {
        //SDL_CreateWindow(window_title,x_position_of_window,y_position_of_window,screen_width,screen_height,show)
        //Create the Window for fileName1
		windowOne = SDL_CreateWindow(fileName1,0, 0,
			dimChanOne[0], dimChanOne[1],SDL_WINDOW_SHOWN);
        //Create the Window for fileName2
		windowTwo = SDL_CreateWindow(fileName2,SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			dimChanTwo[0], dimChanTwo[1],SDL_WINDOW_SHOWN);

        
        
        //CONDITION 1: Window failed to be created.
		if (windowOne == NULL || windowTwo == NULL) 
        {
			printf("ERROR:Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
        //CONDITION 2: Window successfully created.
		else 
        {
			screenSurfaceOne = SDL_GetWindowSurface(windowOne); //Extract Screen Surface and write to it.
            screenSurfaceTwo = SDL_GetWindowSurface(windowTwo); //Extract Screen Surface and write to it.


            //Cover the Surfaces with Color White RGB, OxFF = 0
			SDL_FillRect(screenSurfaceOne, NULL, SDL_MapRGB(screenSurfaceOne->format, 0xff, 0xff, 0xff));
            SDL_FillRect(screenSurfaceTwo, NULL, SDL_MapRGB(screenSurfaceTwo->format, 0xff, 0xff, 0xff));


            
            // Declare variables for the loop.
            int x; // x-direction
			int y; // y-direction
            

            // For FileName1 - Draw Pixels
            for(y =0 ; y< dimChanOne[1]; y++)
			{
                for(x =0 ; x < dimChanOne[0]; x++)
				{
                        char *rgbValue = getPixel(ptrOne); // Get a RGB Value
                        // Determine location to draw pixel.   
			            int *p = screenSurfaceOne->pixels + y * screenSurfaceOne->pitch + x * screenSurfaceOne->format->BytesPerPixel;
                        // Draw the single pixel based on given rgba values stored in rgbValue.
                        *p=SDL_MapRGB(screenSurfaceOne->format, rgbValue[0], rgbValue[1], rgbValue[2]);
                        free(rgbValue); // Free dynamically-allocated memory
				}
			}
            
            
			SDL_UpdateWindowSurface(windowOne); // Display the image of fileName1
            
            // For FileName2 0 Draw Pixels
            for(y =0 ; y< dimChanTwo[1]; y++)
			{
                for(x =0 ; x < dimChanTwo[0]; x++)
				{
                        char *rgbValue = getPixel(ptrTwo); // Get a RGB Value
                        // Determine location to draw pixel.   
			            int *p = screenSurfaceTwo->pixels + y * screenSurfaceTwo->pitch + x * screenSurfaceTwo->format->BytesPerPixel;
                        // Draw the single pixel based on given rgba values stored in rgbValue.
                        *p=SDL_MapRGB(screenSurfaceTwo->format, rgbValue[0], rgbValue[1], rgbValue[2]);
                        free(rgbValue); // Free dynamically-allocated memory
				}
			}
            
            
            
            SDL_UpdateWindowSurface(windowTwo); // Display the image of fileName2

            

		}

        // Wait for Keypress of "q" by USER to close both of the windows 
        SDL_Event event;
        while(SDL_WaitEvent(&event))  //Wait Forever for the next event.
        {
            switch(event.type)
            {
                //USER PRESSES ANY KEY DOWN.!!
                case SDL_KEYDOWN:
                    // IF User press "q" then the program's window is closed.
                    if(event.key.keysym.sym == SDLK_q) // SDLK_ESCAPE = ESC

                    {
                        SDL_DestroyWindow(windowOne); // Close the WindowOne.
                        SDL_DestroyWindow(windowTwo); // Close the WindowOne.
                        SDL_Quit();
                    }
                    break; 
                    
            }
        }
        

	}
    
    // Close the File Connections
    fclose(ptrOne);     // Close the File Connection of fileName1
    fclose(ptrTwo);     // Close the File Connection



}

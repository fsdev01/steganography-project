Assignment TWO - Vinh Huynh

CONTENTS OF THIS FILE
---------------------
 * Introduction
 * Important Changes
 * How to compile
 * Make file commands
 * How to run
 * Functionality
 * Limitations and Known Bugs

INTRODUCTION
------------
Steganography means “hidden writing”. The program’s aim is to encode a secret message and decode a secret message in an image file with a ppm file format. 
We hide our messages in the least significant bit of each colour – the rightmost bit.


IMPORTANT CHANGES 
-----------------
This version adds new functionality including:
 - Ability to display the before & after image, when a secret message is encoded by the hide() program.
 - Allows parallel execution of encoding messages.
 - Allow longer messages to be stored in multiple files

INPUT: When a secret message is entered from the keyboard, the user must press "CRTL-D" when he/she is finished with typing the message to send a EOF signal. 
       Pressing "enter" will no longer be interpreted as end of message. "CRTL-D" means Hold CRTL and press D.

HOW TO BUILD
------------
Requirements: 
 * Ubuntu Linux Operating System
 * libsdl2 library package installed to display images.

Step (1) Unzip the archive file “25155881.tar.gz” to your chosen directory.
Step (2) Open the terminal and change to the directory containing the extracted files from archive.
Step (3) Type into the terminal: make all
(e.g. Type in the text within enclosed quotes “make all” into the terminal)

Now you should have the following programs(executables) in the directory :
 * hide
 * unhide

Also, the directory also contain the following files:
(1)PPM FILES:
 * sample.ppm is an image with NO hidden message - 400(width) x 200 (height) px
 * kittens-000.ppm, kittens-001.ppm, kittens-002.ppm used for testing Task 1 and
   Task 2 of Assignment 2.

(2)TEXT FILES
 * list1.txt used for testing parallel execution (./hide -p list1.txt) - Task 2
 * msg1.txt, msg2.txt, msg3.txt used for testing parallel execution.  - Task 2
 * alice.txt is first 3 chapters of the "Alice's Adventures in Wonderland" ebook
   used for testing Task 1 of Assignment 2(i.e longer messages in multiple files).


(3)OTHER FILES:
 * other.c and other.h
 * readHeader.c and readHeader.h
 * display.c  and display.h


HOW TO RUN
------------
There exist two programs called “hide” and “unhide” with optional flags that
trigger a variety of functionality.

*HIDE PROGRAM
NOTE: When a secret message is entered from the keyboard, the user MUST PRESS "CRTL-D" when
he/she is finished typing the message to send a EOF signal.

(1) Standard Usage with no flags:
    Encodes a message using ONE image file.
    The standard usage is: ./hide inputFileName.ppm  outputFileName.ppm
     - parameter 1: “inputFileName.ppm” is the original image that 
                    does not hold the message.

    -  parameter 2: “outputFileName.ppm” is the image that holds the message.
    
     Try the following:
     Testing Example: ./hide sample.ppm output.ppm < msg1.txt
                      ./unhide output.ppm 

     When a secret message is entered from the keyboard, the user must press "CRTL-D" 
     when he/she is finished with typing the message to send a EOF signal.
     "CRTL-D" means Hold CRTL and press D.
 

(2) Optional Flag: -m
    Encodes a single message across multiple ppm images if required.
     The standard usage is: ./hide -m number-of-files basename output-basename
     - parameter 1: “number-of-files” is number of files provided by user in
                     current directory, with the “basename”.(i.e the number 
                     of files available that can be used to store the message).
                     (e.g cat-000.ppm & cat-001.ppm means the “number-of-files”
		      is equal to 2).

     - parameter 2: “basename” the base name of the image files.
                     (i.e basename=“input”, so the files given by the user
		          are “input-000.ppm input-001.ppm input-002.ppm …” 
                          which # of files depend on “number-of-files” parameter.

     - parameter 3: “output-basename” the base name of the output image files,
	             that the user wishes to store the secret message.
                     (e.g If “ouput-basename”=“output”, the files generated
		     by hide() include “output-000.ppm output-001.ppm” …. )
		     Note: As per Assignment Task 1 Notes section, if the message
 	             requires fewer than the “number-of-files” files, hide()
                     will not produce all the corresponding files, only those
                     that have been used for the secret message.

     Try the following: 
     Testing Example: ./hide -m 3 kittens meow < alice.txt
                      ./unhide -m meow > copiedAlice.txt
     Then compared the text files b/w  "alice.txt" and "copiedAlice.txt".

     When a secret message is entered from the keyboard, the user must press "CRTL-D" 
     when he/she is finished with typing the message to send a EOF signal.
     "CRTL-D" means Hold CRTL and press D.
 
    


(3) Optional Flag: -p
    Allows parallel execution to encode multiple messages at the same time.
    Only supports up to 50 processes(i.e 50 lines in the given “file”.)
    The standard usage is: “./hide -p file”
    - parameter 1: “file” is the file(text file) that contains a list of
                    lines, where each line contains the names of the message file, 
                    input image file and output file.
                    (e.g messagefile.txt  inputImage.ppm outputImage.ppm)
                    
                    Each line in the “file” must end with “\n” character.
                    The filenames don’t have “whitespaces” and are separated
                    by a “single space”.
                    
		    The length of each filename(i.e message file, input image 
                    file and output file) must be < 40 characters(including the
                    file extension). 
    
    Try the following:
    Testing Example: ./hide -p list1.txt
                     ./unhide k1.ppm
                     ./unhide k2.ppm
                     ./unhide k3.ppm
	

(4) Optional Flag: -s
    Allows the “before” and “after” images to be displayed, after the
    the secret message has been encoded.The purpose of this, is to show
    that there are no visible changes to the image, regardless of a 
    secret message being encoded in the output file.
    The standard usage is: “./hide -s inputFile.ppm outputFile.ppm”
    - parameter 1: “inputFileName.ppm” is the original image that 
                    does not hold the message.

    - parameter 2: “outputFileName.ppm” is the image that holds the message.

    Two Images will be displayed in two separate windows:
     (1) one window located at the left-top corner of the screen and 
     (2) another located at the centre of the screen.
    The user can rearrange the positions of the windows, to get
    a side-by-side view, if the images are big and overlap each other.
    Each window has a “window-title” corresponding to the filename of 
    the image that the window is showing(i.e input.ppm output.ppm).

    IMPORTANT: Press “q” via keyboard to close the image windows.

    Try the following:
    Testing Example: ./hide -s sample.ppm output.ppm
                    (Press "-q" to CLOSE THE IMAGE WINDOWS)


    

*UNHIDE PROGRAM

(1) Standard Usage with no flags:
    Decodes the message from a ONE image file.
    The standard usage is: ./unhide inputFileName.ppm  
    - parameter 1: “inputFileName.ppm” is the ppm file with encoded message.

     Try the following:
     Testing Example: ./hide sample.ppm output.ppm < msg1.txt
                      ./unhide output.ppm 

(2) Optional Flag: -m
    Decodes the message from multiple files generated by the “hide -m”
    function. 
    The standard usage is: ./unhide -m output-basename.ppm  
    - parameter 1: “output-basename.ppm” refers to the base name of the 
                   set of files that contain the secret message.
                   (e.g. if “output-basename” equals to “output”,
  	             the directory will contain the following files
                     “output-000.ppm output-001.ppm output-002.ppm”)
     
     Try the following: 
     Testing Example: ./hide -m 3 kittens meow < alice.txt
                      ./unhide -m meow > copiedAlice.txt
     Then compared the text files b/w  "alice.txt" and "copiedAlice.txt".
 
  
 

 

MAKE FILE COMMANDS
—————————————————-
*Make clean:
 Removes all the executables from the directory(i.e hide and unhide).

*Make all:
 Compiles the executables for hide and unhide.

*Make hide:
 Compiles the executable for hide.

*Make unhide:
 Compiles the executable for unhide.



FUNCTIONALITY
-------------
Note: Refer to “How to Run” section for a detailed usage
guide of the optional flags for the hide() and unhide() 
functionality.


 * HIDE FUNCTIONALITY
   The hide function, hides a message in a ppm image.
   - The secret message is entered either via keyboard 
     or by redirection from a text file.

   - hide() allows the user to display the before and after
     images using the -s flag, after a secret message has
     been encoded.Two images are displayed at different locations 
     on the screen and are shown with the relevant 
     window titles. Press “q” to close the windows.

   - hide() also enables the user to encode several
     messages at once to different files using the -p
     flag (i.e parallel execution).

   - hide() gives the user ability to encode longer secret
     messages by using multiple files provided by the user.
     This ability is provided via -m flag. The program does
     this by, using all of pixels available in one-image to
     store part of message, before storing the remaining in
     the other given files.Each message stored across 
     multiple files, ends with “\0” terminator character.

   - The “hide” function will check whether the specified
     secret message can fit into the given input ppm file.
     

*  UNHIDE FUNCTIONALITY
   The unhide function, unhides a secret message in a ppm image.

   - Check whether the magic number is “P6”.

   - Display the secret message to the screen.(terminal)

   - The “unhide” function has the optional flag ‘-m’ to 
     support output files generated, when the hide()
     function encodes a message across images.
 
     




Limitations and Known Bugs 
---------------------------

 * LIMITATIONS IN THE “HIDE” PROGRAM

   - The length of the secret message entered by the user is limited 
     by a certain size (i.e. 4 million characters).

   - The comments in the original input ppm file will not be
     copied over to the output file.

   - Assumes the dimension and channel values are valid integers
     and provided in the correct order.

   - Does not check whether the ppm image’s binary section is
     partially corrupted. 

   - Does not support any combination of the -m,-p and -s options
     at the same time.(consistent to Assignment 2 Task 3 Instructions).

   - Assumes the “file” given to the flag “-p” of hide() contain valid commands.
     (i.e 3 arguments per line of the file in the format of: 
           “message input.ppm output.ppm”).

   - For the “-p” flag of hide(), the program only supports parallel
     execution up to 50 processes. (i.e only the first 50 lines of 
     the file.

   - For the “-p” flag of hide(), the filename of each argument in a given
     line within the “file” has a 100 character limit. 
     (e.g 100 character limit(incl. file extension) for each argument: 
       “message.txt inputImage.ppm outputImage.ppm” 
        100 character limit on the filename of the message
        100 char(including file extension) limit on the filename of the input.ppm
        100 char(including file extension) limit on the filename of the output.ppm)

   - For the “-p” flag, the filenames(i.e msg input.ppm ouput.ppm) don’t
     contain “white spaces” and are separated by a single “space”.
     (consistent to Assignment 2 Task 2 Instructions).

   - For the “p” flag, each line in the “file” (i.e line with the filenames of
     the message, input ppm and output ppm files) must end WITH A “\n” character.
     (consistent to Assignment 2 Task 2 Instructions).

   - For the “-m” flag, as per Assignment Task 1 Notes section, if the message
     requires fewer than the “number-of-files” files, hide() will not produce 
     all the corresponding files, only those that have been used for 
     the secret message.

   - For the "-m" flag, the images must be able to store at least 1 single
     character(i.e \0) otherwise an error will be raised.
    

    


* LIMITATIONS IN THE “UNHIDE” PROGRAM

   - Assumes that the input file contains a hidden message,
     otherwise an error message is displayed.

   - Assumes the dimension and channel values are valid integers
     and provided in the correct order.

   - For the “-m” flag of unhide(), the function assumes the 
     message is encoded in the correct order.
    (e.g file1.ppm contains 1st part of the message.
         file2.ppm contains the 2nd part of the message.
         and so on. otherwise the message will displayed 
         in the wrong order)

    -Some images that don't have a secret message may 
     displayed garbage, if it contains the "\0" character
     because it signals end of message.

   

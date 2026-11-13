#include <stdint.h>
#include <stdio.h>
#include <string.h>


// indicator h or t takes 20 x 120 pixels. 
// number or symbol takes 60 x 120 pixels 
// symbols are 0 1 2 3 4 5 6 7 8 9 . - *space*
//                        -------------
// position on display:   |h 8 2 . 7  |
// position on display:   |t 3 1 . 8  |
//                        -------------

uint16_t* float_to_pixel_buffer(char character, float value)
{

    // converting numbers to char array for transformation. declaring these arrays
    char value_in_characters[6];
    snprintf(value_in_characters, sizeof(value_in_characters), "%.1f", value);

    // value_in_characters[5] is the last index.


    static uint16_t buffer[320 * 120]; 
    memset(&buffer, 0, 320*120*2); // declaring empty buffer


    // for drawing in 2d with 1d object, i will use the formula for the pixel massive index.
    // index = x + y * display_width

    if (character == 'h')
    {
        // drawing 'h' symbol from 0,0 to 20,120
        for (int i = 2; i <= 18; i++) // this draws stripes along x. i represents x here
        {
            buffer[i + 55 * 320] = 0xffff;
            buffer[i + 56 * 320] = 0xffff;
        }

        for (int i = 10; i <= 110; i++) // this draws stripes along y. i represents y here
        {
            buffer[2 + i * 320] = 0xffff;
            buffer[3 + i * 320] = 0xffff;
            buffer[18 + i * 320] = 0xffff;
            buffer[19 + i * 320] = 0xffff;
        }
    } 

    else // if its not h, then its t 
    {
        // drawing 't' symbol from 0,120 to 20,240
        for (int i = 2; i <= 18; i++) // along x, i is x
        {
            buffer[i + 115 * 320] = 0xffff;
            buffer[i + 114 * 320] = 0xffff;
        }
        for (int i = 10; i <= 110; i++) // along y, i is y
        {
            buffer[9 + i * 320] = 0xffff;
            buffer[10 + i * 320] = 0xffff;
        }
    };
        

    
        
    for(int i = 0; i < strlen(value_in_characters); i++ )
    {
        int x = 20 + i*60;

        switch (value_in_characters[i])
        {
        case '0':
            // drawing symbol 0 with following parameters
            // starting with x = 20 + position * 60 and y = y
            // add 20 to account for chunk taken by humidity and temp symbols
            // multiply position by 60 because each position moves x by 60
            // for y - it's either 0 or 120
            //
            // here we always step back a couple units from edge
            // index formula is x + y * display_width
            // all of that we discussed earlier

            for (int i = x + 4; i <= x + 56; i++) // this draws stripes along x
            {
                buffer[i + 110 * 320] = 0xffff;
                buffer[i + 109 * 320] = 0xffff; // top stripe

                buffer[i + 10 * 320] = 0xffff;
                buffer[i + 9 * 320] = 0xffff; // bottom stripe
            }

            for (int i = 10; i <= 110; i++) // this draws stripes along y
            {
                buffer[x + 4 + i * 320] = 0xffff;
                buffer[x + 5 + i * 320] = 0xffff; // left stripe

                buffer[x + 57 + i * 320] = 0xffff;
                buffer[x + 58 + i * 320] = 0xffff; // right stripe
            }
        break;


        case '1':
            for (int i = 10; i <= 110; i++) // this draws stripes along y
            {
                buffer[x + 29 + i * 320] = 0xffff;
                buffer[x + 30 + i * 320] = 0xffff;
                buffer[x + 31 + i * 320] = 0xffff;
                buffer[x + 32 + i * 320] = 0xffff;  // stick
            }
        break;


        case '2':
            for (int i = x + 4; i <= x + 56; i++) // this draws stripes along x
            {
                buffer[i + 110 * 320] = 0xffff;
                buffer[i + 109 * 320] = 0xffff; // top stripe

                buffer[i + 60 * 320] = 0xffff;
                buffer[i + 59 * 320] = 0xffff; // middle stripe

                buffer[i + 10 * 320] = 0xffff;
                buffer[i + 9 * 320] = 0xffff; // bottom stripe
            }

            for (int i = 60; i <= 110; i++) // this draws stripes along y
            {
                buffer[x + 57 + i * 320] = 0xffff;
                buffer[x + 58 + i * 320] = 0xffff; //right half-stick
            }

            for (int i = 10; i <= 60; i++) // this draws stripes along y
            {
                buffer[x + 3 + i * 320] = 0xffff;

                buffer[x + 2 + i * 320] = 0xffff; // left half-stick
            }
        break;


        case '3':

            for (int i = x + 4; i <= x + 56; i++) //this draws stripes along x
            {
                buffer[i + 110 * 320] = 0xffff;
                buffer[i + 109 * 320] = 0xffff; // top stripe

                buffer[i + 60 * 320] = 0xffff;
                buffer[i + 59 * 320] = 0xffff; // middle stripe

                buffer[i + 10 * 320] = 0xffff;
                buffer[i + 9 * 320] = 0xffff; //bottom stripe
            }

            for (int i = 10; i <= 110; i++) // this draws stripes along y
            {
                buffer[x + 57 + i * 320] = 0xffff;
                buffer[x + 58 + i * 320] = 0xffff; //right stick
            }
        break;


        case '4':

            for (int i = x + 4; i <= x + 56; i++) // this draws stripes along x
            {
                buffer[i + 60 * 320] = 0xffff;
                buffer[i + 59 * 320] = 0xffff; // middle stripe
            }

            for (int i = 10; i <= 110; i++) // this draws stripes along y
            {
                buffer[x + 57 + i * 320] = 0xffff;
                buffer[x + 58 + i * 320] = 0xffff; // right stick
            }

            for (int i = 60; i <= 110; i++) // this draws stripes along y
            {
                buffer[x + 3 + i * 320] = 0xffff;
                buffer[x + 2 + i * 320] = 0xffff; // left half-stick
            }
        break;


        case '5':

            for (int i = x + 4; i <= x + 56; i++) // this draws stripes along x
            {
                buffer[i + 110 * 320] = 0xffff;
                buffer[i + 109 * 320] = 0xffff; // top stripe

                buffer[i + 60 * 320] = 0xffff;
                buffer[i + 59 * 320] = 0xffff; // middle stripe

                buffer[i + 10 * 320] = 0xffff;
                buffer[i + 9 * 320] = 0xffff; // bottom stripe
            }

            for (int i = 60; i <= 110; i++) // this draws stripes along y
            {
                buffer[x + 3 + i * 320] = 0xffff;
                buffer[x + 2 + i * 320] = 0xffff; // right half-stick
            }

            for (int i = 10; i <= 60; i++) // this draws stripes along y
            {
                buffer[x + 57 + i * 320] = 0xffff;
                buffer[x + 58 + i * 320] = 0xffff; // left half-stick
            }
        break;


        case '6':

            for (int i = x + 4; i <= x + 56; i++) // this draws stripes along x
            {
                buffer[i + 110 * 320] = 0xffff;
                buffer[i + 109 * 320] = 0xffff; // top stripe

                buffer[i + 60 * 320] = 0xffff;
                buffer[i + 59 * 320] = 0xffff; // middle stripe

                buffer[i + 10 * 320] = 0xffff;
                buffer[i + 9 * 320] = 0xffff; // bottom stripe
            }

            for (int i = 10; i <= 60; i++) // this draws stripes along y
            {
                buffer[x + 57 + i * 320] = 0xffff;
                buffer[x + 58 + i * 320] = 0xffff; // right half-stick
            }

            for (int i = 10; i <= 110; i++) // this draws stripes along y
            {
                buffer[x + 3 + i * 320] = 0xffff;
                buffer[x + 2 + i * 320] = 0xffff; // left stick
            }
        break;


        case '7':

            for (int i = x + 4; i <= x + 56; i++) // x
            {
                buffer[i + 110 * 320] = 0xffff;
                buffer[i + 109 * 320] = 0xffff; // top stripe
            }

            for (int i = 10; i <= 110; i++) // y
            {
                buffer[x + 57 + i * 320] = 0xffff;
                buffer[x + 58 + i * 320] = 0xffff; // right stick
            }
        break;


        case '8':

            for (int i = x + 4; i <= x + 56; i++) // x
            {
                buffer[i + 110 * 320] = 0xffff;
                buffer[i + 109 * 320] = 0xffff; // top stripe

                buffer[i + 60 * 320] = 0xffff;
                buffer[i + 59 * 320] = 0xffff; // middle stripe

                buffer[i + 10 * 320] = 0xffff;
                buffer[i + 9 * 320] = 0xffff; // bottom stripe
            }

            for (int i = 10; i <= 110; i++) // y
            {
                buffer[x + 57 + i * 320] = 0xffff;
                buffer[x + 58 + i * 320] = 0xffff; // right stick

                buffer[x + 3 + i * 320] = 0xffff;
                buffer[x + 2 + i * 320] = 0xffff; // left stick
        }
        break;


        case '9':
            for (int i = x + 4; i <= x + 55; i++) // this draws stripes along x
            {
                buffer[i + 110 * 320] = 0xffff;
                buffer[i + 109 * 320] = 0xffff; // top stripe

                buffer[i + 60 * 320] = 0xffff;
                buffer[i + 59 * 320] = 0xffff; // middle stripe
            }

            for (int i = 10; i <= 110; i++) // this draws stripes along y
            {
                buffer[x + 57 + i * 320] = 0xffff;
                buffer[x + 58 + i * 320] = 0xffff; // right stick
            }

            for (int i = 60; i <= 110; i++) // this draws stripes along y
            {
                buffer[x + 3 + i * 320] = 0xffff;
                buffer[x + 2 + i * 320] = 0xffff; // left half-stick
            }
        break;


        case '-':
            for (int i = x + 4; i <= x + 56; i++) // x
            {
                buffer[i + 60 * 320] = 0xffff;
                buffer[i + 59 * 320] = 0xffff; //  middle stripe
            }
        break;


        case '.':
            for (int i = x + 53; i <= x + 58; i++) // x
            {
                buffer[i + 9 * 320] = 0xffff;
                buffer[i + 10 * 320] = 0xffff;
                buffer[i + 11 * 320] = 0xffff;
                buffer[i + 12 * 320] = 0xffff;
            }
        break;


        case ' ':
        break;
        };
    }




    return buffer;

};
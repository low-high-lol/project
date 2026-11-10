#include <stdint.h>
#include <stdio.h>
#include <string.h>


// indicator h or t takes 20 x 120 pixels. 
// number or symbol takes 60 x 120 pixels 
// symbols are 0 1 2 3 4 5 6 7 8 9 . - *space*

// position argument:     -1 0 1 2 3 4 
//                        -------------
// position on display:   |h 8 2 . 7  |
// position on display:   |t 3 1 . 8  |
//                        -------------
// position argument:      6 7 8 9 10 11


void add_buffer_symbol(uint16_t* buffer, char character, int8_t position)
{

    int x = (position < 5) ? (20 + position * 60) : (20 + (position - 7) * 60);
    int y = (position < 5) ? (0) : (120);

    // if position is < 5 - its in upper row
    // if position is > 5 - its in bottom row - we substract 7 because we use the 1-2-3-4-5 in our calulations for each row
    // we add 20 to nivelate the -1 and 6 positions - they are reserved for H and T, that takes 20 on x
    // and we multiply position on 6 because each new position moves x on 60

    // for y: if x < 0, we should start with y = 0. if x>5 - with y = 120
    // its the difference between upper and bottom rows



    // for drawing in 2d with 1d object, i will use the formula for the pixel massive index.
    // index = x + y * display_width

    switch (character)
    {


    case 'h':
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
        break;


    case 't':
        // drawing 't' symbol from 0,120 to 20,240
        for (int i = 2; i <= 18; i++) // along x, i is x
        {
            buffer[i + 230 * 320] = 0xffff;
            buffer[i + 229 * 320] = 0xffff;
        }
        for (int i = 130; i <= 230; i++) // along y, i is y
        {
            buffer[9 + i * 320] = 0xffff;
            buffer[10 + i * 320] = 0xffff;
        }
        break;


    case '0':
        // drawing symbol 0 with following parameters
        // начало с x = 20 + position * 60 и y = y 
        // starting with x = 20 + position * 60 and y = y
        // add 20 to account for chunk taken by humidity and temp symbols
        // multiply position by 60 because each position moves x by 60
        // for y - it's either 0 or 120
        //
        // here we always step back a couple units from edge
        // index formula is x + y * display_width
        // all of that we discussed earlier

        for (int i = x + 2; i <= x + 58; i++) // this draws stripes along x
        {
            buffer[i + (y + 110) * 320] = 0xffff;
            buffer[i + (y + 109) * 320] = 0xffff; // top stripe

            buffer[i + (y + 10) * 320] = 0xffff;
            buffer[i + (y + 9) * 320] = 0xffff; // bottom stripe
        }

        for (int i = y + 10; i <= y + 110; i++) // this draws stripes along y
        {
            buffer[x + 2 + i * 320] = 0xffff;
            buffer[x + 3 + i * 320] = 0xffff; // left stripe

            buffer[x + 57 + i * 320] = 0xffff;
            buffer[x + 58 + i * 320] = 0xffff; // right stripe
        }
        break;


    case '1':
        for (int i = y + 10; i <= y + 110; i++) // this draws stripes along y
        {
            buffer[x + 29 + i * 320] = 0xffff;
            buffer[x + 30 + i * 320] = 0xffff;
            buffer[x + 31 + i * 320] = 0xffff; // stick
        }
        break;


    case '2':
        for (int i = x + 2; i <= x + 58; i++) // this draws stripes along x
        {
            buffer[i + (y + 110) * 320] = 0xffff;
            buffer[i + (y + 109) * 320] = 0xffff; // top stripe

            buffer[i + (y + 60) * 320] = 0xffff;
            buffer[i + (y + 59) * 320] = 0xffff; // middle stripe

            buffer[i + (y + 10) * 320] = 0xffff;
            buffer[i + (y + 9) * 320] = 0xffff; // bottom stripe
        }

        for (int i = y + 60; i <= y + 110; i++) // this draws stripes along y
        {
            buffer[x + 57 + i * 320] = 0xffff;
            buffer[x + 58 + i * 320] = 0xffff; //right half-stick
        }

        for (int i = y + 10; i <= y + 60; i++) // this draws stripes along y
        {
            buffer[x + 3 + i * 320] = 0xffff;

            buffer[x + 2 + i * 320] = 0xffff; // left half-stick
        }
        break;


    case '3':

    for (int i = x + 2; i <= x + 58; i++) //this draws stripes along x
        {
            buffer[i + (y + 110) * 320] = 0xffff;
            buffer[i + (y + 109) * 320] = 0xffff; // top stripe

            buffer[i + (y + 60) * 320] = 0xffff;
            buffer[i + (y + 59) * 320] = 0xffff; // middle stripe

            buffer[i + (y + 10) * 320] = 0xffff;
            buffer[i + (y + 9) * 320] = 0xffff; //bottom stripe
        }

        for (int i = y + 10; i <= y + 110; i++) // this draws stripes along y
        {
            buffer[x + 57 + i * 320] = 0xffff;
            buffer[x + 58 + i * 320] = 0xffff; //right stick
        }
        break;


    case '4':

        for (int i = x + 2; i <= x + 58; i++) // this draws stripes along x
        {
            buffer[i + (y + 60) * 320] = 0xffff;
            buffer[i + (y + 59) * 320] = 0xffff; // middle stripe
        }

        for (int i = y + 10; i <= y + 110; i++) // this draws stripes along y
        {
            buffer[x + 57 + i * 320] = 0xffff;
            buffer[x + 58 + i * 320] = 0xffff; // right stick
        }

        for (int i = y + 60; i <= y + 110; i++) // this draws stripes along y
        {
            buffer[x + 3 + i * 320] = 0xffff;
            buffer[x + 2 + i * 320] = 0xffff; // left half-stick
        }
        break;


    case '5':

        for (int i = x + 2; i <= x + 58; i++) // this draws stripes along x
        {
            buffer[i + (y + 110) * 320] = 0xffff;
            buffer[i + (y + 109) * 320] = 0xffff; // top stripe

            buffer[i + (y + 60) * 320] = 0xffff;
            buffer[i + (y + 59) * 320] = 0xffff; // middle stripe

            buffer[i + (y + 10) * 320] = 0xffff;
            buffer[i + (y + 9) * 320] = 0xffff; // bottom stripe
        }

        for (int i = y + 60; i <= y + 110; i++) // this draws stripes along y
        {
            buffer[x + 3 + i * 320] = 0xffff;
            buffer[x + 2 + i * 320] = 0xffff; // right half-stick
        }

        for (int i = y + 10; i <= y + 60; i++) // this draws stripes along y
        {
            buffer[x + 57 + i * 320] = 0xffff;
            buffer[x + 58 + i * 320] = 0xffff; // left half-stick
        }
        break;


    case '6':

        for (int i = x + 2; i <= x + 58; i++) // this draws stripes along x
        {
            buffer[i + (y + 110) * 320] = 0xffff;
            buffer[i + (y + 109) * 320] = 0xffff; // top stripe

            buffer[i + (y + 60) * 320] = 0xffff;
            buffer[i + (y + 59) * 320] = 0xffff; // middle stripe

            buffer[i + (y + 10) * 320] = 0xffff;
            buffer[i + (y + 9) * 320] = 0xffff; // bottom stripe
        }

        for (int i = y + 10; i <= y + 60; i++) // this draws stripes along y
        {
            buffer[x + 57 + i * 320] = 0xffff;
            buffer[x + 58 + i * 320] = 0xffff; // right half-stick
        }

        for (int i = y + 10; i <= y + 110; i++) // this draws stripes along y
        {
            buffer[x + 3 + i * 320] = 0xffff;
            buffer[x + 2 + i * 320] = 0xffff; // left stick
        }
        break;


    case '7':

        for (int i = x + 2; i <= x + 58; i++) // x
        {
            buffer[i + (y + 110) * 320] = 0xffff;
            buffer[i + (y + 109) * 320] = 0xffff; // top stripe
        }

        for (int i = y + 10; i <= y + 110; i++) // y
        {
            buffer[x + 57 + i * 320] = 0xffff;
            buffer[x + 58 + i * 320] = 0xffff; // right stick
        }
        break;


    case '8':

        for (int i = x + 2; i <= x + 58; i++) // x
        {
            buffer[i + (y + 110) * 320] = 0xffff;
            buffer[i + (y + 109) * 320] = 0xffff; // top stripe

            buffer[i + (y + 60) * 320] = 0xffff;
            buffer[i + (y + 59) * 320] = 0xffff; // middle stripe

            buffer[i + (y + 10) * 320] = 0xffff;
            buffer[i + (y + 9) * 320] = 0xffff; // bottom stripe
        }

        for (int i = y + 10; i <= y + 110; i++) // y
        {
            buffer[x + 57 + i * 320] = 0xffff;
            buffer[x + 58 + i * 320] = 0xffff; // right stick

            buffer[x + 3 + i * 320] = 0xffff;
            buffer[x + 2 + i * 320] = 0xffff; // left stick
        }
        break;


    case '9':
        for (int i = x + 2; i <= x + 58; i++) // this draws stripes along x
        {
            buffer[i + (y + 110) * 320] = 0xffff;
            buffer[i + (y + 109) * 320] = 0xffff; // top stripe

            buffer[i + (y + 60) * 320] = 0xffff;
            buffer[i + (y + 59) * 320] = 0xffff; // middle stripe
        }

        for (int i = y + 10; i <= y + 110; i++) // this draws stripes along y
        {
            buffer[x + 57 + i * 320] = 0xffff;
            buffer[x + 58 + i * 320] = 0xffff; // right stick
        }

        for (int i = y + 60; i <= y + 110; i++) // this draws stripes along y
        {
            buffer[x + 3 + i * 320] = 0xffff;
            buffer[x + 2 + i * 320] = 0xffff; // left half-stick
        }
        break;


    case '-':
        for (int i = x + 2; i <= x + 58; i++) // x
        {
            buffer[i + (y + 60) * 320] = 0xffff;
            buffer[i + (y + 59) * 320] = 0xffff; //  middle stripe
        }
        break;


    case '.':
        for (int i = x + 53; i <= x + 58; i++) // x
        {
            buffer[i + (y + 9) * 320] = 0xffff;
            buffer[i + (y + 10) * 320] = 0xffff;
            buffer[i + (y + 11) * 320] = 0xffff;
            buffer[i + (y + 12) * 320] = 0xffff;
        }
        break;


    case ' ':
        break;
    }
};





uint16_t* float_to_pixel_buffer(float humidity, float temperature)

{

    //
    // converting numbers to char array for transformation. declaring these arrays
    char humidity_in_characters[6];
    char temperature_in_characters[6];
    
    snprintf(humidity_in_characters, sizeof(humidity_in_characters), "%.1f", humidity);
    snprintf(temperature_in_characters, sizeof(temperature_in_characters), "%.1f", temperature);

    // 
    // now we pass these chars into loop that will make a section in buffer with drawn symbol out of each char
    
    // humidity in characters[3] - last index. 
    // temperature in characters[4] last index.
    //
    // iterating from -1 - to place humidity or temp symbols at -1 and start going through array elements from zero

    static uint16_t buffer[320 * 240]; 
    memset(&buffer, 0, 320*240*2); // declaring empty buffer


    
    for (int8_t i = -1; i < 13 ; i ++) // there are 12 of them: 2 are humidity or temp symbols. 10 - the temp numbers themselves. 1 - end of array /0
    // only one array end because the program wont reach the end of the second array
    // -1 symbol  0 H_digit 1 H_digit 2 H_digit 3 H_digit 4 H_digit 5 /0end    6 symbol  7 T_digit 8 T_digit 9 T_digit 10 T_digit  11 T_digit
    {
        if(i < 5) // up to 5 is humidity
        {
            if(i == -1)
            {
                add_buffer_symbol(buffer, 'h', i);
                continue;
            }
            add_buffer_symbol(buffer, humidity_in_characters[i], i); // this function and symbols should be moved into number_symbol
            continue;
        };

        if(i > 5) // after 5 is temperature. ignoring five, because under humidity_in_characters[5] will be /0
        {
            if(i == 6)
            {
                add_buffer_symbol(buffer, 't', i);
                continue;
            }
            add_buffer_symbol(buffer, temperature_in_characters[i-7], i); // here its i-7 because when i gets here it will be 7, but we want to start with index 0 of the array
        };
    };

    return buffer ;
};
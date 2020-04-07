#include <WiegandNG.h>
#include <stdio.h>
#include <string.h>

#define MAX_CHAR_ARRAY 80

WiegandNG wg;
char ch = '0';
char CardNumber[MAX_CHAR_ARRAY];
int arraylocation = 0;


//String stringOne = "hello"; //Test var

void PrintBinary(WiegandNG tempwg) {
        volatile unsigned char *buffer=tempwg.getRawData();
        unsigned int bufferSize = tempwg.getBufferSize();
        unsigned int countedBits = tempwg.getBitCounted();

        unsigned int countedBytes = (countedBits/8);
        if ((countedBytes % 8)>0) countedBytes++;
        unsigned int bitsUsed = countedBytes * 8;

        for (int i=bufferSize-countedBytes; i< bufferSize;i++) {
                unsigned char bufByte=buffer[i];
                for(int x=0; x<8;x++) {
                        if ( (((bufferSize-i) *8)-x) <= countedBits) {
                                if((bufByte & 0x80)) {
                                        if (ch == '1')
                                        {
                                            Serial.print("1");
                                            CardNumber[arraylocation]='1';
                                            CardNumber[arraylocation + 1]='\0';
                                            arraylocation++;
                                        }
                                        else {
                                            Serial.print("X");
                                        }
                                }
                                else {
                                       if (ch == '1')
                                       {
                                            Serial.print("0");
                                            CardNumber[arraylocation]='0';
                                            CardNumber[arraylocation + 1]='\0';
                                            arraylocation++;
                                       }
                                       else {
                                            Serial.print("X");
                                       }
                                }
                        }

                        bufByte<<=1;
                }
        }

        Serial.println();
}

void setup() {
        Serial.begin(9600);

        // initialize Wiegand ND for 34 bit data, every 8 bits take up 1 byte of Arduino memory
        // as long as there is still memory, user can even capture 1024 bit Wiegand by calling begin(1024)
        if(!wg.begin(1024)) {
                Serial.println("Out of memory!");
        }
        Serial.println("         Menu         ");
        Serial.println("----------------------");
        Serial.println("1) Print cleartext");
        Serial.println("2) Mask card number");
        Serial.println("");

        while ((ch != '1' ) && (ch != '2'))
        {
          ch = Serial.read();
        }

          Serial.print("You selected: ");
          Serial.println(ch);
          Serial.println();

}


void loop() {
        if(wg.available()) {
                arraylocation = 0;

                wg.pause();                               // pause Wiegand pin interrupts
                Serial.print("Bits=");
                Serial.println(wg.getBitCounted());       // display the number of bits counted
                Serial.print("RAW Binary=");
                PrintBinary(wg);                          // display raw data in binary form, raw data inclusive of PARITY

                if (ch == '1')
                {
                        Serial.print("Full card number = ");
                        for (int b=0; b < wg.getBitCounted(); b++)
                                Serial.print(CardNumber[b]);
                        Serial.println();

                        //Serial.print("Full card number = ");
                        //Serial.println(CardNumber);
                }

                wg.clear();                               // compulsory to call clear() to enable interrupts for subsequent data
        }

}

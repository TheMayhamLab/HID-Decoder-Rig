/*
 * Name:      HID-Card-Reader
 * Purpose:   Rig for testing HID cards and implantable chips
 * By:        Michael Vieau (The Mayhem Lab)
 * Created:   2020.04.18
 * Modified:  2002.04.20
 * Rev Level  0.2
 * 
 * References
 * Code based off of sample by Daniel Smith (www.pagemac.com)
 * https://www.arduino.cc/reference/en/language/structure/bitwise-operators/bitshiftleft/
 * https://www.arduino.cc/reference/en/language/structure/compound-operators/compoundbitwiseor/
 *
 * Your reader should have at least 4 connections (some readers have more).  Connect the Red wire 
 * to 5V.  Connect the black to ground.  Connect the green wire (DATA0) to Digital Pin 2 (INT0).  
 * Connect the white wire (DATA1) to Digital Pin 3 (INT1).  That's it!
*/

#include <stdio.h>
#include <string.h>

#define MAX_BITS 100                  // max number of bits 
#define WEIGAND_WAIT_TIME  3000       // time to wait for another weigand pulse.  

unsigned char bitCount;               // number of bits currently captured
unsigned long cardCode=0;             // decoded card code
unsigned long TAGID=0;                // Full cardID as seen with a Proxmark3
unsigned char databits[MAX_BITS];     // stores all of the data bits
unsigned long facilityCode=0;         // decoded facility code
unsigned char flagDone;               // goes low when data is currently being captured
unsigned int weigand_counter;         // countdown until we assume there are no more bits

char ch = '1';                        // Set this to a '1' to bypass the menu and make it "just work"
int DOOR_PIN = 12;
unsigned char i;                      // General counter

// List of codes that will open the door.
long Validcodes[] = { 56138, 1234 };           
int NumValidcodes = 2;                // Number of codes in the above array

// ====================================================================================
void setup()
{
  pinMode(2, INPUT);          // DATA0 (INT0)
  pinMode(3, INPUT);          // DATA1 (INT1)
  pinMode(12, OUTPUT);        // DOOR
  pinMode(13, OUTPUT);        // LED
  
  Serial.begin(9600);
 
  // binds the ISR functions to the falling edge of INTO and INT1
  attachInterrupt(0, ISR_INT0, FALLING);  
  attachInterrupt(1, ISR_INT1, FALLING);
  
  weigand_counter = WEIGAND_WAIT_TIME;

  menu();
}
// ====================================================================================
void loop()
{
  // This waits to make sure that there have been no more data pulses before processing data
  if (!flagDone) {
    if (--weigand_counter == 0)
      flagDone = 1;	
  }
  
  // if we have bits and we the weigand counter went out
  if (bitCount > 0 && flagDone) {
    
    Serial.println("");
    Serial.print("Bits ========== ");
    Serial.println(bitCount);
    //-------------
    //  26 bits
    //-------------
    if (bitCount == 26)
    {
      // facility
      for (i=1; i<9; i++)
      {
        facilityCode <<=1;            // Used for bit shifting
        facilityCode |= databits[i];  // Compound bitwise OR operator
      }
      
      // card code
      for (i=9; i<25; i++)
      {
         cardCode <<=1;
         cardCode |= databits[i];
      }
      // TAG ID
      for (i=1; i<26; i++)
      {
        TAGID <<=1;
        TAGID |= databits[i];
      }
      printBits();  
    }
    //-------------
    // 33 bits  **** Not working correctly ****
    //-------------
    else if (bitCount == 33)
    {  
      // facility code
      for (i=0; i<7; i++)
      {
         facilityCode <<=1;
         facilityCode |= databits[i];
      }
      
      // card code
      for (i=8; i<25; i++)
      {
         cardCode <<=1;
         cardCode |= databits[i];
      }
      // TAG ID
      for (i=1; i<26; i++)
      {
        TAGID <<=1;
        TAGID |= databits[i];
      }
      printBits();
    }
    //-------------
    //  35 bits
    //-------------
    else if (bitCount == 35)
    {  
      // facility code = bits 2 to 14
      for (i=2; i<14; i++)
      {
         facilityCode <<=1;
         facilityCode |= databits[i];
      }
      
      // card code = bits 15 to 34
      for (i=14; i<34; i++)
      {
         cardCode <<=1;
         cardCode |= databits[i];
      }
      // TAG ID
      for (i=1; i<26; i++)
      {
        TAGID <<=1;
        TAGID |= databits[i];
      }
      printBits();
    }
    //-------------
    //  37 bits
    //-------------
    else if (bitCount == 37)
    {
      // card code
      for (i=2; i<20; i++)
      {
         cardCode <<=1;
         cardCode |= databits[i];
      }
      
      // facility code
      for (i=20; i<26; i++)
      {
         facilityCode <<=1;
         facilityCode |= databits[i];
      }
      // TAG ID
      for (i=1; i<26; i++)
      {
        TAGID <<=1;
        TAGID |= databits[i];
      }
      printBits();
    }
    else 
    {
     Serial.println("Decoding not avalible for this bit length"); 
    }
     // cleanup and get ready for the next card
     bitCount = 0;
     facilityCode = 0;
     cardCode = 0;
     TAGID = 0;
     for (i=0; i<MAX_BITS; i++) 
     {
       databits[i] = 0;
     }
  }
}
// ====================================================================================
void menu()
{
  Serial.println("  HID testing rig");
  Serial.println("By: The Mayhem Lab");
  Serial.println(" ");
  Serial.println("         Menu         ");
  Serial.println("----------------------");
  Serial.println("1) Print cleartext");
  Serial.println("2) Mask card number");
  Serial.println("");

  while ((ch != '1' ) && (ch != '2'))
  {
    ch = Serial.read();
  }

  if (ch == '1')
  {
    Serial.println("I will show you all the dataz");
  }
  else
  {
    Serial.println("Card data will be masked for security");
  }
  Serial.println(" ");
  
}
// ====================================================================================
// interrupt that happens when INTO goes low (0 bit)
void ISR_INT0()
{
  if (ch == '1')
  {
    Serial.print("0");
    bitCount++;
    flagDone = 0;
    weigand_counter = WEIGAND_WAIT_TIME;  
  }
  else 
  {
    Serial.print("X");
  }
}
// ====================================================================================
// interrupt that happens when INT1 goes low (1 bit)
void ISR_INT1()
{
  if (ch == '1')
  {
    Serial.print("1");
    databits[bitCount] = 1;
    bitCount++;
    flagDone = 0;
    weigand_counter = WEIGAND_WAIT_TIME;  
  }
  else
  {
    Serial.print("X");
  }
}
// ====================================================================================
void printBits()
{
  //Serial.println(sizeof(TAGID, BIN));
  Serial.print("TAG ID ======== ");
  Serial.println(TAGID);
  Serial.print("Facility Code = ");
  Serial.println(facilityCode);
  Serial.print("Card Number === ");
  Serial.println(cardCode);
  Serial.println(" " );
  login();
}
// ====================================================================================
void login()
{
  for (i=0; i<NumValidcodes; i++)
  {
    // Check to see if the card opens the door
    if (cardCode == Validcodes[i])
    {
      digitalWrite(DOOR_PIN, HIGH);
      Serial.println("Opening door");
      delay(2000);
      digitalWrite(DOOR_PIN, LOW);
      Serial.println("Locking door");
      Serial.println("");
    }
  }
}

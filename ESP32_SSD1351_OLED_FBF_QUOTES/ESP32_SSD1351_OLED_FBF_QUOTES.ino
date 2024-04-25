//FBF (Foul Bachelor Frog) Meme Displayer for ESP32/ESP8266 and other internet of things nerd devices
//Uses the Adafruit GFX library and SPI connectivity to the microcontroller
//
//Video tutorial here: TBC
//
//If you're using on something other than an ESP32 then the random number generator will need replacing
//If using a screen other than a TFT/OLED that's 128x128 pixels then the positioning of the text will
//need altering. To make a larger background image replace the image data in imageData.h
//Use LCD image converter to replace the background image. Tutorial here: https://youtu.be/nVLaM5JgG9w
//Watch out for screen burn if displaying FBF for an extended period of time on an OLED!

//Define constants
#define imageWidth 128
#define imageHeight 128
#define MAX_TEXT_CHARACTER_LENGTH 20
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128

//Libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <bootloader_random.h>
#include <esp_random.h>
#include "imageData.h"

//These connections must stay like this due to use of hardware SPI
#define SCLK_PIN GPIO_NUM_18 // CLK (or use constant SCK)
#define MOSI_PIN GPIO_NUM_23 // DIN (or use constant MOSI)

//These connections can be changed to other pins if desired
#define DC_PIN   GPIO_NUM_2
#define CS_PIN   GPIO_NUM_0
#define RST_PIN  GPIO_NUM_4

//Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

//Initialise TFT/OLED display
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);

//C++ class for Memes
class Meme {
  public:
  const char* headerText;
  const char* footerText;
    Meme() { }
    Meme(const char h[], const char f[]) { // Constructor with parameters
        headerText = h;
       footerText = f; 
      }
};

//Memes assumed to have top text and bottom text
enum TextPlacement { top, bottom };

//This is most of the FBF memes (the ones that will fit on the screen anyway)
//Some have also been shortened so they will fit
//If you add more memes remember to change the size of the array from 60 to the new size
//Also remember to keep text below the value of MAX_TEXT_CHARACTER_LENGTH
//Template for adding more memes: {"", ""},
Meme memes[60] {
  //<----MAX LENGTH---->
  {"FIX LIFE", "TOMORROW"},
  {"FIX LIFE", "NEXT WEEK"},
  {"FINISH WORKOUT", "CLEAR BROWSE HISTORY"},
  {"ORDER PIZZA", "EXTRA NAPKINS"},
  {"4:23AM", "ONE MORE EPISODE"},
  {"GIRL NICE TO YOU", "IMAGINE LIFE TOGETHER"},
  {"BLOW LOAD", "CLOSE 31 BROWSER TABS"},
  {"BUY FRUIT", "NEVER EAT FRUIT"},
  {"TAKE QUICK NAP", "WAKE UP NEXT MORNING"},
  {"MICROWAVE EXPLODES", "DINNER'S READY"},
    
  {"NO CLEAN PLATES", "USE CLIPBOARD"},
  {"TO LAZY TO COOK", "STARVE"},
  {"LEAVE BED", "GO LIE ON COUCH"},
  {"TEETH DIRTY", "SCRAPE WITH FINGERNAIL"},
  {"OUT OF SHAMPOO", "USE SOAP INSTEAD"},
  {"GIRL OVER", "USE EDGE INSTEAD"},
  {"TOILET CLOGGED?", "PEE IN SINK"},
  {"DENTISTS TODAY", "TIME TO FLOSS"},
  {"DUE TOMORROW?", "DO TOMORROW"},
  {"OPEN CAN OF PRINGLES", "THROW AWAY LID"},

  {"DIRTY PILLOW CASE", "FLIP PILLOW OVER"},
  {"OUT OF PLATES?", "USE MICROWAVE'S PLATE"},
  {"FINISHED CAN OF SODA", "NEW ASHTRAY"},
  {"DID LAUNDRY", "PRODUCTIVE DAY"},
  {"ROOMMATE MOVES OUT", "CAN FAP ANYWHERE"},
  {"NO TOOTHPASTE", "CHEW GUM INSTEAD"},
  {"SPILLS WATER", "LET IT EVAPORATE"},
  {"NO CLEAN KNIVES", "USE SPOON HANDLE"},
  {"MCDONALDS BAG", "TEMPORARY TRASHCAN"},
  {"NO NUT NOVEMBER?", "TOO LATE"},

  {"NO PLANS?", "NO SHOWER"},
  {"ACNE ON CHIN", "GROW BEARD"}, //ISSUE
  {"SHAVER BROKEN", "GROW BEARD"},
  {"TERRIBLE BEDHEAD", "WEAR HAT"},
  {"DISGUSTED WITH SELF", "FAP"},
  {"RESUME CLASSES", "RESUME HYGIENE"},
  {"BREAKFAST", "BEER & STRING CHEESE"},
  {"OUT OF CLEAN PLATES", "BUY NEW PLATES"},
  {"THE FLOOR", "THE LARGEST SHELF"},
  {"NO CLEAN PLATES", "TUPPERWEAR LID"},

  {"OVERSLEEP", "FOR 4PM CLASS"},
  {"AWAKE AT 9AM", "DAY FEELS INFINITE"},
  {"BLOWN LIGHTBULB", "STOP USING THAT ROOM"},
  {"NO MILK", "VODKA CEREAL"},
  {"TRASH FULL", "BALANCE MORE ON TOP"},
  {"GET MARRIED", "TO REM FROM RE:ZERO"},
  {"GET HOME", "REMOVE PANTS"},
  {"STALE CEREAL", "EAT IT ANYWAY"},
  {"SPILL DRINK", "FIND NEAREST SOCK"},
  {"CAN'T GET GIRL", "FIND 150 POKEMON"},

  {"TIME FOR BREAKFAST", "IT'S 1PM"},
  {"THING FALLS OFF COUCH", "IT'S GONE"},
  {"POSSIBLE TRAP?", "FAP ANYWAY"},
  {"HOT NEW STEPSISTER", "MOVE BACK IN"},
  {"5 PUSHUPS", "EYE OF THE TIGER"},
  {"WATCH STAR TREK", "ASTROPHYSICIST"},
  {"15 MINUTES LATE", "FOR EVERYTHING"},
  {"EAT GUMMI BEARS", "CONSIDER IT FRUIT"},
  {"FORMAT HDD", "BRAVE NEW ERA"},
  {"RAINING OUTSIDE", "NO SCREEN GLARE"}
};

//Initialise meme variables
Meme memeToDisplay = memes[random(sizeof(memes) / sizeof(Meme))];
Meme previousMeme = memeToDisplay;

void setup(void) {
  Serial.begin(115200);
  Serial.println("ESP32 is starting up");

  bootloader_random_enable(); //ESP32 random generator if wifi/bluetooth is disabled
  randomSeed(esp_random());

  Serial.println("TFT screen initialising");
  tft.begin();
  
  //Uncomment this if you want to rotate the screen
  //tft.setRotation(1);
}

void loop() {

  //Choose a meme. It should be different to the previous one and also must have text of less than the maximum characters length
  do {
    memeToDisplay = memes[random(sizeof(memes) / sizeof(Meme))];
  } while (memeToDisplay.headerText == previousMeme.headerText &&
    (strlen(memeToDisplay.headerText) > MAX_TEXT_CHARACTER_LENGTH || strlen(memeToDisplay.footerText) > MAX_TEXT_CHARACTER_LENGTH)
    );

  //Uncomment for debugging purposes if you want to always display a particular meme
  //The first meme in the memes array is [0]
  //memeToDisplay = memes[0];

  //Screen needs to be cleared before new stuff is output to the display
  tft.fillScreen(BLACK);

  //This renders the FBF background to the display (x, y, imageData, width, height)
  tft.drawRGBBitmap(0, 0, fbfImageData, imageWidth, imageHeight);
  
  tft.setTextColor(WHITE);

  //You could potentially display longer meme text by using the text wrapping feature
  tft.setTextWrap(false);
  
  //Place the meme labels on top of the FBF image
  placeText(top);
  placeText(bottom);

  //Keep a track of the current meme so we don't choose it next time around
  previousMeme = memeToDisplay;
  
  //Sleep for 10 seconds
  delay(5000);

}

//Places the meme text on the screen
//3 different font sizes are used
//The Adafruit default fonts are very pixely so you could replace them with better fonts
void placeText(TextPlacement placement) {

  byte textLength = strlen(memeToDisplay.headerText);
  if (placement == bottom) {
    textLength = strlen(memeToDisplay.footerText);
  }

  byte textScreenWidth;
  byte characterWidth = 12;
  byte xPosAdjustment = 0;
  byte yPosFooterAdjustment = 0;

  Serial.print("Text to place: ");
  if (placement == top) {
    Serial.print(memeToDisplay.headerText);
  } else {
    Serial.print(memeToDisplay.footerText);
  }
  Serial.print(" Length: ");
  Serial.println(textLength);
  
  byte xPos = 0;
  byte yPos = 6;
  
  //Largest font
  if (textLength <= 7) {
    tft.setTextSize(3);
    characterWidth = 17;
    xPosAdjustment = 0;
    Serial.println("Set font size to 3 (large)");
  }

  //Medium sized font
  if (textLength >= 8 && textLength < 11) {
    tft.setTextSize(2);
    characterWidth = 12;
    xPosAdjustment = 2;
    Serial.println("Set font size to 2 (medium)");
  }  

  //Smallest font
  if (textLength >= 11) {
    tft.setTextSize(1);
    characterWidth = 6;
    xPosAdjustment = 0;
    yPosFooterAdjustment = 5;
    Serial.println("Set font size to 1 (small)");
  }

  tft.setCursor(xPos, yPos);
  
  if (placement == bottom) {
    yPos = 104 + yPosFooterAdjustment;
  }

  //Center the text on the display
  textScreenWidth = characterWidth * textLength;
  xPos = ((SCREEN_WIDTH / 2) - (textScreenWidth / 2)) + xPosAdjustment;

  //Move cursor to where the text should be displayed
  tft.setCursor(xPos, yPos);

  //Actually render the text onto the display
  if (placement == top) {
    tft.print(memeToDisplay.headerText);
  } else {
    tft.print(memeToDisplay.footerText);
  }

}
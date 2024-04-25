//Courage Wolf Meme Displayer for ESP32/ESP8266 and other internet of things nerd devices
//Uses the Adafruit GFX library and SPI connectivity to the microcontroller
//
//Video tutorial here: https://youtu.be/aBMWbegtDcs
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

//If you add more memes remember to change the size of the array from 60 to the new size
//Also remember to keep text below the value of MAX_TEXT_CHARACTER_LENGTH
//Template for adding more memes: {"", ""},
Meme memes[45] {
  //<----MAX LENGTH---->
  {"GOD GAVE U LEMONS?", "FIND NEW GOD"},
  {"SHOW HER", "YOUR BALLS"},
  {"THIRSTY?", "DRINK TESTOSTERONE"},
  {"FOREVERALONE?", "A LONE WOLF"},
  {"PAIN IS TEMPORARY", "GLORY IS ETERNAL"},
  {"EAT ICECREAM", "IN DECEMBER"},
  {"WEAR SHORTS", "IN JANUARY"},
  {"FIND THE LINE", "AND CROSS IT"},
  {"PAIN?", "I LOVE PAIN"},
  {"DO MATH HOMEWORK", "IN PEN"},

  {"NO JOB?", "START YOUTUBE CHANNEL"},
  {"YOU HAVE", "NO LIMITS"},
  {"PAIN IS WEAKNESS", "LEAVING THE BODY"},
  {"HAVE ENEMIES?", "GOOD!"},
  {"EAT PROBLEMS", "SHIT SUCCESS"},
  {"LATE FOR WORK", "ASK FOR RAISE"},
  {"CLIMB A MOUNTAIN", "TELL NOBODY"},
  {"GO TO DENTIST", "EAT CANDY"},
  {"I DO NOT FAIL", "NEITHER DO YOU"},
  {"COWARDS", "ARE FOR BREAKFAST"},

  {"GO AHEAD", "BE THE ALPHA MALE"},
  {"THE BEST WAY OUT", "IS THROUGH"},
  {"PANIC ATTACK?", "ATTACK PANIC FIRST"},
  {"CALL 911", "THEY NEED MY HELP"},
  {"LENT?", "GIVE UP FEAR"},
  {"GO FISHING", "FOR BEARS!"},
  {"WIN", "THE GAME"},
  {"RAIN & WIND?", "THOR'S ON UR SIDE"},
  {"SHE REJECTED U?", "HER LOSS"},
  {"ATTEND CLASS", "TEACH"},

  {"XMAS SHOPPING", "ON THE 24TH"},
  {"PLAY DUCK HUNT", "SHOOT A BEAR"},
  {"3 VS 1?", "TRIPLE KILL"},
  {"REACH FINISH LINE", "KEEP RUNNING"},
  {"EAT COAL", "SHIT DIAMONDS"},
  {"NO RETREAT", "NO SURRENDER"},
  {"YOU WANNA BE", "THE VERY BEST"},
  {"72HR PROJECT", "CHALLENGE ACCEPTED"},
  {"STOP WHINING", "MAN UP & WIN"},
  {"STOP WHINING", "MAN UP"},

  {"NO JOBS?", "START BUSINESS"},
  {"OUTRUN A CAR", "ON FOOT"},
  {"DON'T SURVIVE", "THRIVE!"},
  {"NOT BORED", "SEND ME NOTHING"},
  {"OUT OF HONEY?", "CHEW BEES!"}
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
  //memeToDisplay = memes[2];

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
  delay(10000);

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
    yPosFooterAdjustment = 8;
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
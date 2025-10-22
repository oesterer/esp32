/*  
 Test the tft.print() viz. embedded tft.write() function

 This sketch used font 2, 4, 7
 
 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 Note that yield() or delay(0) must be called in long duration for/while
 loops to stop the ESP8266 watchdog triggering.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
 */


#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
TFT_eSprite scrollSprite = TFT_eSprite(&tft); // Off-screen buffer for the message

// Parameters used to keep the scrolling message animation state.
const char *kScrollText = "Hello there ESP32 fans!";
extern const GFXfont FreeSansBold24pt7b;
const GFXfont *kScrollFont = &FreeSansBold24pt7b;
int16_t scrollX = 0;
int16_t messageWidth = 0;
int16_t messageHeight = 0;
int16_t spriteWidth = 0;
int16_t spriteHeight = 0;
uint8_t colorIndex = 0;
int8_t bounceOffset = 0;
int8_t bounceDirection = 1;
const int8_t kBounceRange = 25;

uint16_t colorWheel(uint8_t pos) {
  pos = 255 - pos;
  if (pos < 85) {
    return tft.color565(255 - pos * 3, 0, pos * 3);
  }
  if (pos < 170) {
    pos -= 85;
    return tft.color565(0, pos * 3, 255 - pos * 3);
  }
  pos -= 170;
  return tft.color565(pos * 3, 255 - pos * 3, 0);
}

void setup(void) {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextWrap(false, false);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setFreeFont(kScrollFont);

  messageWidth = tft.textWidth(kScrollText);
  messageHeight = tft.fontHeight();
  scrollX = tft.width();

  scrollSprite.setColorDepth(16);
  spriteWidth = messageWidth + 12;
  spriteHeight = messageHeight + 12;
  scrollSprite.createSprite(spriteWidth, spriteHeight);
  scrollSprite.fillSprite(TFT_BLACK);
  scrollSprite.setTextWrap(false);
  scrollSprite.setFreeFont(kScrollFont);
}

void loop() {
  scrollSprite.fillSprite(TFT_BLACK);
  scrollSprite.setTextColor(colorWheel(colorIndex), TFT_BLACK);
  int16_t baseline = spriteHeight - ((spriteHeight - messageHeight) / 2) - 2;
  scrollSprite.setCursor(6, baseline);
  scrollSprite.print(kScrollText);

  int16_t baseY = (tft.height() - spriteHeight) / 2;
  if (baseY < 0) {
    baseY = 0;
  }
  int16_t textY = baseY + bounceOffset;
  if (textY < 0) {
    textY = 0;
  }
  if (textY + spriteHeight > tft.height()) {
    textY = tft.height() - spriteHeight;
  }

  tft.fillRect(0, textY, tft.width(), spriteHeight, TFT_BLACK);

  for (int16_t x = scrollX; x < tft.width(); x += spriteWidth) {
    scrollSprite.pushSprite(x, textY);
  }
  for (int16_t x = scrollX - spriteWidth; x + spriteWidth > 0; x -= spriteWidth) {
    scrollSprite.pushSprite(x, textY);
  }

  scrollX -= 2;
  if (scrollX <= -spriteWidth) {
    scrollX += spriteWidth;
  }

  bounceOffset += bounceDirection;
  if (bounceOffset > kBounceRange) {
    bounceOffset = kBounceRange;
    bounceDirection = -bounceDirection;
  } else if (bounceOffset < -kBounceRange) {
    bounceOffset = -kBounceRange;
    bounceDirection = -bounceDirection;
  }

  colorIndex += 3;

  delay(30);
}

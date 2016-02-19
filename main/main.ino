#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bridge.h>
#include <HttpClient.h>

// If using software SPI (the default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/* Uncomment this block to use hardware SPI
  #define OLED_DC     6
  #define OLED_CS     7
  #define OLED_RESET  8
  Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
*/

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

char uri[] = "http://apidev.accuweather.com/currentconditions/v1/209441.json?language=en&apikey=hoArfRosT1215";

void setup()   {

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  renderDetails("Loading", "...", "...");
}

void renderDetails(const String& place, const String& temperature, const String& forecast){
  // Clear the buffer.
  display.clearDisplay();

  // text display tests
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println(place);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(temperature + " C");
  display.setTextSize(1);
  display.println(forecast);
  display.display();
}

void loop() {
  makeHttpCall();
  delay(30000);
}

void makeHttpCall(){
  
  HttpClient client;
  client.get(uri);
  // example response:
  // "[{\"LocalObservationDateTime\":\"2016-02-12T00:35:00+00:00\",\"EpochTime\":1455237300,\"WeatherText\":\"Vietomis debesuota\",\"WeatherIcon\":35,\"IsDayTime\":false,\"Temperature\":{\"Metric\":{\"Value\":4.6,\"Unit\":\"C\",\"UnitType\":17},\"Imperial\":{\"Value\":40.0,\"Unit\":\"F\",\"UnitType\":18}},\"MobileLink\":\"\",\"Link\":\"\"}]";
  
  String json = "";
  while (client.available()) {
    char in = client.read();
    json += in;
  }

  Serial.println( json );

  String location = "Killarney";
  String k1 = "WeatherText";
  String k2 = "Temperature";
  String k3 = "Value";
  String forecast = printKeyValue(k1, json);
  String tempJson = printKeyValue(k2, json);
  String temp = printKeyValue(k3, tempJson);

  renderDetails(location, temp, forecast);
}

String printKeyValue(const String& k, const String& j){
  Serial.println( "method starts:" + k + ":" + j);
  int keyStartsAt = j.indexOf(k);
  Serial.println( keyStartsAt );
  int keyEndsAt = keyStartsAt + k.length(); // inludes double quote
  Serial.println( keyEndsAt );
  int colonPosition = j.indexOf(":", keyEndsAt);
  int valueEndsAt = j.indexOf(",", colonPosition);
  String val = j.substring(colonPosition + 1, valueEndsAt);
  val.trim();
  Serial.println( val );
  return val;
}

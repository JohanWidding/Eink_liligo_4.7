#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include <stdlib.h>
#include <stdio.h>
#include "epd_driver.h"
#include "epd_driver.h"
#include "firasans.h"
#include "font.h"
#include "font10.h"
#include "yr.h"
#include "energi.h"
#include "ferge.h"
#include "sol.h"
#include "delvis.h"
#include "sky.h"
#include "regn.h"
#include "sno.h"
#include "luftfuktighet.h"
#include "trykk.h"
#include "snow.h"
#include "swim.h"
#include "uv.h"
#include "gust.h"
#include "temperatur.h"
#include "vind.h"
#include "kompass.h"


#if CONFIG_IDF_TARGET_ESP32
#define BATT_PIN            36
#endif

#if defined(CONFIG_IDF_TARGET_ESP32)
#define BUTTON_2            0
#define BUTTON_3            35
#define BUTTON_4            34
#define BUTTON_5            39
#define BUTTON_PIN_BITMASK  0x8000000000 // 2^39 -> Hexadecimal
#else
#error "Platform not supported"
#endif

const char* ssid       = "Altibox BG20";
const char* password   = "Bergen123ABC";

bool didConnect = false;
uint8_t *framebuffer;
int cursor_x = 200;
int cursor_y = 250;
int vref = 1100;
float rainfall_list[20] = {10, 80, 200, 150, 50, 50, 0, 9, 39, 38, 32, 26, 145, 120, 67, 10, 10, 10, 10};
char ukedager[7][10] = {"Sondag", "Mandag", "Tirsdag", "Onsdag", "Torsdag", "Fredag", "Lordag"};
char symboler_dager[9][25];
char following_days[9][10];



int get_wakeup_reason();
void pageONE();
void pageTWO();
void pageTHREE();
void header();
void connect_to_wifi();
void get_data_el();
void get_data_wether();
void get_hour_wether();
void electricity_graph();
void rain_graph();

struct tm timeinfo;
struct tm timeinfo2;
char time_now[40];
char year[40];
char month[40];
char day[40];
char dayofweek[40];
char timeofday[40];

String BORTELID_SNOW;
String KVAM_SNOW;
String TEMPERATUREN_NOW;
String FOLES_SOM;
String LUFTFUKTIGHET;
String TRYKK;
String UVstraalinng;
String BADETEMPERATUREN;
String VINDSTYRKE;
String VINDKASTSTYRKE;

char stromURL[100];
float strom[24];
float regn[20];
float temperatur_dager[9];

void setup()
{
    Serial.begin(115200);

    int pageNum = get_wakeup_reason();

    epd_init();
    epd_poweron();
    epd_clear();
    
    framebuffer = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    epd_poweroff();

    connect_to_wifi();
    if (didConnect){
        epd_poweroff();
    
        if (pageNum == 1) {
            printf("Trykket paa knapp 1\n");
            get_data_wether();
    
            epd_poweron();
            epd_clear();
            header();
            rain_graph();
            pageONE();
            
          }
        else if (pageNum == 2) {
            printf("Trykket paa knapp 2\n");
            get_data_el();
    
            epd_poweron();
            epd_clear();
            header();
            pageTWO();
          }
        else {
            printf("Trykket paa knapp 3\n");
            epd_poweron();
            epd_clear();
            header();
            pageTHREE();
          }
    }
    epd_draw_grayscale_image(epd_full_screen(), framebuffer);
    epd_poweroff();
    epd_poweroff_all();
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_3, LOW);
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ALL_LOW);
    Serial.println("Going to sleep now");
    esp_deep_sleep_start();
}

void loop(){}

// Possible colors are 0xF0 (white) through 0x80 (median gray) til 0x00 (black).
// 540(H)X960(V) resolution



void header() 
{
  cursor_x = 20;
  cursor_y = 45;

  epd_draw_line(0, 50, 960, 50, 0,framebuffer);
  Rect_t area = {
        .x = 210,
        .y = 5,
        .width = yr_width,
        .height = yr_height,
    };
  epd_draw_image(area, (uint8_t *)yr_data, BLACK_ON_WHITE);
  area = {
        .x = 350,
        .y = 5,
        .width = energi_width,
        .height = energi_height,
    };
  epd_draw_image(area, (uint8_t *)energi_data, BLACK_ON_WHITE);
  area = {
        .x = 470,
        .y = 5,
        .width = ferge_width,
        .height = ferge_height,
    };
  epd_draw_image(area, (uint8_t *)ferge_data, BLACK_ON_WHITE);
  
  delay(10); // Make adc measurement more accurate
  uint16_t v = analogRead(BATT_PIN);
  float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
  String voltage = String(battery_voltage) + "V";
  writeln((GFXfont *)&FiraSans, (char *)voltage.c_str(), &cursor_x, &cursor_y, NULL);

  cursor_x = 700;
  cursor_y = 30;
  writeln((GFXfont *)&font10, time_now, &cursor_x, &cursor_y, NULL);
}

void pageONE()
{ 
  const char *SIDE1 = "»";
  const char *SIDE2 = "«";
  cursor_x = 180;
  cursor_y = 40;
  writeln((GFXfont *)&FiraSans, SIDE1, &cursor_x, &cursor_y, NULL);
  cursor_x = 255;
  cursor_y = 40;
  writeln((GFXfont *)&FiraSans, SIDE2, &cursor_x, &cursor_y, NULL);
  const char *REGNRADAR = "Nedborsvarsel de neste 90 minuttene";
  cursor_x = 350;
  cursor_y = 240;
  writeln((GFXfont *)&font10, REGNRADAR, &cursor_x, &cursor_y, NULL);
  epd_draw_rect(0, 50, 351, 540, 0, framebuffer);
  epd_draw_rect(350, 250, 960, 540, 0, framebuffer);
  for (int i = 0; i <20; i++) {
            epd_fill_rect(350+(i*32), 485-rainfall_list[i],32, rainfall_list[i], 0, framebuffer);
            epd_draw_rect(350+(i*32), 485-rainfall_list[i],32, rainfall_list[i], 0xF0, framebuffer);
          }
  for(int i=0;i<7;i++)
  { 
    int minutes = 0+(15*i);
    String all_hours = String(minutes);

    cursor_x = 355+(i*101);
    cursor_y = 520;
    
    if (i == 6) {
      all_hours = String(minutes);
      cursor_x = 935;
      epd_draw_line(945, 490, 945, 503, 0x80,framebuffer);
      }
    
    writeln((GFXfont *)&font10, (char *)all_hours.c_str(), &cursor_x, &cursor_y, NULL);
    if (i == 0) {
      epd_draw_line(362+(i*101), 490, 362+(i*101), 503, 0x80,framebuffer);
      }
    else {
      epd_draw_line(365+(i*101), 490, 365+(i*101), 503, 0x80,framebuffer);
      }
  }
  epd_draw_line(350, 485, 960, 485, 0x80,framebuffer);

  const char* ver_akk_naa = "Veret akkurat na";
  const char* temperatur = "Temperatur";
  const char* badetemperatur = "Badetemp.";
  const char* luftfuktighet = "Luftfuktighet";
  const char* vindstyrke = "Vindstyrke";
  const char* Trykk = "Trykk";
  const char* UV_straling = "UV-straling";
  const char* snodybde = "Snodybde";
  const char* bortelid = "Bortelid";
  const char* kvamskogen = "Kvamskogen";
  const char* bortelid_sno = "300 cm";
  const char* kvamskogen_sno = "500 cm";
  const char* hPa = "hPa";

  epd_draw_line(0, 66, 350, 66, 0x80,framebuffer);
  epd_draw_line(0, 67, 350, 67, 0x80,framebuffer);
  
  epd_draw_line(0, 180, 350, 180, 0x80,framebuffer);
  epd_draw_line(0, 181, 350, 181, 0x80,framebuffer);

  epd_draw_line(0, 290, 350, 290, 0x80,framebuffer);
  epd_draw_line(0, 291, 350, 291, 0x80,framebuffer);

  epd_draw_line(0, 440, 350, 440, 0x80,framebuffer);
  epd_draw_line(0, 439, 350, 439, 0x80,framebuffer);
  
  epd_draw_line(180, 66, 180, 290, 0x80,framebuffer);
  epd_draw_line(181, 66, 181, 290, 0x80,framebuffer);

  epd_draw_line(230, 290, 230, 439, 0x80,framebuffer);
  epd_draw_line(231, 290, 231, 439, 0x80,framebuffer);

  cursor_x = 100;
  cursor_y = 65;
  writeln((GFXfont *)&font10, ver_akk_naa, &cursor_x, &cursor_y, NULL);

  cursor_x = 60;
  cursor_y = 307;
  writeln((GFXfont *)&font10, vindstyrke, &cursor_x, &cursor_y, NULL);

  cursor_x = 240;
  cursor_y = 307;
  writeln((GFXfont *)&font10, badetemperatur, &cursor_x, &cursor_y, NULL);
  cursor_x = 260;
  cursor_y = 410;
  writeln((GFXfont *)&FiraSans, (char *)BADETEMPERATUREN.c_str(), &cursor_x, &cursor_y, NULL);
  Rect_t area = {
        .x = 232,
        .y = 307,
        .width = swim_width,
        .height = swim_height,
    };
  epd_draw_image(area, (uint8_t *)swim_data, BLACK_ON_WHITE);
  


  area = {
        .x = 0,
        .y = 90,
        .width = temperatur_width,
        .height = temperatur_height,
    };
  epd_draw_image(area, (uint8_t *)temperatur_data, BLACK_ON_WHITE);
  cursor_x = 30;
  cursor_y = 85;
  writeln((GFXfont *)&font10, temperatur, &cursor_x, &cursor_y, NULL);
  cursor_x = 110;
  cursor_y = 140;
  writeln((GFXfont *)&FiraSans, (char *)TEMPERATUREN_NOW.c_str(), &cursor_x, &cursor_y, NULL);
  area = {
        .x = 185,
        .y = 90,
        .width = luftfuktighet_width,
        .height = luftfuktighet_height,
    };
  epd_draw_image(area, (uint8_t *)luftfuktighet_data, BLACK_ON_WHITE);
  cursor_x = 215;
  cursor_y = 85;
  writeln((GFXfont *)&font10, luftfuktighet, &cursor_x, &cursor_y, NULL);
  cursor_x = 270;
  cursor_y = 140;
  writeln((GFXfont *)&FiraSans, (char *)LUFTFUKTIGHET.c_str(), &cursor_x, &cursor_y, NULL);
  
   area = {
        .x = 0,
        .y = 200,
        .width = trykk_width,
        .height = trykk_height,
    };
  epd_draw_image(area, (uint8_t *)trykk_data, BLACK_ON_WHITE);
  cursor_x = 60;
  cursor_y = 200;
  writeln((GFXfont *)&font10, Trykk, &cursor_x, &cursor_y, NULL);
  cursor_x = 90;
  cursor_y = 250;
  writeln((GFXfont *)&FiraSans, (char *)TRYKK.c_str(), &cursor_x, &cursor_y, NULL);
  cursor_x = 100;
  cursor_y = 270;
  writeln((GFXfont *)&font10, hPa, &cursor_x, &cursor_y, NULL);
  area = {
        .x = 180,
        .y = 200,
        .width = uv_width,
        .height = uv_height,
    };
  epd_draw_image(area, (uint8_t *)uv_data, BLACK_ON_WHITE);
  cursor_x = 215;
  cursor_y = 200;
  writeln((GFXfont *)&font10, UV_straling, &cursor_x, &cursor_y, NULL);
  cursor_x = 270;
  cursor_y = 250;
  writeln((GFXfont *)&FiraSans, (char *)UVstraalinng.c_str(), &cursor_x, &cursor_y, NULL);
  cursor_x = 50;
  cursor_y = 410;
  writeln((GFXfont *)&FiraSans, (char *)VINDSTYRKE.c_str(), &cursor_x, &cursor_y, NULL);

  
  area = {
        .x = 0,
        .y = 540-snow_height,
        .width = snow_width,
        .height = snow_height,
    };
  epd_draw_image(area, (uint8_t *)snow_data, BLACK_ON_WHITE);

  cursor_x = 20;
  cursor_y = 455;
  writeln((GFXfont *)&font10, snodybde, &cursor_x, &cursor_y, NULL);

  cursor_x = 130;
  cursor_y = 560-snow_height;
  writeln((GFXfont *)&font10, bortelid, &cursor_x, &cursor_y, NULL);
  cursor_x = 230;
  writeln((GFXfont *)&font10, kvamskogen, &cursor_x, &cursor_y, NULL);
  cursor_y = cursor_y +30;
  cursor_x = 140;
  writeln((GFXfont *)&font10, (char *)BORTELID_SNOW.c_str(), &cursor_x, &cursor_y, NULL);
  cursor_x = 240;
  writeln((GFXfont *)&font10, (char *)KVAM_SNOW.c_str(), &cursor_x, &cursor_y, NULL);

  
  
  for (int i = 0; i <6; i++) {
    area = {
        .x = 350+(i*101),
        .y = 60,
        .width = 100,
        .height = 100,
    };
    cursor_x = 366+(i*101);
    cursor_y = 70;
    writeln((GFXfont *)&font10, following_days[i], &cursor_x, &cursor_y, NULL);
    cursor_x = 375+(i*101);
    if (temperatur_dager[i]<9.5) {cursor_x = cursor_x + 10;}
    cursor_y = 200;
    char temp[25];
    sprintf(temp, "%f", temperatur_dager[i]);
    String TEMPERATUR = String(atoi(temp)) + "°";
    writeln((GFXfont *)&FiraSans, (char *)TEMPERATUR.c_str(), &cursor_x, &cursor_y, NULL);

    epd_draw_rect(350+(i*101), 50, 100, 155, 0, framebuffer);
    const char* symbol =symboler_dager[i];
    if (strcmp(symbol, "clearsky_day") == 0) {
        epd_draw_image(area, (uint8_t *)sol_data, BLACK_ON_WHITE);
      }
    else if (strcmp(symbol , "partlycloudy_day") == 0 || strcmp(symbol , "fair_day") == 0){
        epd_draw_image(area, (uint8_t *)delvis_data, BLACK_ON_WHITE);
      }
    else if (strcmp(symbol , "lightsleet") == 0|| strcmp(symbol , "sleet") == 0|| strcmp(symbol , "heavysleet") == 0 || strcmp(symbol , "lightrain") == 0 || strcmp(symbol , "rain") == 0 || strcmp(symbol , "heavyrain") == 0) {
        epd_draw_image(area, (uint8_t *)regn_data, BLACK_ON_WHITE);
      }
    else if (strcmp(symbol , "lightsnow") == 0|| strcmp(symbol , "snow") == 0|| strcmp(symbol , "heavysnow") == 0) {
        epd_draw_image(area, (uint8_t *)sno_data, BLACK_ON_WHITE);
      }
    else {
      epd_draw_image(area, (uint8_t *)sky_data, BLACK_ON_WHITE);
      }
  }


}
void pageTWO()
{
  const char *SIDE1 = "»";
  const char *SIDE2 = "«";
  cursor_x = 320;
  cursor_y = 40;
  writeln((GFXfont *)&FiraSans, SIDE1, &cursor_x, &cursor_y, NULL);
  cursor_x = 395;
  cursor_y = 40;
  writeln((GFXfont *)&FiraSans, SIDE2, &cursor_x, &cursor_y, NULL);
  epd_draw_rect(100,80, 860, 150, 0x80, framebuffer);
  epd_draw_rect(100,230, 860, 150, 0x80, framebuffer);
  epd_draw_rect(100,380, 860, 70, 0x80, framebuffer);
  for(int i=0;i<24;i++)
  { 
    epd_draw_line(100+(i*37), 65, 100+(i*37), 450, 0x80,framebuffer);
  }
  electricity_graph();
  for(int i=0;i<24;i++)
  { 
    String all_hours = String(i)+":00";
    if (i < 10) {all_hours = "0"+String(i)+":00";}
    cursor_x = 75+(i*37);
    if(i % 2 == 0)
        cursor_y = 490;
    else
        cursor_y = 520;
    writeln((GFXfont *)&font10, (char *)all_hours.c_str(), &cursor_x, &cursor_y, NULL);
  }
  
}
void pageTHREE()
{
  const char *SIDE1 = "»";
  const char *SIDE2 = "«";
  cursor_x = 440;
  cursor_y = 40;
  writeln((GFXfont *)&FiraSans, SIDE1, &cursor_x, &cursor_y, NULL);
  cursor_x = 560;
  cursor_y = 40;
  writeln((GFXfont *)&FiraSans, SIDE2, &cursor_x, &cursor_y, NULL);
}

void get_data_el()
{
if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    
    HTTPClient http;
    //-----------------------------------------------------------------------
    http.begin("https://www.timeapi.io/api/Time/current/zone?timeZone=Europe/Oslo"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
    if (httpCode > 0) { //Check for the returning code
        Serial.println("https://www.timeapi.io/api/Time/current/zone?timeZone=Europe/Oslo");
  
        String payload = http.getString();

        DynamicJsonDocument doc(1024);
      
        // Parse JSON object        StaticJsonDocument<200> doc;
        deserializeJson(doc, payload);
        strptime(doc["dateTime"], "%FT%TZ", &timeinfo);
        strftime(time_now,40, "%d.%m.%Y %H:%M", &timeinfo);
        strftime(year,40, "%Y", &timeinfo);
        strftime(month,40, "%m", &timeinfo);
        strftime(day,40, "%d", &timeinfo);
        strftime(timeofday,40, "%H:%M", &timeinfo);

        String new_url;
        if ((atoi(day)<10) && (atoi(month) < 10)) {
          new_url = "https://www.hvakosterstrommen.no/api/v1/prices/" + String(atoi(year))+ "/0"+ String(atoi(month))+ "-0"+ String(atoi(day)) + "_NO5.json";
          }
        else if (atoi(day)<10) {
          new_url = "https://www.hvakosterstrommen.no/api/v1/prices/" + String(atoi(year))+ "/"+ String(atoi(month))+ "-0"+ String(atoi(day)) + "_NO5.json";
          }
        else if (atoi(month)<10) {
          new_url = "https://www.hvakosterstrommen.no/api/v1/prices/" + String(atoi(year))+ "/0"+ String(atoi(month))+ "-"+ String(atoi(day)) + "_NO5.json";
          }
        else {
          new_url = "https://www.hvakosterstrommen.no/api/v1/prices/" + String(atoi(year))+ "/"+ String(atoi(month))+ "-"+ String(atoi(day)) + "_NO5.json";
          }
        HTTPClient http;
        http.begin(new_url); //Specify the URL
        httpCode = http.GET(); 
        if (httpCode > 0) { //Check for the returning code
            Serial.println(new_url);
            String payload = http.getString();
    
            DynamicJsonDocument doc(3500);
          
            // Parse JSON object        StaticJsonDocument<200> doc;
            deserializeJson(doc, payload);
            for (int i = 0; i <24; i++) {
              strom[i] = doc[i]["NOK_per_kWh"];
          }
          }
      }
       
}
}
///Denne funksjonen er ikke i bruk pga den ikke fungerer (Minneproblermer)
void get_hour_wether() {
  HTTPClient http;
  WiFiClient client;

  http.begin("https://www.yr.no/api/v0/locations/10-1047305/forecast"); //Specify the URL
  int httpCode = http.GET();  
  
  if (httpCode > 0) { //Check for the returning code
      Serial.println("hour forcast success");

      String payload = getValue(http, "shortIntervals", 3, 59152);
      
      DynamicJsonDocument doc(49152);

      deserializeJson(doc, payload);

      for (int i = 0; i <9; i++) {
        const char* symbol = doc[i]["temperature"]["value"];
        Serial.println(symbol);
      
      }
  }  
}

void get_data_wether()
{
if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    
    HTTPClient http;
    WiFiClient client;
    //-----------------------------------------------------------------------
    http.begin("https://www.timeapi.io/api/Time/current/zone?timeZone=Europe/Oslo"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
    if (httpCode > 0) { //Check for the returning code
        Serial.println("https://www.timeapi.io/api/Time/current/zone?timeZone=Europe/Oslo");
  
        String payload = http.getString();

        DynamicJsonDocument doc(1024);
      
        // Parse JSON object        StaticJsonDocument<200> doc;
        deserializeJson(doc, payload);
        strptime(doc["dateTime"], "%FT%TZ", &timeinfo);
        strftime(time_now,40, "%a   %d.%m.%Y   %H:%M", &timeinfo);
        strftime(year,40, "%Y", &timeinfo);
        strftime(month,40, "%m", &timeinfo);
        strftime(day,40, "%d", &timeinfo);
        strftime(dayofweek,40, "%w", &timeinfo);
        strftime(timeofday,40, "%H:%M", &timeinfo);

        HTTPClient http;
        http.begin("https://www.yr.no/api/v0/locations/10-1047305/forecast/now"); //Specify the URL
        httpCode = http.GET(); 
        if (httpCode > 0) { //Check for the returning code
            Serial.println("https://www.yr.no/api/v0/locations/10-1047305/forecast/now    -> Regnmengde");
            
            String payload = http.getString();
    
            DynamicJsonDocument doc(3500);
          
            // Parse JSON object        StaticJsonDocument<200> doc;
            deserializeJson(doc, payload);
            for (int i = 0; i <19; i++) {
              float downfall = doc["points"][i]["precipitation"]["intensity"];
              downfall = downfall * 10;
              regn[i] = downfall;
          }
          }
        http.begin("https://www.yr.no/api/v0/locations/5-41550/observations"); //Specify the URL
        httpCode = http.GET(); 
        if (httpCode > 0) { //Check for the returning code
            Serial.println("https://www.yr.no/api/v0/locations/5-41550/observations       -> Ljosland snødybde");
            
            String payload = http.getString();
    
            DynamicJsonDocument doc(12072);
          
            // Parse JSON object        StaticJsonDocument<200> doc;
            deserializeJson(doc, payload);
            float snow = doc["_embedded"]["stations"][0]["observations"]["snow"]["depth"];
            float temperature = doc["_embedded"]["stations"][0]["observations"]["temperature"]["intervals"][0]["value"];
            char string[25];
            sprintf(string, "%f", snow);
            KVAM_SNOW = String(atoi(string)) + " cm";
            Serial.println(snow);
            Serial.println(temperature);
          }
        http.begin("https://www.yr.no/api/v0/locations/5-51250/observations"); //Specify the URL
        httpCode = http.GET(); 
        if (httpCode > 0) { //Check for the returning code
            Serial.println("https://www.yr.no/api/v0/locations/5-51250/observations   -> Øvstedal snødybde");
            
            String payload = http.getString();
    
            DynamicJsonDocument doc(12072);
          
            // Parse JSON object        StaticJsonDocument<200> doc;
            deserializeJson(doc, payload);
            float snow = doc["_embedded"]["stations"][0]["observations"]["snow"]["depth"];
            float temperature = doc["_embedded"]["stations"][0]["observations"]["temperature"]["intervals"][0]["value"];
            char string[25];
            sprintf(string, "%f", snow);
            BORTELID_SNOW = String(atoi(string)) + " cm";
            Serial.println(snow);
            Serial.println(temperature);
          }
        http.begin("https://www.yr.no/api/v0/locations/10-1047305/nearestwatertemperatures?language=nb"); //Specify the URL
        httpCode = http.GET(); 
        if (httpCode > 0) { //Check for the returning code
            Serial.println("https://www.yr.no/api/v0/locations/10-1047305/nearestwatertemperatures?language=nb    -> Badetemperatur");
            
            String payload = http.getString();
    
            DynamicJsonDocument doc(12072);
          
            // Parse JSON object        StaticJsonDocument<200> doc;
            deserializeJson(doc, payload);
            float badetemperatur = doc["_embedded"]["nearestLocations"][0]["temperature"];
            bool isInOcean = doc["_embedded"]["nearestLocations"][0]["location"]["isInOcean"];
            if (isInOcean == false) {
              char string[25];
              sprintf(string, "%f", badetemperatur);
              BADETEMPERATUREN = String(atoi(string)) + "°";
              }

            Serial.println(badetemperatur);
            Serial.println(isInOcean);
          }
        http.begin("https://www.yr.no/api/v0/locations/10-1047305/forecast"); //Specify the URL
        httpCode = http.GET(); 
        if (httpCode > 0) { //Check for the returning code
            Serial.println("https://www.yr.no/api/v0/locations/10-1047305/forecast    -> Væremelding");

            String payload = getValue(http, "dayIntervals", 3, 6500);
            
            String humidity = getValue(http, "humidity", 3, 40);
            String wind = getValue(http, "wind", 3, 200);
            String pressure = getValue(http, "pressure", 3, 40);
            String uvIndex = getValue(http, "uvIndex", 3, 40);
            String temperature = getValue(http, "temperature", 3, 200);
            String feelsLike = getValue(http, "feelsLike", 3, 40);
            

            DynamicJsonDocument json(200);
            deserializeJson(json, wind);
            float retning = json["direction"];
            float hastighet = json["speed"];
            float vindkast = json["gust"];

            deserializeJson(json, humidity);
            float luftfuktighet = json["value"];

            deserializeJson(json, pressure);
            float trykk = json["value"];

            deserializeJson(json, uvIndex);
            float UV = json["value"];

            deserializeJson(json, temperature);
            float temp = json["value"];

            deserializeJson(json, feelsLike);
            float erDetKaldt = json["value"];


            char string1[25];
            char string2[25];
            char string3[25];
            char string4[25];
            char string5[25];
            char string6[25];
            char string7[25];
            
            sprintf(string1, "%f", temp);
            sprintf(string2, "%f", erDetKaldt);
            sprintf(string3, "%f", UV);
            sprintf(string4, "%f", trykk);
            sprintf(string5, "%f", luftfuktighet);
            sprintf(string6, "%f", hastighet);
            sprintf(string7, "%f", vindkast);

            TEMPERATUREN_NOW = String(atoi(string1)) + "°";
            FOLES_SOM = String(atoi(string2)) + "°";
            UVstraalinng = String(atoi(string3)) + "";
            TRYKK = String(atoi(string4)) + "";
            LUFTFUKTIGHET = String(atoi(string5)) + "%";
            VINDSTYRKE = String(atoi(string6)) + " m/s";
            VINDKASTSTYRKE = String(atoi(string7)) + " m/s";


            Serial.println(trykk);
            Serial.println(luftfuktighet);
            Serial.println(UV);
            Serial.println(temp);
            Serial.println(erDetKaldt);
            
            
            DynamicJsonDocument doc(6500);
            
            // Parse JSON object        StaticJsonDocument<200> doc;
            deserializeJson(doc, payload);
            int day_index = atoi(dayofweek);
            for (int i = 0; i <9; i++) {
              temperatur_dager[i] = doc[i]["temperature"]["value"];
              const char* symbol = doc[i]["twentyFourHourSymbol"];
              strncpy(symboler_dager[i], symbol, sizeof(symboler_dager[i]));
              strncpy(following_days[i], ukedager[day_index], sizeof(following_days[i]));
              day_index++;
              if (day_index > 6) {day_index = 0;}
          }
          
      }
      
       
}
}}
void electricity_graph() {
    
  float h = strom[0];
  float m = strom[0];
  for(int i=0;i<24;i++)
  {
    if(strom[i]>h)
    {
      h=strom[i];
    }
    if(strom[i]<m)
    {
      m=strom[i];
    }
  }
  float my = (h + m) / 2;
  float alpha = 150 / (h-my);
  float sptpris = strom[5];
  char spotpris[25];
  char hoy[25];
  char mini[25];
  char mean[25];
  sprintf(hoy, "%.2f", ((0.7+((h-0.7)*0.1))*1.25));
  sprintf(mini, "%.2f", ((0.7+((m-0.7)*0.1))*1.25));
  sprintf(mean, "%.2f", ((0.7+((my-0.7)*0.1))*1.25));
  sprintf(spotpris, "%.2f kr", ((0.7+((sptpris-0.7)*0.1))*1.25));

  for(int i=0;i<23;i++)
  { 
    Serial.println(strom[i]);
    epd_draw_line(100+(i*37), 230-((strom[i]-my)*alpha), 135+(i*37), 230-((strom[i+1]-my)*alpha), 0,framebuffer);
    epd_draw_line(100+(i*37), 230-((strom[i]-my)*alpha)+1, 135+(i*37), 230-((strom[i+1]-my)*alpha)+1, 0,framebuffer);
    epd_draw_line(100+(i*37), 230-((strom[i]-my)*alpha)-1, 135+(i*37), 230-((strom[i+1]-my)*alpha)-1, 0,framebuffer);
  }
  cursor_x = 20;
  cursor_y = 90;
  writeln((GFXfont *)&FiraSans, hoy, &cursor_x, &cursor_y, NULL);
  cursor_x = 20;
  cursor_y = 240;
  writeln((GFXfont *)&FiraSans, mean, &cursor_x, &cursor_y, NULL);
  cursor_x = 20;
  cursor_y = 390;
  writeln((GFXfont *)&FiraSans, mini, &cursor_x, &cursor_y, NULL);

  }

void rain_graph() {

  float pixel_height = 200;
    
  float h = regn[0];
  float m = regn[0];
  
  for(int i=0;i<19;i++)
  {
    if(regn[i]>h)
    {
      h=regn[i];
    }
    if(regn[i]<m)
    {
      m=regn[i];
    }
  }
  
  float my = 0;
  float alpha = 0;
  if (h != 0) {
    my = (h + m) / 2;
    alpha = pixel_height / h;
  }

  char hoy[25];
  char mini[25];
  char mean[25];
  sprintf(hoy, "%.2f", ((0.7+((h-0.7)*0.1))*1.25));
  sprintf(mini, "%.2f", ((0.7+((m-0.7)*0.1))*1.25));
  sprintf(mean, "%.2f", ((0.7+((my-0.7)*0.1))*1.25));

  for(int i=0;i<19;i++)
  { 
    float downfall_pixel = regn[i]*alpha;
    rainfall_list[i] = downfall_pixel;
  }
  }






int get_wakeup_reason(void)
{
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    int result;

    switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0 : result = 2; return result;
    case ESP_SLEEP_WAKEUP_EXT1 : result = 3; return result;
    default : result = 1; return result;
    }

    return result;
}


void connect_to_wifi() 
{
  int wifi_counter = 0;

  //connect to WiFi--------------------------
  String connecting = "Kobler til " + String(ssid) + "\n";
  cursor_x = 200;
  cursor_y = 250;
  writeln((GFXfont *)&FiraSans, (char *)connecting.c_str(), &cursor_x, &cursor_y, NULL);
  cursor_x = 200;
  cursor_y = 300;
  const char *DOT = ".";
  const char *CONECTED = " TILKOBLET!";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      writeln((GFXfont *)&FiraSans, DOT, &cursor_x, &cursor_y, NULL);
      cursor_x = cursor_x + (5);
      wifi_counter ++;
      if (wifi_counter == 30) {
        break;
        }
  }
  if (WiFi.status() == WL_CONNECTED)
   {
    didConnect = true;
    writeln((GFXfont *)&FiraSans, CONECTED, &cursor_x, &cursor_y, NULL);
    }
  else {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    epd_clear();
    header();
    String wififail = "Wifi-tilkobling mislykkes 🙃";
    cursor_x = 100;
    cursor_y = 250;
    writeln((GFXfont *)&FiraSans, (char *)wififail.c_str(), &cursor_x, &cursor_y, NULL);
    String sleepytime = "Går i dvale nå";
    cursor_x = 100;
    cursor_y = 300;
    writeln((GFXfont *)&FiraSans, (char *)sleepytime.c_str(), &cursor_x, &cursor_y, NULL);
    }
}


String getValue(HTTPClient &http, String key, int skip, int get) {
  bool found = false, look = false;
  int ind = 0;
  String ret_str = "";

  int len = http.getSize();
  char char_buff[1];
  WiFiClient * stream = http.getStreamPtr();
  while (http.connected() && (len > 0 || len == -1)) {
    size_t size = stream->available();
    if (size) {
      int c = stream->readBytes(char_buff, ((size > sizeof(char_buff)) ? sizeof(char_buff) : size));
      if (len > 0)
        len -= c;
      if (found) {
        if (skip == 0) {
          ret_str += char_buff[0];
          get --;
        } else
          skip --;
        if (get <= 0)
          break;
      }
      else if ((!look) && (char_buff[0] == key[0])) {
        look = true;
        ind = 1;
      } else if (look && (char_buff[0] == key[ind])) {
        ind ++;
        if (ind == key.length()) found = true;
      } else if (look && (char_buff[0] != key[ind])) {
        ind = 0;
        look = false;
      }
    }
  }
  return ret_str;
}
// inserts into subject[] at position pos
void append(char subject[], const char insert[], int pos) {
    char *buf = (char*)malloc(strlen(subject) + strlen(insert) + 2);
    // or you could use malloc() to allocate sufficient space
    // e.g. char *buf = (char*)malloc(strlen(subject) + strlen(insert) + 2);
    // to fill with zeros: memset(buf, 0, 100);

    strncpy(buf, subject, pos); // copy at most first pos characters
    int len = strlen(buf);
    strcpy(buf+len, insert); // copy all of insert[] at the end
    len += strlen(insert);  // increase the length by length of insert[]
    strcpy(buf+len, subject+pos); // copy the rest

    strcpy(subject, buf);   // copy it back to subject
    // Note that subject[] must be big enough, or else segfault.
    // deallocate buf[] here, if used malloc()
    // e.g. free(buf);
}

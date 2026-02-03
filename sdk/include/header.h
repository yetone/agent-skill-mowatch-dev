#ifndef HEADER_H
#define HEADER_H
#include <stdint.h>

#define SCREEN_WIDTH   200
#define SCREEN_HEIGHT  200

#define BLACK   0
#define WHITE   0xFF
#define GREY    0x80

#define MODE_EMPTY  0
#define MODE_FILL   1

#define FA_READ           0x01
#define FA_WRITE          0x02
#define FA_OPEN_EXISTING  0x00
#define FA_CREATE_NEW     0x04
#define FA_CREATE_ALWAYS  0x08
#define FA_OPEN_ALWAYS    0x10
#define FA_OPEN_APPEND    0x30

#define SCREEN_TYPE_EINK 0
#define SCREEN_TYPE_MONO 1
#define SCREEN_TYPE SCREEN_TYPE_EINK

typedef enum UPDAT_TYPE {
    FULL_UPDATE,
    PART_UPDATE,
    NONE_UPDATE,
} UpdateType;

typedef enum MBUTTON_TYPE {
    KEY_NULL,
    KEY_CENTER,
    KEY_UP,
    KEY_DOWN,
    KEY_CENTER_UP,
    KEY_UP_CENTER,
    KEY_CENTER_DOWN,
    KEY_DOWN_CENTER,
    KEY_UP_DOWN,
    KEY_DOWN_UP,
    KEY_BACK,
} ButtonType;

typedef struct TODAYDATA {
    uint16_t moonicon;
    uint16_t pressure;
    uint16_t humidity;
    uint8_t sunraise[6];
    uint8_t sunset[6];
    uint8_t lunar[16];
} TodayData;

typedef struct WEATHER {
    uint8_t date;
    char day_temp;
    char night_temp;
    uint16_t day_icon;
    uint16_t night_icon;
} Weather;

typedef enum WEATHERDAY {
    TOTDAY,
    TOMORROW,
    AFTERTOMOROW,
    DAY_COUNT,
} WeatherDay;

typedef enum ROTATE {
    ROTATE_0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270,
} Rotate;

typedef enum BLUESTTE {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
} BlueState;

__attribute__((section(".keep_section"))) void onDraw(void);
__attribute__((section(".keep_section"))) UpdateType onUpdate(int delta);
__attribute__((section(".keep_section"))) UpdateType onKey(ButtonType key);
__attribute__((section(".keep_section"))) void app_init(intptr_t *draw_ptr_t, intptr_t *onkey_ptr_t, intptr_t* onupdate_ptr_t, intptr_t* func_arr);

void __initialize_datas(const intptr_t* func_arr);

void *m_malloc(uint32_t size);
void m_free(void *ptr);
int rand(void);
void srand(unsigned int seed);
void co_sprintf(char *out, const char *format, ...);
uint16_t utf_len(const unsigned char *chr);

void create_msg_dialog(const uint8_t* title, const uint8_t* msg, void (*submit)(uint8_t ok));
void create_menu_dialog(const char* title, const char** menu_item_names, const uint8_t count, void (*submit)(uint8_t confirm));
void create_picker_dialog(uint8_t num, const uint8_t* nums, const uint8_t count, void (*submit)(uint8_t data));
uint8_t watch_app_isweather_ok(void);
uint8_t watch_app_battpercent(void);
TodayData watch_app_getToday(void);
Weather watch_app_getweather(WeatherDay day);
BlueState watch_app_bluestate(void);
void watch_app_http_req(const char* url, void(req_callback)(char*));
void watch_app_log(char* log);
void watch_app_exit(void);
uint32_t watch_app_read_file(const uint8_t* file_name, uint8_t* buffer, uint32_t len, uint32_t seekofs);
uint32_t watch_app_write_file(const uint8_t* file_name, uint8_t* buffer, uint32_t len, uint32_t seekofs, uint8_t fa_mode);
uint8_t watch_app_delete_file(const uint8_t* file_name);
uint8_t watch_app_mkdir(const uint8_t* dir);

void set_update_interval(uint32_t interval);
uint32_t get_update_interval(void);

void eink_clear(uint16_t color);
void eink_drawpixel(uint16_t x, uint16_t y, uint16_t color);
void eink_drawline(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color);
void eink_drawdashedline(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t Color);
void eink_drawrect(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color, uint8_t fill_mode);
void eink_drawcircle(int x_center, int y_center, int radius, int color, int fill_mode);
void eink_draw_bmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t data[], uint16_t color, uint8_t transparent);
void eink_set_rotate(Rotate rotate);
void get_weather_icon(uint16_t iconidx, uint8_t *buffer);
uint16_t eink_drawstr(uint16_t x, uint16_t y, const unsigned char *chr, uint16_t size, uint16_t color);
uint16_t eink_drawchstr(uint16_t x, uint16_t y, const unsigned char *chr, uint16_t color);
uint16_t eink_draw_rectstr(const unsigned char *chr, uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color);

int RTC_getYear(void);
int RTC_getMon(void);
int RTC_getDay(void);
int RTC_getHour(void);
int RTC_getMin(void);
int RTC_getSec(void);
int RTC_getWeek(void);
int RTC_getTimeStamp(void);

#endif

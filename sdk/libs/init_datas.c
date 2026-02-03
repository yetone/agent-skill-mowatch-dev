#include "header.h"
#include <stdint.h>

enum FUNC_TAGS {
    tag_watch_app_battpercent,
    tag_watch_app_http_req,
    tag_watch_app_log,
    tag_watch_app_exit,
    tag_watch_app_mkdir,
    tag_watch_app_write_file,
    tag_watch_app_read_file,
    tag_watch_app_delete_file,
    tag_watch_app_bluestate,
    tag_watch_app_getweather,
    tag_watch_app_getToday,
    tag_watch_app_isweather_ok,
    tag_eink_clear,
    tag_eink_drawpixel,
    tag_eink_drawline,
    tag_eink_drawdashedline,
    tag_eink_drawrect,
    tag_eink_drawcircle,
    tag_eink_draw_bmp,
    tag_eink_drawstr,
    tag_eink_drawchstr,
    tag_eink_draw_rectstr,
    tag_RTC_getYear,
    tag_RTC_getMon,
    tag_RTC_getDay,
    tag_RTC_getHour,
    tag_RTC_getMin,
    tag_RTC_getSec,
    tag_RTC_getWeek,
    tag_RTC_getTimeStamp,
    tag_create_msg_dialog,
    tag_create_menu_dialog,
    tag_os_malloc,
    tag_os_free,
    tag_utf_len,
    tag_create_picker_dialog,
    tag_eink_set_rotate,
    tag_set_interval,
    tag_get_interval,
    tag_get_weather_icon,
    tag_beep_on,
    tag_beep_off,
    tag_buzzer,
    tag_count,
};

const intptr_t *func_array;

extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

void __initialize_datas(const intptr_t *func_arr) {
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    uint32_t *end = &_edata;
    while(dst < end) { *dst++ = *src++; }
    src = &_sbss;
    end = &_ebss;
    while(src < end) { *src++ = 0; }
    func_array = func_arr;
}

typedef void* (*func_malloc_ptr_t)(uint32_t);
typedef void (*func_eink_clear_ptr_t)(uint16_t);
typedef void (*func_free_ptr_t)(void*);
typedef uint16_t (*func_utf_len_ptr_t)(const unsigned char*);
typedef void (*func_get_weather_icon_ptr_t)(uint16_t iconidx, uint8_t *buffer);
typedef void (*func_msgdialog_ptr_t)(const uint8_t* title, const uint8_t* msg, void (*submit)(uint8_t ok));
typedef void (*func_pickerdialog_ptr_t)(uint8_t num, const uint8_t* nums, uint8_t count, void (*submit)(uint8_t data));
typedef void (*func_menudialog_ptr_t)(const char* title, const char** menu_item_names, const uint8_t count, void (*submit)(uint8_t confirm));
typedef uint8_t (*func_get_ptr_t)(void);
typedef TodayData (*func_gettoday_ptr_t)(void);
typedef Weather (*func_getweather_ptr_t)(WeatherDay);
typedef BlueState (*func_getblue_ptr_t)(void);
typedef void (*func_httpreq_ptr_t)(const char* url, void(req_callback)(char*));
typedef void (*func_void_ptr_t)(void);
typedef void (*func_enable_ptr_t)(uint8_t);
typedef void (*func_set_interval_ptr_t)(uint32_t);
typedef uint32_t (*func_get_interval_ptr_t)(void);

void set_update_interval(uint32_t interval) {
    return ((func_set_interval_ptr_t)(intptr_t)func_array[tag_set_interval])(interval);
}
uint32_t get_update_interval(void) {
    return ((func_get_interval_ptr_t)(intptr_t)func_array[tag_get_interval])();
}
void get_weather_icon(uint16_t iconidx, uint8_t *buffer) {
    return ((func_get_weather_icon_ptr_t)(intptr_t)func_array[tag_get_weather_icon])(iconidx, buffer);
}
void eink_set_rotate(uint8_t rotate) {
    return ((func_enable_ptr_t)(intptr_t)func_array[tag_eink_set_rotate])(rotate);
}
void create_picker_dialog(uint8_t num, const uint8_t* nums, const uint8_t count, void (*submit)(uint8_t data)) {
    return ((func_pickerdialog_ptr_t)(intptr_t)func_array[tag_create_picker_dialog])(num, nums, count, submit);
}
void *m_malloc(uint32_t size) {
    return ((func_malloc_ptr_t)(intptr_t)func_array[tag_os_malloc])(size);
}
void m_free(void *ptr) {
    return ((func_free_ptr_t)(intptr_t)func_array[tag_os_free])(ptr);
}
uint16_t utf_len(const unsigned char *chr) {
    return ((func_utf_len_ptr_t)(intptr_t)func_array[tag_utf_len])(chr);
}
void create_msg_dialog(const uint8_t* title, const uint8_t* msg, void (*submit)(uint8_t ok)) {
    return ((func_msgdialog_ptr_t)(intptr_t)func_array[tag_create_msg_dialog])(title, msg, submit);
}
void create_menu_dialog(const char* title, const char** menu_item_names, const uint8_t count, void (*submit)(uint8_t confirm)) {
    return ((func_menudialog_ptr_t)(intptr_t)func_array[tag_create_menu_dialog])(title, menu_item_names, count, submit);
}
uint8_t watch_app_isweather_ok(void) {
    return ((func_get_ptr_t)(intptr_t)func_array[tag_watch_app_isweather_ok])();
}
uint8_t watch_app_battpercent(void) {
    return ((func_get_ptr_t)(intptr_t)func_array[tag_watch_app_battpercent])();
}
TodayData watch_app_getToday(void) {
    return ((func_gettoday_ptr_t)(intptr_t)func_array[tag_watch_app_getToday])();
}
Weather watch_app_getweather(WeatherDay day) {
    return ((func_getweather_ptr_t)(intptr_t)func_array[tag_watch_app_getweather])(day);
}
BlueState watch_app_bluestate(void) {
    return ((func_getblue_ptr_t)(intptr_t)func_array[tag_watch_app_bluestate])();
}
void watch_app_http_req(const char* url, void(req_callback)(char*)) {
    return ((func_httpreq_ptr_t)(intptr_t)func_array[tag_watch_app_http_req])(url, req_callback);
}
typedef void (*func_applog_ptr_t)(char* log);
void watch_app_log(char* log) {
    return ((func_applog_ptr_t)(intptr_t)func_array[tag_watch_app_log])(log);
}
void watch_app_exit(void) {
    return ((func_void_ptr_t)(intptr_t)func_array[tag_watch_app_exit])();
}
typedef uint32_t (*func_readfile_ptr_t)(const uint8_t* file_name, uint8_t* buffer, uint32_t len, uint32_t seekofs);
typedef uint32_t (*func_writefile_ptr_t)(const uint8_t* file_name, uint8_t* buffer, uint32_t len, uint32_t seekofs, uint8_t fa_mode);
typedef uint8_t (*func_mkdel_ptr_t)(const uint8_t* file_name);
uint32_t watch_app_read_file(const uint8_t* file_name, uint8_t* buffer, uint32_t len, uint32_t seekofs) {
    return ((func_readfile_ptr_t)(intptr_t)func_array[tag_watch_app_read_file])(file_name, buffer, len, seekofs);
}
uint32_t watch_app_write_file(const uint8_t* file_name, uint8_t* buffer, uint32_t len, uint32_t seekofs, uint8_t fa_mode) {
    return ((func_writefile_ptr_t)(intptr_t)func_array[tag_watch_app_write_file])(file_name, buffer, len, seekofs, fa_mode);
}
uint8_t watch_app_mkdir(const uint8_t* dir) {
    return ((func_mkdel_ptr_t)(intptr_t)func_array[tag_watch_app_mkdir])(dir);
}
uint8_t watch_app_delete_file(const uint8_t* file_name) {
    return ((func_mkdel_ptr_t)(intptr_t)func_array[tag_watch_app_delete_file])(file_name);
}
void eink_clear(uint16_t color) {
    return ((func_eink_clear_ptr_t)(intptr_t)func_array[tag_eink_clear])(color);
}
typedef void (*func_drawpixel_ptr_t)(uint16_t x, uint16_t y, uint16_t color);
void eink_drawpixel(uint16_t x, uint16_t y, uint16_t color) {
    return ((func_drawpixel_ptr_t)(intptr_t)func_array[tag_eink_drawpixel])(x, y, color);
}
typedef void (*func_drawline_ptr_t)(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color);
void eink_drawline(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color) {
    return ((func_drawline_ptr_t)(intptr_t)func_array[tag_eink_drawline])(start_x, start_y, end_x, end_y, color);
}
void eink_drawdashedline(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color) {
    return ((func_drawline_ptr_t)(intptr_t)func_array[tag_eink_drawdashedline])(start_x, start_y, end_x, end_y, color);
}
typedef void (*func_drawrect_ptr_t)(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color, uint8_t fill_mode);
void eink_drawrect(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color, uint8_t fill_mode) {
    return ((func_drawrect_ptr_t)(intptr_t)func_array[tag_eink_drawrect])(start_x, start_y, end_x, end_y, color, fill_mode);
}
typedef void (*func_drawcircle_ptr_t)(int x_center, int y_center, int radius, int color, int fill_mode);
void eink_drawcircle(int x_center, int y_center, int radius, int color, int fill_mode) {
    return ((func_drawcircle_ptr_t)(intptr_t)func_array[tag_eink_drawcircle])(x_center, y_center, radius, color, fill_mode);
}
typedef void (*func_drawbmp_ptr_t)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t data[], uint16_t color, uint8_t transparent);
void eink_draw_bmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t data[], uint16_t color, uint8_t transparent) {
    return ((func_drawbmp_ptr_t)(intptr_t)func_array[tag_eink_draw_bmp])(x, y, w, h, data, color, transparent);
}
typedef uint16_t (*func_drawstr_ptr_t)(uint16_t x, uint16_t y, const unsigned char *chr, uint16_t size, uint16_t color);
uint16_t eink_drawstr(uint16_t x, uint16_t y, const unsigned char *chr, uint16_t size, uint16_t color) {
    return ((func_drawstr_ptr_t)(intptr_t)func_array[tag_eink_drawstr])(x, y, chr, size, color);
}
typedef uint16_t (*func_drawchstr_ptr_t)(uint16_t x, uint16_t y, const unsigned char *chr, uint16_t color);
uint16_t eink_drawchstr(uint16_t x, uint16_t y, const unsigned char *chr, uint16_t color) {
    return ((func_drawchstr_ptr_t)(intptr_t)func_array[tag_eink_drawchstr])(x, y, chr, color);
}
typedef uint16_t (*func_drawrectstr_ptr_t)(const unsigned char *chr, uint16_t start_x, uint16_t start_y, uint16_t stop_x, uint16_t stop_y, uint16_t color);
uint16_t eink_draw_rectstr(const unsigned char *chr, uint16_t start_x, uint16_t start_y, uint16_t stop_x, uint16_t stop_y, uint16_t color) {
    return ((func_drawrectstr_ptr_t)(intptr_t)func_array[tag_eink_draw_rectstr])(chr, start_x, start_y, stop_x, stop_y, color);
}
typedef int(*func_rtc_ptr_t)(void);
int RTC_getDay(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getDay])(); }
int RTC_getHour(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getHour])(); }
int RTC_getMin(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getMin])(); }
int RTC_getMon(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getMon])(); }
int RTC_getSec(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getSec])(); }
int RTC_getTimeStamp(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getTimeStamp])(); }
int RTC_getWeek(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getWeek])(); }
int RTC_getYear(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getYear])(); }

#ifndef SPMP_INPUT_TEST_H
#define SPMP_INPUT_TEST_H
#include "oem_adapter.h"
typedef uint32_t (*input_time_fn)(void); /* native +124: low tick delta *10, modulo32 */
typedef void (*input_delay_fn)(uint32_t low_ticks,uint32_t high_ticks); /* +11c */
typedef int32_t (*input_open_fn)(const char *,uint32_t flags,uint32_t mode);
typedef int32_t (*input_write_fn)(int32_t,const void *,uint32_t);
typedef int32_t (*input_seek_fn)(int32_t,int32_t offset,uint32_t whence);
typedef int32_t (*input_close_fn)(int32_t);
typedef uint32_t (*input_error_fn)(void); /* Snapshot of current thread's eCos errno. */
struct input_services {
    struct probe_oem_api graphics;
    input_time_fn time;
    input_delay_fn delay;
    input_open_fn open;
    input_write_fn write;
    input_seek_fn seek;
    input_close_fn close;
    input_error_fn error_number;
};
struct input_config { uint32_t duration_units,max_polls,prompt_units,redraw_units; };
enum input_log_status { INPUT_LOG_OK=0,INPUT_LOG_OPEN_ERROR=1,INPUT_LOG_SEEK_ERROR=2,
 INPUT_LOG_WRITE_ERROR=3,INPUT_LOG_CLOSE_ERROR=4,INPUT_LOG_LIMIT=5 };
struct input_state {
    uint32_t polls,events,keys,pressed,released,seen,last,elapsed,start;
    uint32_t log_status,log_bytes,log_records,prompt,sample[13],sample_valid;
    uint32_t result,frames;
    int16_t x,y;
    uint32_t ui_keys,ui_events,ui_second,ui_prompt,ui_log,ui_cursor_tick;
    int16_t ui_x,ui_y;
    uint8_t color_saved;
    struct mmi_rgb888 original_color;
    uint32_t menu_choice,menu_view,ui_menu,ui_view;
    uint32_t log_errno;
};
_Static_assert(sizeof(struct input_state)==172,"Input state trace layout");
_Static_assert(offsetof(struct input_state,menu_choice)==152,"Input menu trace offset");
extern const char input_log_path[];
void input_journal_append(struct input_state *,const struct input_services *,const char *path,const char *row);
const char *input_prompt(uint32_t index);
uint32_t input_test_run(struct input_state *,const struct input_services *,const struct input_config *);
#endif

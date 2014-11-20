#ifndef KITCHENLIGHT_H_P9HZWAYT
#define KITCHENLIGHT_H_P9HZWAYT

#include <stdint.h>


#define KITCHENLIGHT_BUFFER_SIZE (30*6)


typedef struct ScreenConfig {
	void (*Init)(void);
	void (*Deinit)(void);
	void (*Update)(void);
	void (*Draw)(void);
	void (*OnDelegate)(void);
	void (*OnResume)(void);
	void (*OnPause)(void);
	void (*OnUnpause)(void);
	//void (*OnInit)(struct ScreenConfig* state);
	//void (*OnDeinit)(struct ScreenConfig* state);
	//void (*OnDelegate)(struct ScreenConfig* state);
	//void (*OnResume)(struct ScreenConfig* state);
	//void (*OnPause)(struct ScreenConfig* state);
	//void (*OnUnpause)(struct ScreenConfig* state);
	//void (*Update)(uint32_t);
	//void (*Draw)(uint32_t);
} ScreenConfig;

typedef struct ScreenState {
	struct ScreenConfig *config;
	struct ScreenState *previousState;
        void* user_data;
} ScreenState;


extern ScreenState* current_screen_state;


uint32_t* get_working_buffer(void);
void set_next_buffer(uint32_t* buffer);
void set_next_buffer_ro(uint32_t* buffer);
void change_screen(ScreenConfig *sc);
void ExitState(void);
void copy_buffer_b2f(uint32_t* src, uint32_t* dst);


void initialize_kitchenlight(void);
void kitchenlight_loop(void);


#endif /* end of include guard: KITCHENLIGHT_H_P9HZWAYT */

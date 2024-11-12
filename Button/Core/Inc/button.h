#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_


typedef struct {
	int ioIdx;		// input pin index
	int cState;		// current state
	int aState;		// active state
	int dbCount;	// debounce counter
} BTN_PIN;


enum {
	BTN_SET,
	BTN_UP,
	BTN_DOWN,
};

void BTN_InitButtons(void);
void BTN_ScanButtons(void);


extern unsigned g_Buttons[];

#endif /* INC_BUTTON_H_ */

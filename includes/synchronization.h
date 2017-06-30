#ifndef __synchro_h
#define __synchro_h

typedef enum{
	READER,
	WRITER
} synchro_type;

void synchro_init();
void enter_critical(synchro_type type);
void leave_critical(synchro_type type);

#endif
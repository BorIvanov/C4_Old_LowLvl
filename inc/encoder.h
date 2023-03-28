#ifndef __interrupt_H
#define __interrupt_H

void initEncoder();
uint16_t getEncoderXPosition();
uint16_t getEncoderZPosition();
void initTimer4Interrupt();
void initTimer3Interrupt();
#endif

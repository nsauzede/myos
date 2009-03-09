#ifndef IOPORT_H
#define IOPORT_H

#if 1
void outb( /*uint8_t*/ value, /*uint16_t*/ port);
uint8_t inb( /*uint_16_t*/ port);
#else
void outb( uint8_t value, uint16_t port);
uint8_t inb( uint_16_t port);
#endif

#endif/*IOPORT_H*/

#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void irq_handler(void);

    extern void irq_enable(void);
    extern void irq_disable(void);

    extern volatile unsigned int irq_count;

#ifdef __cplusplus
}
#endif

#endif // IRQ_H

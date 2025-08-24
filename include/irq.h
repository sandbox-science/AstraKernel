#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void irq_handler(void);

    void irq_enable(void);
    void irq_disable(void);

    extern volatile unsigned int tick;

#ifdef __cplusplus
}
#endif

#endif // IRQ_H

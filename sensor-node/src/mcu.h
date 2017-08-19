#ifndef __INIT_HW_HAL_CAN_H
#define __INIT_HW_HAL_CAN_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdbool.h>

void init_hw(void);
bool is_ms_set(void);
void can_transmit(void);

#ifdef __cplusplus
}
#endif

#endif /* __INIT_HW_HAL_CAN_H */

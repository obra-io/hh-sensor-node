#ifndef __INIT_HW_HAL_CAN_H
#define __INIT_HW_HAL_CAN_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdbool.h>

void init_hw(void);
void send_can_msg (uint16_t adc_8,uint8_t num_of_bytes,uint8_t can_id);

#ifdef __cplusplus
}
#endif

#endif /* __INIT_HW_HAL_CAN_H */

// BSP/Inc/bsp_tb6612.h
#ifndef BSP_TB6612_H
#define BSP_TB6612_H

#include "board_config.h"
#include "bsp_uart.h"
#include <stdint.h>
#include <stdbool.h>

/* 电机编号（对应 D24A 的 A/B/C/D 四路） */
typedef enum {
    BSP_MOTOR_A = 0,
    BSP_MOTOR_B,
    BSP_MOTOR_C,
    BSP_MOTOR_D,
    BSP_MOTOR_NUM
} bsp_motor_id_t;

/* 电机运动状态 */
typedef enum {
    BSP_MOTOR_STOP = 0,     // 滑行停止
    BSP_MOTOR_BRAKE,        // 刹车
    BSP_MOTOR_FORWARD,      // 正转
    BSP_MOTOR_BACKWARD,     // 反转
} bsp_motor_dir_t;

/* 占空比满量程，0 ~ 1000 对应 0% ~ 100% */
#define BSP_MOTOR_DUTY_MAX   1000

/* ---------- 基础控制接口 ---------- */
int  bsp_motor_init(void);
void bsp_motor_set(bsp_motor_id_t id, bsp_motor_dir_t dir, uint16_t duty);
void bsp_motor_stop(bsp_motor_id_t id);
void bsp_motor_brake(bsp_motor_id_t id);
void bsp_motor_stop_all(void);
void bsp_motor_standby(bool enable);

/* ---------- 电机自检（调试用） ---------- */
void bsp_motor_selftest_start(uint32_t step_ms, uint16_t duty);
void bsp_motor_selftest_stop(void);
bool bsp_motor_selftest_is_running(void);
void bsp_motor_selftest_task(void);   /* 周期性调用，非阻塞 */

#endif /* BSP_TB6612_H */
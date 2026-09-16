// BSP/Src/bsp_tb6612.c
#include "bsp_tb6612.h"
#include "main.h"

extern TIM_HandleTypeDef htim1;      /* CubeMX 生成的 TIM1 句柄 */

/* PWM 定时器 ARR 值，与 CubeMX 配置一致
 * PSC=71, ARR=99 → 72MHz/72/100 = 10kHz
 */
#define TB6612_PWM_ARR       99

/* 电机硬件资源表 */
typedef struct {
    uint32_t        pwm_channel;
    GPIO_TypeDef   *in1_port;
    uint16_t        in1_pin;
    GPIO_TypeDef   *in2_port;
    uint16_t        in2_pin;
} tb6612_motor_t;

static const tb6612_motor_t motors[BSP_MOTOR_NUM] = {
    [BSP_MOTOR_A] = { TIM_CHANNEL_1, GPIOB, GPIO_PIN_0, GPIOB, GPIO_PIN_1 },
    [BSP_MOTOR_B] = { TIM_CHANNEL_2, GPIOC, GPIO_PIN_4, GPIOC, GPIO_PIN_5 },
    [BSP_MOTOR_C] = { TIM_CHANNEL_3, GPIOC, GPIO_PIN_0, GPIOC, GPIO_PIN_1 },
    [BSP_MOTOR_D] = { TIM_CHANNEL_4, GPIOC, GPIO_PIN_2, GPIOC, GPIO_PIN_3 },
};

static const char *motor_name[BSP_MOTOR_NUM] = { "A", "B", "C", "D" };

/* ==================== 基础控制 ==================== */

int bsp_motor_init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

    for (uint8_t i = 0; i < BSP_MOTOR_NUM; i++) {
        __HAL_TIM_SET_COMPARE(&htim1, motors[i].pwm_channel, 0);
        HAL_GPIO_WritePin(motors[i].in1_port, motors[i].in1_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(motors[i].in2_port, motors[i].in2_pin, GPIO_PIN_RESET);
    }

    bsp_motor_standby(true);
    return 0;
}

void bsp_motor_set(bsp_motor_id_t id, bsp_motor_dir_t dir, uint16_t duty)
{
    if (id >= BSP_MOTOR_NUM) return;
    if (duty > BSP_MOTOR_DUTY_MAX) duty = BSP_MOTOR_DUTY_MAX;

    const tb6612_motor_t *m = &motors[id];

    switch (dir) {
        case BSP_MOTOR_FORWARD:
            HAL_GPIO_WritePin(m->in1_port, m->in1_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(m->in2_port, m->in2_pin, GPIO_PIN_RESET);
            break;

        case BSP_MOTOR_BACKWARD:
            HAL_GPIO_WritePin(m->in1_port, m->in1_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(m->in2_port, m->in2_pin, GPIO_PIN_SET);
            break;

        case BSP_MOTOR_BRAKE:
            HAL_GPIO_WritePin(m->in1_port, m->in1_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(m->in2_port, m->in2_pin, GPIO_PIN_SET);
            duty = 0;
            break;

        case BSP_MOTOR_STOP:
        default:
            HAL_GPIO_WritePin(m->in1_port, m->in1_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(m->in2_port, m->in2_pin, GPIO_PIN_RESET);
            duty = 0;
            break;
    }

    uint32_t compare = (uint32_t)duty * TB6612_PWM_ARR / BSP_MOTOR_DUTY_MAX;
    __HAL_TIM_SET_COMPARE(&htim1, m->pwm_channel, compare);
}

void bsp_motor_stop(bsp_motor_id_t id)
{
    bsp_motor_set(id, BSP_MOTOR_STOP, 0);
}

void bsp_motor_brake(bsp_motor_id_t id)
{
    bsp_motor_set(id, BSP_MOTOR_BRAKE, 0);
}

void bsp_motor_stop_all(void)
{
    for (uint8_t i = 0; i < BSP_MOTOR_NUM; i++) {
        bsp_motor_stop((bsp_motor_id_t)i);
    }
}

void bsp_motor_standby(bool enable)
{
    HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin,
                      enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* ==================== 电机自检 ==================== */

typedef struct {
    bool     running;
    uint32_t step_ms;       /* 每步持续时间 */
    uint16_t duty;          /* 测试占空比 */
    uint32_t last_tick;     /* 上一步切换时间 */
    uint8_t  step;          /* 0 ~ (2*BSP_MOTOR_NUM - 1) */
} motor_selftest_t;

static motor_selftest_t s_test = {
    .running = false,
    .step_ms = 1500,
    .duty    = 500,
    .last_tick = 0,
    .step    = 0,
};

void bsp_motor_selftest_start(uint32_t step_ms, uint16_t duty)
{
    if (step_ms < 100) step_ms = 100;      /* 下限保护 */
    s_test.step_ms = step_ms;
    s_test.duty    = (duty > BSP_MOTOR_DUTY_MAX) ? BSP_MOTOR_DUTY_MAX : duty;
    s_test.step    = 0;
    s_test.last_tick = HAL_GetTick();
    s_test.running = true;

    bsp_motor_stop_all();
}

void bsp_motor_selftest_stop(void)
{
    s_test.running = false;
    bsp_motor_stop_all();
}

bool bsp_motor_selftest_is_running(void)
{
    return s_test.running;
}

void bsp_motor_selftest_task(void)
{
    if (!s_test.running) return;

    uint32_t now = HAL_GetTick();
    if (now - s_test.last_tick < s_test.step_ms) return;
    s_test.last_tick = now;

    /* 先停止所有电机 */
    bsp_motor_stop_all();

    uint8_t idx = s_test.step / 2;
    if (idx >= BSP_MOTOR_NUM) {
        /* 完成一轮，重新开始 */
        s_test.step = 0;
        return;
    }

    bsp_motor_dir_t dir = (s_test.step % 2 == 0)
                        ? BSP_MOTOR_FORWARD
                        : BSP_MOTOR_BACKWARD;

    bsp_motor_set((bsp_motor_id_t)idx, dir, s_test.duty);

    bsp_uart_printf(BSP_UART_1, "[SELFTEST] Motor %s %s duty=%d\r\n",
                    motor_name[idx],
                    (dir == BSP_MOTOR_FORWARD) ? "FWD" : "BWD",
                    s_test.duty);

    s_test.step++;
}
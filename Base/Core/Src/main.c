#include "main.h"
#include <stdint.h>
#include "io.h"

typedef uint32_t clock_t;
int g_count;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void Task_LED(void);
void Task_A(void);


int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  IO_Init(IOP_LED);

  // IO_WRITE(IOP_LED, 1);
  // IO_WRITE(IOP_LED, 0);

  // Görev Çevrimi (Task Loop)
  // Co-Operative Multitasking (Yardımlaşmalı Çoklu Görev)
  while (1)
  {
	  Task_LED();
	  Task_A();
  }
}

void Task_A(void)
{
	++g_count;
}

void Task_LED(void)
{
	static enum {
		I_LED_OFF,
		S_LED_OFF,
		I_LED_ON,
		S_LED_ON,
	} state = I_LED_OFF;

	static clock_t t0;	// duruma ilk geçiş saati
	clock_t t1;			// güncel saat değeri

	t1 = HAL_GetTick();

	switch(state) {
	case I_LED_OFF:
		t0 = t1;
		IO_WRITE(IOP_LED, 0);
		state = S_LED_OFF;
		break;

	case S_LED_OFF:
		if(t1 >= t0 + 900) {
			state = I_LED_ON;
		}
		break;

	case I_LED_ON:
		t0 = t1;
		IO_WRITE(IOP_LED, 1);
		state = S_LED_ON;
		break;

	case S_LED_ON:
		if(t1 >= t0 + 100) {
			state = I_LED_OFF;
		}
		break;
	}
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */

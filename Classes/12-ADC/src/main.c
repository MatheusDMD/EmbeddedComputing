/** 
 * 12- ADC
 * Rafael Corsi @ insper.edu.br
 * Abril 2017
 * 
 * Configura o ADC do SAME70 para fazer leitura
 * do sensor de temperatura interno
 */

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "asf.h"

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/** Header printf */
#define STRING_EOL    "\r"
#define STRING_HEADER "-- AFEC Temperature Sensor Example --\r\n" \
"-- "BOARD_NAME" --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/** Reference voltage for AFEC,in mv. */
#define VOLT_REF        (3300)

/** The maximal digital value */
/** 2^12 - 1                  */
#define MAX_DIGITAL     (4095UL)

/** The conversion data is done flag */
volatile bool is_conversion_done = false;

/** The conversion data value */
volatile uint32_t g_ul_value = 0;

/* Canal do sensor de temperatura */
#define AFEC_CHANNEL_TEMP_SENSOR 11


/************************************************************************/
/* Definindo data inicial                                               */
/************************************************************************/

#define YEAR        2017
#define MOUNTH      3
#define DAY         27
#define WEEK        13
#define HOUR        9
#define MINUTE      5
#define SECOND      0

/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/
void increment_segundo(int count,int segundo,int minuto, int hora, int dia, int mes, int ano);
void RTC_init();
/**
 * \brief Configure UART console.
 * BaudRate : 115200
 * 8 bits
 * 1 stop bit
 * sem paridade
 */

static void configure_console(void)
{
 
	const usart_serial_options_t uart_serial_options = {
		.baudrate   = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits   = CONF_UART_STOP_BITS,
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/** 
 * converte valor lido do ADC para temperatura em graus celsius
 * input : ADC reg value
 * output: Temperature in celsius
 */
static int32_t convert_adc_to_temp(int32_t ADC_value){
  
  int32_t ul_vol;
  int32_t ul_temp;
  
	ul_vol = ADC_value * VOLT_REF / MAX_DIGITAL;

  /*
   * According to datasheet, The output voltage VT = 0.72V at 27C
   * and the temperature slope dVT/dT = 2.33 mV/C
   */
  ul_temp = (ul_vol - 720)  * 100 / 233 + 27;
  return(ul_temp);
}

void increment_segundo(int count,int segundo,int minuto, int hora, int dia, int mes, int ano){
	int var = 0;
	while(segundo < 60){
	if(segundo == 59){
      if(minuto>=59){
	      if(hora>=23){
		      if(dia>=30){
			      if(mes>=12){
				      ano++;
					  mes = 1;
			      }
			      else{
				      mes++;
					  dia = 1;
			      }
		      }
		      else{
			      dia++;
				  hora = 0;
		      }
	      }
	      else{
		      hora++;
			  minuto = 0;
	      }
      }
      else{
	      minuto++;
		  segundo = 0;
      }
	 }
	 segundo++;
	 count ++;
	 if(var == count){
		break;
	 }
	}
}

/************************************************************************/
/* Call backs / Handler                                                 */
/************************************************************************/

/**
 * \brief AFEC interrupt callback function.
 */
static void AFEC_Temp_callback(void)
{
	g_ul_value = afec_channel_get_value(AFEC0, AFEC_CHANNEL_TEMP_SENSOR);
	is_conversion_done = true;
}


void RTC_Handler(void)
{
	uint32_t ul_status = rtc_get_status(RTC);

	/* Second increment interrupt */
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
		
		rtc_clear_status(RTC, RTC_SCCR_SECCLR);

		} else {
		/* Time or date alarm */
		if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
			
			/*Atualiza hora */
			uint32_t ano,mes,dia,hora,minuto,segundo;
			
			rtc_get_date(RTC, &ano, &mes, &dia, NULL);
			rtc_get_time(RTC, &hora, &minuto, &segundo);
			
			/* incrementa minuto */
			increment_segundo(1,segundo,minuto,hora,dia,mes,ano);
			
			/* configura novo alarme do RTC */
			rtc_set_date_alarm(RTC, 1, mes, 1, dia);
			rtc_set_time_alarm(RTC, 1, hora, 1, minuto, 1, segundo);
			
			/* printa valor de temperatura */
			is_conversion_done = false;
			int temp = convert_adc_to_temp(g_ul_value);
			printf("%d/%d/%d - %d:%d:%d - Temp : %d \r\n",dia,mes,ano,hora,minuto,segundo,temp);
			afec_start_software_conversion(AFEC0);

			rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
		}
	}
}

/************************************************************************/
/* inicializador do RTC                                                 */
/************************************************************************/
void RTC_init(){
	/* Configura o PMC */
	pmc_enable_periph_clk(ID_RTC);
	
	/* Default RTC configuration, 24-hour mode */
	rtc_set_hour_mode(RTC, 0);

	/* Configura data e hora manualmente */
	rtc_set_date(RTC, YEAR, MOUNTH, DAY, WEEK);
	rtc_set_time(RTC, HOUR, MINUTE, SECOND);

	/* Configure RTC interrupts */
	NVIC_DisableIRQ(RTC_IRQn);
	NVIC_ClearPendingIRQ(RTC_IRQn);
	NVIC_SetPriority(RTC_IRQn, 2);
	NVIC_EnableIRQ(RTC_IRQn);
	
	/* Ativa interrupcao via alarme */
	rtc_enable_interrupt(RTC,  RTC_IER_ALREN);
	
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/
/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{

	/* Initialize the SAM system. */
	sysclk_init();
  ioport_init();
  board_init();
  RTC_init();
  
  /* inicializa delay */
  delay_init(sysclk_get_cpu_hz());

  /* inicializa console (printf) */
	configure_console();
  
	/* Output example information. */
	puts(STRING_HEADER);

  /************************************* 
   * Ativa e configura AFEC
   *************************************/  

  /* Ativa AFEC - 0 */
	afec_enable(AFEC0);

  /* struct de configuracao do AFEC */
	struct afec_config afec_cfg;

  /* Carrega parametros padrao */
	afec_get_config_defaults(&afec_cfg);

  /* Configura AFEC */
	afec_init(AFEC0, &afec_cfg);
  
  /* Configura trigger por software */
  afec_set_trigger(AFEC0, AFEC_TRIG_SW);
  
  /* configura call back */
 	afec_set_callback(AFEC0, AFEC_INTERRUPT_EOC_11,	AFEC_Temp_callback, 1); 
   
  /*** Configuracao específica do canal AFEC ***/
  struct afec_ch_config afec_ch_cfg;
  afec_ch_get_config_defaults(&afec_ch_cfg);
  afec_ch_cfg.gain = AFEC_GAINVALUE_0;
  afec_ch_set_config(AFEC0, AFEC_CHANNEL_TEMP_SENSOR, &afec_ch_cfg);
  
  /*
   * Calibracao:
	 * Because the internal ADC offset is 0x200, it should cancel it and shift
	 * down to 0.
	 */
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_TEMP_SENSOR, 0x200);

  /***  Configura sensor de temperatura ***/
	struct afec_temp_sensor_config afec_temp_sensor_cfg;

	afec_temp_sensor_get_config_defaults(&afec_temp_sensor_cfg);
	afec_temp_sensor_set_config(AFEC0, &afec_temp_sensor_cfg);

  /* Selecina canal e inicializa conversão */  
	afec_channel_enable(AFEC0, AFEC_CHANNEL_TEMP_SENSOR);
  afec_start_software_conversion(AFEC0);



    rtc_set_time_alarm(RTC, 1, HOUR, 1, MINUTE, 1, SECOND+1);
    
    
    while (1) {
	    /* Entra em modo sleep */
	    
    }
}

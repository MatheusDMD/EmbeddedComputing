/************************************************************************
* Rafael Corsi   - Insper
* rafael.corsi@insper.edu.br
*
* Computação Embarcada
*
* 10-PIO-INTERRUPCAO
*
* [ref] http://www.atmel.com/Images/Atmel-42142-SAM-AT03258-Using-Low-Power-Mode-in-SAM4E-Microcontroller_Application-Note.pdf
* [ref] https://www.eecs.umich.edu/courses/eecs373/labs/refs/M3%20Guide.pdf
************************************************************************/


#include "asf.h"
#include "conf_clock.h"



/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/**
 * LEDs
 */
#define LED_PIO_BOARD_ID		ID_PIOC
#define LED_PIO_BOARD           PIOC
#define LED_PIN_BOARD		    8
#define LED_PIN_BOARD_MASK      (1<<LED_PIN_BOARD)

#define LED_PIO_1_ID			ID_PIOA
#define LED_PIO_1				PIOA
#define LED_PIN_1				0
#define LED_PIN_1_MASK			(1<<LED_PIN_1)

#define LED_PIO_2_ID			ID_PIOC
#define LED_PIO_2				PIOC
#define LED_PIN_2				30
#define LED_PIN_2_MASK			(1<<LED_PIN_2)

#define LED_PIO_3_ID			ID_PIOB
#define LED_PIO_3				PIOB
#define LED_PIN_3				2
#define LED_PIN_3_MASK			(1<<LED_PIN_3)
/**
 * Botão
 */
#define BUT_PIO_BOARD_ID    ID_PIOA
#define BUT_PIO_BOARD		PIOA
#define BUT_PIO_1_ID		ID_PIOD
#define BUT_PIO_1			PIOD
#define BUT_PIO_2_ID		ID_PIOC
#define BUT_PIO_2			PIOC
#define BUT_PIO_3_ID		ID_PIOA
#define BUT_PIO_3			PIOA

#define BUT_PIN_BOARD   11
#define BUT_PIN_1	    28
#define BUT_PIN_2	    31
#define BUT_PIN_3	    19

#define BUT_PIN_BOARD_MASK    (1 << BUT_PIN_BOARD)
#define BUT_PIN_1_MASK		  (1 << BUT_PIN_1)
#define BUT_PIN_2_MASK		  (1 << BUT_PIN_2)
#define BUT_PIN_3_MASK		  (1 << BUT_PIN_3)

#define BUT_DEBOUNCING_VALUE  79

/************************************************************************/
/* global                                                               */
/************************************************************************/

int flag_handler_1 = 0;
int flag_handler_2 = 0;
int flag_handler_3 = 0;

/************************************************************************/
/* prototype                                                             */
/************************************************************************/
void led_init(uint32_t id, Pio *pio, uint32_t mask, uint32_t estado);
void but_init(Pio *pio, const uint32_t mask, uint32_t id, uint32_t type, void (*p_handler) (uint32_t, uint32_t) );
void but_Handler_Board();
void but_Handler_1();
void but_Handler_2();
void but_Handler_3();

/************************************************************************/
/* Interrupçcões                                                        */
/************************************************************************/

void but_Handler_Board(){
    /*
     *  limpa interrupcao do PIO
     */
    uint32_t pioIntStatus;
    pioIntStatus =  pio_get_interrupt_status(BUT_PIO_BOARD);
   
   /**
    *  Toggle status led
    */
   if(pio_get_output_data_status(LED_PIO_BOARD, LED_PIN_BOARD_MASK))
    pio_clear(LED_PIO_BOARD, LED_PIN_BOARD_MASK);
   else
    pio_set(LED_PIO_BOARD,LED_PIN_BOARD_MASK);
    
}

void but_Handler_1(){
    /*
     *  limpa interrupcao do PIO
     */
    uint32_t pioIntStatus;
    pioIntStatus =  pio_get_interrupt_status(BUT_PIO_1);
	flag_handler_1 = 1;
    
	/**
    *  Toggle status led
    */
   if(pio_get_output_data_status(LED_PIO_1, LED_PIN_1_MASK))
		pio_clear(LED_PIO_1, LED_PIN_1_MASK);
   else
		pio_set(LED_PIO_1,LED_PIN_1_MASK);
}

void but_Handler_2(){
    /*
     *  limpa interrupcao do PIO
     */
    uint32_t pioIntStatus;
    pioIntStatus =  pio_get_interrupt_status(BUT_PIO_2);
    flag_handler_2 = 1;
   /**
    *  Toggle status led
    */
   if(pio_get_output_data_status(LED_PIO_2, LED_PIN_2_MASK))
    pio_clear(LED_PIO_2, LED_PIN_2_MASK);
   else
    pio_set(LED_PIO_2,LED_PIN_2_MASK);
    
}

void but_Handler_3(){
    /*
     *  limpa interrupcao do PIO
     */
    uint32_t pioIntStatus;
    pioIntStatus =  pio_get_interrupt_status(BUT_PIO_3);
	flag_handler_3 = 1;
   /**
    *  Toggle status led
    */
   if(pio_get_output_data_status(LED_PIO_3, LED_PIN_3_MASK))
    pio_clear(LED_PIO_3, LED_PIN_3_MASK);
   else
    pio_set(LED_PIO_3,LED_PIN_3_MASK);
    
}

/************************************************************************/
/* Funções Aux                                                          */
/************************************************************************/

void led_blink(uint32_t original_state, int times){
	if(original_state == 0){
		for (int i = 0; i < times; i++)
		{
			pio_set(LED_PIO_BOARD, LED_PIN_BOARD_MASK);
			delay_s(0.1);
			pio_clear(LED_PIO_BOARD, LED_PIN_BOARD_MASK);
			delay_s(0.1);
		}
	}else{
		for (int i = 0; i < times; i++)
		{
			pio_set(LED_PIO_BOARD, LED_PIN_BOARD_MASK);

			pio_clear(LED_PIO_BOARD, LED_PIN_BOARD_MASK);
		}
	}
}


/************************************************************************/
/* Funções	                                                            */
/************************************************************************/

/**
 * @Brief Inicializa o pino do LED
 */
void led_init(uint32_t id, Pio *pio, uint32_t mask, uint32_t estado){
    pmc_enable_periph_clk(id);
    pio_set_output(pio, mask, 1, 0, 0);
};

/**
 * @Brief Inicializa o pino do BUT
 *  config. botao em modo entrada enquanto 
 *  ativa e configura sua interrupcao.
 */
void but_init(Pio *pio, const uint32_t mask, uint32_t id, uint32_t type, void (*p_handler) (uint32_t, uint32_t) ){
    /* config. pino botao em modo de entrada */
    pmc_enable_periph_clk(id);
    pio_set_input(pio, mask, PIO_PULLUP | PIO_DEBOUNCE);
    
    /* config. interrupcao em borda de descida no botao do kit */
    /* indica funcao (but_Handler) a ser chamada quando houver uma interrupção */
    pio_enable_interrupt(pio, mask);
	//PIO_IT_FALL_EDGE
    pio_handler_set(pio, id, mask, type, p_handler);
    
    /* habilita interrupçcão do PIO que controla o botao */
    /* e configura sua prioridade                        */
    NVIC_EnableIRQ(id);
    NVIC_SetPriority(id, 1);
};

/************************************************************************/
/* Main                                                                 */
/************************************************************************/
int main(void)
{
	/************************************************************************/
	/* Inicialização básica do uC                                           */
	/************************************************************************/
	sysclk_init();
	WDT->WDT_MR = WDT_MR_WDDIS;

	/************************************************************************/
	/* Inicializao I/OS                                                     */
	/************************************************************************/
	led_init(LED_PIO_BOARD_ID,LED_PIO_BOARD,LED_PIN_BOARD_MASK,1);
	led_init(LED_PIO_1_ID,LED_PIO_1,LED_PIN_1_MASK,1);
	led_init(LED_PIO_2_ID,LED_PIO_2,LED_PIN_2_MASK,1);
	led_init(LED_PIO_3_ID,LED_PIO_3,LED_PIN_3_MASK,1);

    but_init(BUT_PIO_BOARD,BUT_PIN_BOARD_MASK,BUT_PIO_BOARD_ID,PIO_IT_FALL_EDGE, but_Handler_Board);
	but_init(BUT_PIO_1,BUT_PIN_1_MASK,BUT_PIO_1_ID,PIO_IT_RISE_EDGE, but_Handler_1);
	but_init(BUT_PIO_2,BUT_PIN_2_MASK,BUT_PIO_2_ID,PIO_IT_FALL_EDGE, but_Handler_2);
	but_init(BUT_PIO_3,BUT_PIN_3_MASK,BUT_PIO_3_ID,PIO_IT_RE_OR_HL, but_Handler_3);

	/************************************************************************/
	/* Super loop                                                           */
	/************************************************************************/
	while(1){
       /* entra em modo sleep */
       pmc_sleep(SLEEPMGR_SLEEP_WFI);

	   if(flag_handler_1 == 1){	   
		flag_handler_1 = 0;
		led_blink(0,1);
	   }
	   else if(flag_handler_2 == 1){
		flag_handler_2 = 0;
		led_blink(0,2);
	   }
	   else if(flag_handler_3 == 1){
		flag_handler_3 = 0;
		led_blink(0,3);
	   }
	};
}




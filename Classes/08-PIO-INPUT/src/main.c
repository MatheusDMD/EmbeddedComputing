/**************************************************************************
* Rafael Corsi   - Insper 
* rafael.corsi@insper.edu.br        
*
* Computação Embarcada
*
* 08-PIO-ENTRADA
************************************************************************/


#include "asf.h"
#include "conf_clock.h"

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/**
 * LEDs
 */
#define LED_PIO_ID		ID_PIOC
#define LED_PIO         PIOC
#define LED_PIN			8
#define LED_PIN_MASK	(1<<LED_PIN) 

/**
 * Botão
 */ 
#define BUT_PIO_ID		10
#define BUT_PIO         PIOA
#define BUT_PIN			11
#define BUT_PIN_MASK	(1 << BUT_PIN)

/************************************************************************/
/* Prototipação                                                        */
/************************************************************************/
void ledConfig();

/************************************************************************/
/* Funções	                                                            */
/************************************************************************/



/**
 * @Brief Inicializa o pino do LED
 */
void ledConfig(){
	PMC->PMC_PCER0 = (1<<LED_PIO_ID);	
	PIOC->PIO_OER  = (1 << 8);
	PIOC->PIO_PER  = (1 << 8);
	PIOC->PIO_CODR = (1 << 8);
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
	/* Inicializa perifericos                                               */
	/************************************************************************/
	// Configura LED em modo saída
	ledConfig();

	// Configura botao
	PMC->PMC_PCER0= (1<<10);
	PIOA->PIO_PER = (1<<11);
	PIOA->PIO_ODR = (1<<11);
	PIOA->PIO_PUER= (1<<11);
	PIOA->PIO_IFER= (1<<11);
	
	//PINO DA PLACA ADJACENTE
	PMC->PMC_PCER0= (1<<ID_PIOD);
	PIOD->PIO_PER = (1<<28);
	PIOD->PIO_ODR = (1<<28);
	PIOD->PIO_PUER= (1<<28);
	PIOD->PIO_IFER= (1<<28);	
	
	/************************************************************************/
	/* Super loop                                                           */
	/************************************************************************/
	while(1){
		if( PIOD->PIO_PDSR & (1<<28) ){
			PIOC->PIO_SODR = (1 << 8);
		}
		else{
			PIOC->PIO_CODR = (1 << 8);
		}	  
	};
}
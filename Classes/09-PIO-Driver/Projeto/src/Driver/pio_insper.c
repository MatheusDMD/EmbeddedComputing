#include "pio_insper.h"

void _pio_set_output(	Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_default_level, const uint32_t ul_pull_up_enable){
	
	p_pio->PIO_PER  = ul_mask;           // Ativa controle do pino no PIO    (PIO ENABLE register)
	p_pio->PIO_OER  = ul_mask;           // Ativa saída                      (Output ENABLE register)

	if(!ul_pull_up_enable){
		p_pio->PIO_PUER	 = ul_mask;
	}
	if(!ul_default_level)                                 // Checa pela inicialização desejada
		p_pio->PIO_CODR = ul_mask;       // Coloca 0 na saída                (CLEAR Output Data register)
	else
		p_pio->PIO_SODR = ul_mask;       // Coloca 1 na saída                (SET Output Data register)
}

void _pio_set_input( 	Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_attribute){
	
	p_pio->PIO_ODR	 = ul_mask;        // Desativa saída                   (Output DISABLE register)
	p_pio->PIO_PER	 = ul_mask;        // Ativa controle do pino no PIO    (PIO ENABLE register)
	_pio_pull_up(p_pio,ul_mask,PIO_PULLUP & ul_attribute);
	if(PIO_DEBOUNCE & ul_attribute){
		p_pio->PIO_IFER	 = ul_mask;        // Ativa debouncing
	}
}

void _pio_pull_up(	Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_pull_up_enable){
		if(ul_pull_up_enable){
			p_pio->PIO_PUER	 = ul_mask;        // Ativa pull-up no PIO             (PullUp ENABLE register)
		}else{
			p_pio->PIO_PUDR	 = ul_mask;        // Ativa pull-up no PIO             (PullUp DISABLE register)
		}
}

void _pio_pull_down( Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_pull_down_enable){
		if(ul_pull_down_enable){
			p_pio->PIO_PPDER	 = ul_mask;        // Ativa pull-up no PIO             (PullUp ENABLE register)
		}else{
			p_pio->PIO_PPDDR	 = ul_mask;        // Ativa pull-up no PIO             (PullUp ENABLE register)
		}
}

void _pio_set(   Pio *p_pio, const uint32_t ul_mask){
	p_pio->PIO_SODR = ul_mask;       // Coloca 1 na saída                (SET Output Data register)
}


void _pio_clear( Pio *p_pio, const uint32_t ul_mask){
	p_pio->PIO_CODR = ul_mask;       // Coloca 0 na saída                (CLEAR Output Data register)
}

uint32_t _pio_get_output_data_status(const Pio *p_pio, const uint32_t ul_mask){
	return p_pio->PIO_PDSR & (ul_mask);
}
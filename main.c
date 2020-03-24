#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "serial.h"
#include "vt100.h"
#include "start.h"
#include "game.h"


int main(void) {
	/*	INITIALISATION VITESSE DE COMMUNICATION STM32F4_DISCOVERY - TERMINAL	*/
	serial_init(115200);

	/*	NETTOYAGE ÉCRAN AU DÉMARRAGE	*/
	vt100_clear_screen();

	/*	DEFINITION FENÊTRE DE JEU	*/
	window('#');

	/*	PAGE D'ACCUEIL	*/
	home_page();

	/*	LANCEMENT PAGE D'ACCUEIL	*/
	game_launch();

	/*	INITIALISATION DU JOUEUR	*/
	init_player();

	/*	INITIALISATION DES ENNEMIES	*/
	init_enemy();

	vt100_move(2, 2);
	serial_puts("LIFE : 3");

	/* Initialize LEDs */
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);

	/* Turn on LEDs */
	STM_EVAL_LEDOn(LED3);
	STM_EVAL_LEDOn(LED4);
	STM_EVAL_LEDOn(LED5);
	STM_EVAL_LEDOn(LED6);

	/*	BOUCLE INFINIE	*/
	while (1) {
		signed char var = serial_get_last_char();
		if (var == ' ') {
			player_fire();
			life_Player_display();
			score_display();
		}
		player_movement(var);
		enemy_fire();
		timer();
	}
}

/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size) {
	/* TODO, implement your code here */
	return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void) {
	/* TODO, implement your code here */
	return -1;
}

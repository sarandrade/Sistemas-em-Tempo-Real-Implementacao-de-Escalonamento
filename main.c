/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
 * This project provides two demo applications.  A simple blinky style project,
 * and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is used to select between the two.
 * The simply blinky demo is implemented and described in main_blinky.c.  The
 * more comprehensive test and demo application is implemented and described in
 * main_full.c.
 *
 * This file implements the code that is not demo specific, including the
 * hardware setup and FreeRTOS hook functions.
 *
 *******************************************************************************
 * NOTE: Windows will not be running the FreeRTOS demo threads continuously, so
 * do not expect to get real time behaviour from the FreeRTOS Windows port, or
 * this demo application.  Also, the timing information in the FreeRTOS+Trace
 * logs have no meaningful units.  See the documentation page for the Windows
 * port for further information:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 *

 *
 *******************************************************************************
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <time.h> 
#include <math.h>
#include <string.h>

/* This project provides two demo applications.  A simple blinky style demo
application, and a more comprehensive test and demo application.  The
mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is used to select between the two.

If mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is 1 then the blinky demo will be built.
The blinky demo is implemented and described in main_blinky.c.

If mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is not 1 then the comprehensive test and
demo application will be built.  The comprehensive test and demo application is
implemented and described in main_full.c. */
#define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY	1

/* This demo uses heap_5.c, and these constants define the sizes of the regions
that make up the total heap.  heap_5 is only used for test and example purposes
as this demo could easily create one large heap region instead of multiple
smaller heap regions - in which case heap_4.c would be the more appropriate
choice.  See http://www.freertos.org/a00111.html for an explanation. */
#define mainREGION_1_SIZE	7201
#define mainREGION_2_SIZE	29905
#define mainREGION_3_SIZE	6407

/*-----------------------------------------------------------*/

/*
 * main_blinky() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1.
 * main_full() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 0.
 */
//extern void main_blinky( void );
//extern void main_full( void );

/*
 * Only the comprehensive demo uses application hook (callback) functions.  See
 * http://www.freertos.org/a00016.html for more information.
 */
//void vFullDemoTickHookFunction( void );
//void vFullDemoIdleFunction( void );

/*
 * This demo uses heap_5.c, so start by defining some heap regions.  It is not
 * necessary for this demo to use heap_5, as it could define one large heap
 * region.  Heap_5 is only used for test and example purposes.  See
 * http://www.freertos.org/a00111.html for an explanation.
 */
static void  prvInitialiseHeap( void );

/*
 * Prototypes for the standard FreeRTOS application hook (callback) functions
 * implemented within this file.  See http://www.freertos.org/a00016.html .
 */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/*
 * Writes trace data to a disk file when the trace recording is stopped.
 * This function will simply overwrite any trace files that already exist.
 */
static void prvSaveTraceFile( void );

/*-----------------------------------------------------------*/

/* When configSUPPORT_STATIC_ALLOCATION is set to 1 the application writer can
use a callback function to optionally provide the memory required by the idle
and timer tasks.  This is the stack that will be used by the timer task.  It is
declared here, as a global, so it can be checked by a test that is implemented
in a different file. */
StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

/* Notes if the trace is running or not. */
static BaseType_t xTraceRunning = pdTRUE;


/*############################ Implementação do jogo ZigZag ############################*/

/*  # Disciplina: Sistemas em Tempo Real
	# Professor: Danilo Freire de Souza Santos
	# Aluna: Sara Andrade Dias - 116110719
	# Projeto: Implementação do jogo ZigZag
*/

/* Criando as Task Handlers 
	-> T1 - Atualiza display 
	-> T2 - Cria caminho 
	-> T3 - Lê comando do jogador 
	-> T4 - Adiciona diamante 
	-> T5 - Checa fim do jogo 
*/
xTaskHandle HAtualizaDisplay;
xTaskHandle HCriaCaminho;
xTaskHandle HLeComandoDoJogador;
xTaskHandle HAdicionaDiamante;
xTaskHandle HChecaFimDoJogo;

/* Inicializando os valores dos tempos de execução das tarefas periódicas (ms)
	-> T1 - Atualiza display (e = 3ms)
	-> T2 - Cria caminho (e = 3ms)
	-> T4 - Adiciona diamante (e = 0.5s = 500ms)
	-> T5 - Checa fim do jogo (e = 1ms)
	-> E = e = Tempo de Execução
*/
int	E_ATUALIZA_DISPLAY = 3;
int E_CRIA_CAMINHO = 3;
int E_ADICIONA_DIAMANTE = 500;
int E_CHECA_FIM_DO_JOGO = 1;

/* Inicializando os valores dos períodos das tarefas periódicas (ms)
	-> T1 - Atualiza display (P = 20ms)
	-> T2 - Cria caminho (P = 20ms)
	-> T4 - Adiciona diamante (P = 5s = 5000ms)
	-> T5 - Checa fim do jogo (P = 5ms)
	-> P = Período
*/
int P_ATUALIZA_DISPLAY = 20;
int P_CRIA_CAMINHO = 20;
int P_ADICIONA_DIAMANTE = 5000;
int P_CHECA_FIM_DO_JOGO = 5;

/* Inicializando váriavel que indica fim da partida */
bool fim_de_jogo = FALSE;

/* Variável que acompanha o tempo de execução das tarefas sendo executadas */
int tempo_para_executar = 0;

/* Inicializando os contadores necessários nas tarefas: T1, T2, T3 e T5.
   Qaundo o sistema reinicia, todos os contadores também são reiniciados.*/

int contador_T1 = 0;
int contador_T2 = 0;
int contador_T3 = 0;
int contador_T5 = 0;


/* ---------- Funções das Tarefas do Sistema ---------- */

/* Para implementar as tarefas foram utilizadas funções mock, que são uma versão falsa de um serviço 
   externo ou interno que pode substituir o serviço real, ajudando seus testes de escalonamento. */

/* T1 - Atualiza display: P = D(hard) = 20ms; e = 3ms */
void AtualizaDisplay() {
	/* Essa função faz a atualização das informações do jogo no display. 
	   Para simular o tempo de execução dessa tarefa (3ms) foi utilizado a funçao delay(). */

	TickType_t UltimaAtualizacao;
	
	while (1) {
		
		/* A mensagem será apresentada a cada 1s -> 50 * 20 (período da tarefa) = 1000ms = 1s */
		if (contador_T1 % 50 == 0) {
			printf("-> O display foi atualizado > %d vezes \n", contador_T1);
			fflush(stdout);
		}

		/* Incrementando contador de T1 */
		contador_T1++;

		/* Simulando o tempo de execução */
		delay(E_ATUALIZA_DISPLAY);
		
		UltimaAtualizacao = xTaskGetTickCount(); 
		vTaskDelayUntil(&UltimaAtualizacao, P_ATUALIZA_DISPLAY/2);
	}
}

/* T2 - Cria caminho: P = D(soft) = 20ms; e = 3ms */
void CriaCaminho() {
	/* Essa função cria o caminho a frente que deve ser atuiaizado no display.
	   Para simular o tempo de execução dessa tarefa (3ms) foi utilizado a funçao delay(). */

	TickType_t UltimaAtualizacao;

	while (1) {

		/* A mensagem será apresentada a cada 2s -> 100 * 20 (período da tarefa) = 2000ms = 2s */
		if (contador_T2 % 100 == 0) {
			printf("-> O caminho foi atualizado > %d vezes \n", contador_T2);
			fflush(stdout);
		}

		/* Incrementando contador de T2 */
		contador_T2++;

		/* Simulando o tempo de execução */
		delay(E_CRIA_CAMINHO);
		
		UltimaAtualizacao = xTaskGetTickCount();
		vTaskDelayUntil(&UltimaAtualizacao, P_CRIA_CAMINHO/2);
	}
}

/* T4 - Adiciona diamante: P = D(soft) = 5s; e = 0.5s */
void AdicionaDiamante() {
	/* Essa função adiciona um diamante novo no caminho a cada 5s.
	   Para simular o tempo de execução dessa tarefa (0.5s) foi utilizado a funçao delay(). */

	TickType_t UltimaAtualizacao;

	while (1) {
		
		/* Simulando o tempo de execução */
		delay(E_ADICIONA_DIAMANTE);

		/* A mensagem será apresentada a cada 5s (período da tarefa) */
		printf("-> Novo Diamante!! \n")
		fflush(stdout);

		UltimaAtualizacao = xTaskGetTickCount();
		vTaskDelayUntil(&UltimaAtualizacao, P_ADICIONA_DIAMANTE/2);
	}
}

/* T5 - Checa fim do jogo: P = D(hard) = 5ms; e = 1ms */
void ChecaFimDoJogo() {
	/* Essa função verifica se a partida chegou ao fim, ou seja, a bola caiu.
	   Para simular o tempo de execução dessa tarefa (1ms) foi utilizado a funçao delay(). */

	TickType_t UltimaAtualizacao;

	while (1) {

		/* Simulando o tempo de execução */
		delay(E_CHECA_FIM_DO_JOGO);
		UltimaAtualizacao = xTaskGetTickCount();

		if (fim_de_jogo == FALSE) {
			/* A mensagem será apresentada a cada 1s -> 200 * 5 (período da tarefa) = 1000ms = 1s */
			if (contador_T5 % 200 == 0) {
				printf("-> Fim do Jogo Verificado > %d vezes \n", contador_T5);
				fflush(stdout);
			}

			/* Incrementando contador de T5 */
			contador_T5++;
		}
		else {
			/* A mensagem será apresentada quando ESC for pressionado, simulando fim da partida */
			printf("### A bolinha caiu ### \n");
			printf("### Fim de Jogo ### \n");

			finaliza_partida();
		}
		vTaskDelayUntil(&UltimaAtualizacao, P_CHECA_FIM_DO_JOGO/2);
	}
}

/* T3 - Lê comando do jogador: TE = Tarefa Esporádica; D(hard) = 35ms; e = 3ms */
static void LeComandoDoJogador() {
	/* Essa função lê os comandos feitos pelo jogador.
	   Para simular o tempo de execução dessa tarefa (3ms) foi utilizado a funçao delay(). */

	while (1) {
		/* Se alguma tecla foi pressionada */
		if (_kbhit() != 0) {

			int tecla = getch();

			/* Se a tecla ESC for pressionada */
			if (tecla == 27) {
				/* Simula a queda da bolinha */
				fim_de_jogo = TRUE; 
			}
			/* Se a barra de espaço for pressionada */
			else if (tecla == 32) {
				if (contador_T3 == 0) {
					printf("Mudando sentido da bolinha para > Direita");
					contador_T3 = 1;
				}
				else if (contador_T3 == 1){
					printf("Mudando sentido da bolinha para > Esquerda");
					contador_T3 = 0;
				}
			}
		}
		vTaskDelay(40);
	}
}


/* --------------- Funções Auxiliares --------------- */


void delay(int tempo_execucao){
	/* Essa função simula o tempo de execução das tarefas do sistema */

	/* Adicionando tempo de execução à pilha de tempo de execução */
	int tempo_de_sobra = tempo_para_executar;
	tempo_para_executar += tempo_execucao;

	/* Armazenando o tempo inicial */
	TickType_t tempo_inicial = xTaskGetTickCount();

	/* Loop até que o tempo de execução da tarefa seja alcançado */
	while (xTaskGetTickCount() < tempo_inicial + tempo_para_executar - tempo_de_sobra);

	tempo_para_executar = tempo_de_sobra + tempo_execucao;
}

void finaliza_partida() {
	/* Essa função, ao fim de uma partida, oferece a opção de recomeçar outra partida ou sair do jogo. */

	char resposta;
	while (1) {
		printf("## Para jogar novamente pressione - 1 - ## \n ## Para sair do jogo pressione - 0 - ## \n");
		scanf("%c", &resposta);
		scanf("%c", &resposta); // precisa de dois??????????

		if (resposta == 1) {
			system("cls");
			printf("## NOVA PARTIDA ## \n");

			/* Reiniciando as variáveis */
			fim_de_jogo = FALSE;
			contador_T1 = 0;
			contador_T2 = 0;
			contador_T5 = 0;

			break;
		}

		else if (resposta == 0) {

			printf("## Até a Próxima ;) ##");
			Sleep(1000);

			/* Encerra o escalonador do sistema e o programa */
			vTaskEndScheduler();

			break;
		}
	}
}


/* ---------------------- MAIN ---------------------- */


int main( void )
{
	/* This demo uses heap_5.c, so start by defining some heap regions.  heap_5
	is only used for test and example reasons.  Heap_4 is more appropriate.  See
	http://www.freertos.org/a00111.html for an explanation. */
	prvInitialiseHeap();

	/* Initialise the trace recorder.  Use of the trace recorder is optional.
	See http://www.FreeRTOS.org/trace for more information. */
	vTraceEnable( TRC_START );

	/* Criando as tarefas 
	   -> Prioridades: T3 > T5 > T1 > T2 > T4 */

	xTaskCreate(AtualizaDisplay, (signed char*)"Atualiza Display", configMINIMAL_STACK_SIZE, NULL, 3, &HAtualizaDisplay);
	xTaskCreate(CriaCaminho, (signed char*)"Cria Caminho", configMINIMAL_STACK_SIZE, NULL, 2, &HCriaCaminho);
	xTaskCreate(LeComandoDoJogador, (signed char*)"Le Comando do Jogador", configMINIMAL_STACK_SIZE, NULL, 5, &HLeComandoDoJogador);
	xTaskCreate(AdicionaDiamante, (signed char*)"Adiciona Diamante", configMINIMAL_STACK_SIZE, NULL, 1, &HAdicionaDiamante);
	xTaskCreate(ChecaFimDoJogo, (signed char*)"Checa Fim do Jogo", configMINIMAL_STACK_SIZE, NULL, 4, &HChecaFimDoJogo);
	
	/* Criando o menu do jogo */

	int menu = 0;

	while (menu != 1) {
		system("cls");
		printf("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- \n");
		printf("----------------------------   ZigZag   ----------------------------- \n");
		printf("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- \n");
		printf("-> Para começar a partida pressione - 1 - \n");
		printf("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- \n");
		printf("-> Digite aqui: ");
		scanf("%d", &menu);
		printf("\n-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- \n");
		printf("-> Para mudar o sentido da bolinha pressione a - barra de espaço - \n");
		printf("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- \n");
		printf("---------------------------- Bom Jogo ;p ---------------------------- \n");
	}

	system("cls");

	vTaskStartScheduler();
	for (;;);
	return 0;
}


/* ############################ Fim da Implementação do jogo ZigZag ############################ */


void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c, heap_2.c or heap_4.c is being used, then the
	size of the	heap available to pvPortMalloc() is defined by
	configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the xPortGetFreeHeapSize()
	API function can be used to query the size of free heap space that remains
	(although it does not provide information on how the remaining heap might be
	fragmented).  See http://www.freertos.org/a00111.html for more
	information. */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If application tasks make use of the
	vTaskDelete() API function to delete themselves then it is also important
	that vApplicationIdleHook() is permitted to return to its calling function,
	because it is the responsibility of the idle task to clean up memory
	allocated by the kernel to any task that has since deleted itself. */

	/* Uncomment the following code to allow the trace to be stopped with any
	key press.  The code is commented out by default as the kbhit() function
	interferes with the run time behaviour. */
	/*
		if( _kbhit() != pdFALSE )
		{
			if( xTraceRunning == pdTRUE )
			{
				vTraceStop();
				prvSaveTraceFile();
				xTraceRunning = pdFALSE;
			}
		}
	*/

	#if ( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY != 1 )
	{
		/* Call the idle task processing used by the full demo.  The simple
		blinky demo does not use the idle task hook. */
		vFullDemoIdleFunction();
	}
	#endif
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  This function is
	provided as an example only as stack overflow checking does not function
	when running the FreeRTOS Windows port. */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
	#if ( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY != 1 )
	{
		vFullDemoTickHookFunction();
	}
	#endif /* mainCREATE_SIMPLE_BLINKY_DEMO_ONLY */
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
	/* This function will be called once only, when the daemon task starts to
	execute	(sometimes called the timer task).  This is useful if the
	application includes initialisation code that would benefit from executing
	after the scheduler has been started. */
}
/*-----------------------------------------------------------*/

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
static BaseType_t xPrinted = pdFALSE;
volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

	/* Called if an assertion passed to configASSERT() fails.  See
	http://www.freertos.org/a00110.html#configASSERT for more information. */

	/* Parameters are not used. */
	( void ) ulLine;
	( void ) pcFileName;

	printf( "ASSERT! Line %ld, file %s, GetLastError() %ld\r\n", ulLine, pcFileName, GetLastError() );

 	taskENTER_CRITICAL();
	{
		/* Stop the trace recording. */
		if( xPrinted == pdFALSE )
		{
			xPrinted = pdTRUE;
			if( xTraceRunning == pdTRUE )
			{
				vTraceStop();
				prvSaveTraceFile();
			}
		}

		/* You can step out of this function to debug the assertion by using
		the debugger to set ulSetToNonZeroInDebuggerToContinue to a non-zero
		value. */
		while( ulSetToNonZeroInDebuggerToContinue == 0 )
		{
			__asm{ NOP };
			__asm{ NOP };
		}
	}
	taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

static void prvSaveTraceFile( void )
{
FILE* pxOutputFile;

	fopen_s( &pxOutputFile, "Trace.dump", "wb");

	if( pxOutputFile != NULL )
	{
		fwrite( RecorderDataPtr, sizeof( RecorderDataType ), 1, pxOutputFile );
		fclose( pxOutputFile );
		printf( "\r\nTrace output saved to Trace.dump\r\n" );
	}
	else
	{
		printf( "\r\nFailed to create trace dump file\r\n" );
	}
}
/*-----------------------------------------------------------*/

static void  prvInitialiseHeap( void )
{
/* The Windows demo could create one large heap region, in which case it would
be appropriate to use heap_4.  However, purely for demonstration purposes,
heap_5 is used instead, so start by defining some heap regions.  No
initialisation is required when any other heap implementation is used.  See
http://www.freertos.org/a00111.html for more information.

The xHeapRegions structure requires the regions to be defined in start address
order, so this just creates one big array, then populates the structure with
offsets into the array - with gaps in between and messy alignment just for test
purposes. */
static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
volatile uint32_t ulAdditionalOffset = 19; /* Just to prevent 'condition is always true' warnings in configASSERT(). */
const HeapRegion_t xHeapRegions[] =
{
	/* Start address with dummy offsets						Size */
	{ ucHeap + 1,											mainREGION_1_SIZE },
	{ ucHeap + 15 + mainREGION_1_SIZE,						mainREGION_2_SIZE },
	{ ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE,	mainREGION_3_SIZE },
	{ NULL, 0 }
};

	/* Sanity check that the sizes and offsets defined actually fit into the
	array. */
	configASSERT( ( ulAdditionalOffset + mainREGION_1_SIZE + mainREGION_2_SIZE + mainREGION_3_SIZE ) < configTOTAL_HEAP_SIZE );

	/* Prevent compiler warnings when configASSERT() is not defined. */
	( void ) ulAdditionalOffset;

	vPortDefineHeapRegions( xHeapRegions );
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
	state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
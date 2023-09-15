/* Modelo MM1 - Un Servidor, Una Cola  */

#include <stdio.h>
#include <stdlib.h> 
#include "simlib.c"

// definición de eventos
#define Inicio_día        1  /*Evento inicio día           */
#define Fin_día           2  /* Tipo de Evento 2: Partidas          */
#define arribo_cliente    3
#define partida_caja      4

// definición colas y servidores
#define Cola_caja_1  1 /* Lista 1: Cola  */
#define Cola_caja_2  2
#define Cola_caja_rapida   3  /* Lista 2: Cola */
#define Cola_Cajas
#define Servidor_caja_1    4/* Lista  3 : servidor*/
#define Servidor_caja_2    5
#define Servidor_caja_rapida 6

// definición demoras
#define Demora_cola_cajas_comunes       1  /* Sampst 1: Demora en Cola            */
#define Demora_cola_caja_rapida

/* Declaraci¢n de variables propias */
bool super_abierto
float media_interarribos, min_tiempo_recorrido, max_tiempo_recorrido,probabilidad_cliente,min_cajas_comunes, max_cajas_comunes, min_caja_rapida, max_caja_rapida, cont_clientes, tipocaja , rápida=1, comunes=2,  tiempo_activo, tiempo_inactivo, cant_días;
int num_clientes, clientes_act, i;

// Definición posiciones transfer
#define Tiempo_Evento   1
#define Tipo_Evento     2
#define Caja            3
/* Declaraci¢n de rutinas propias */

void inicializa(void);
void Rutina_inicio_día(void);
void Rutina_fin_día(void);
void Rutina_arribo_cliente(void);
void Rutina_partida_caja(void);
void estadísticos(void);


int main()  /* Main function. */
{
	/* Apertura de Archivos que sean necesarios */

	float minimo, maximo;


	/* Initializar Simlib */
	init_simlib();


	/* Establecer maxatr = M ximo n£mero de Atributos utilizados  */
	maxatr = 4;

	/* Initializar el Sistema */
	inicializa();

	// Comienzo
	while (sim_time < cant_dias*8*60)
	{

		/* Determinar pr¢ximo Evento */
		timing();

		/* Invocar la función correspondiente. */

		switch( next_event_type )
		{
			case Inicio_día:
				Rutina_inicio_día();
				break;
			case Fin_día:
				Rutina_fin_día();
				break;
			case arribo_cliente:
				Rutina_arribo_cliente();
				break;
			case partida_caja:
				Rutina_partida_caja();
				break;
			default;
			break;
        }
	}

	/* Llamada al Reporte para mostrar los resultados */
	reporte();
	//getchar();
	system("pause");
}


void inicializa(void)  /* Inicializar el Sistema */
{
	// Se asignan los valores a las variables
	media_interarribos = 3.8;
	min_tiempo_recorrido = 5;
	max_tiempo_recorrido = 35;
	porcentaje_clientes_no_compran = 5;
	porcentaje_caja_rapida = 35;
	min_cajas_comunes = 2;
	max_cajas_comunes = 5;
	min_caja_rapida = 1.5;
	max_caja_rapida = 2.5;
	cont_clientes = 0;
	tiempo_activo = 480;
	tiempo_inactivo = 480;
	cont_no_compran = 0;
	cont_clientes=0
	super_abierto = false;
	/* Se carga el primer inicio de día */

	transfer[Tipo_Evento] = Inicio_día;
	transfer[Tiempo_Evento] = sim_time + tiempo_inactivo;

	// Se pronostica el primer arribo de cliente
	cont_clientes++;
	transfer[1] = sim_time + expon(media_interarribos,arribo_cliente);
	transfer[2] = arribo_cliente;
	list_file(INCREASING,LIST_EVENT);

}


void Rutina_arribo_cliente(void)  /* Evento Arribo */
{
	/* Determinar pr¢ximo arribo y cargar en Lista de Eventos */
	cont_clientes++;
	transfer[Tiempo_Evento] = sim_time + expon(media_interarribos,arribo_cliente);
	transfer[Tipo_Evento] = arribo_cliente;
	list_file(INCREASING,LIST_EVENT);

	/* Chequear si el Servidor est  desocupado */
	probabilidad_cliente = lcgrand(arribo_cliente);
	if (probabilidad_cliente <= 0.05)
		{
			cont_no_compran++;
		}
	else if (probabilidad_cliente <=0.35)
		{
		  if (list_size[Servidor_caja_rapida]==0 )
			 {
				list_file(FIRST, Servidor_caja_rapida);
				sampst(0, 0, Demora_cola_caja_rapida);
				transfer[Tiempo_Evento]=sim_time+uniform(min_caja_rapida, max_caja_rapida)
			 }
		}
	else
	{
		if (list_size[Servidor_caja_1] && list_size[Servidor_caja_2] == 0)
		{
			
			if (list_size[Cola_caja_1]< list_size[Cola_caja_2])

			/* Si está desocupado ocuparlo y generar la partida */
			{
				list_file(FIRST, Servidor_caja_1);
			}
			else
			{
				list_file(FIRST, Servidor_caja_2)
			}
			sampst(0.0, Demora_cola_cajas_comunes);
			transfer[Tiempo_Evento] = sim_time + uniform(min_cajas_comunes, max_cajas_comunes);
			transfer[Tipo_Evento] = partida_caja ;
			list_file(INCREASING, LIST_EVENT);

		}

		else
		{

			/* Si el Servidor est  ocupado poner el Trabajo en Cola */

			transfer[Tiempo_Evento] = sim_time;
			list_file(LAST, Cola);
		}

	}
	}
		


void Rutina_partida_caja(void)  /* Evento Partida */
{
	int TipoCaja = transfer[Caja];
	/* Desocupar el Servidor */
	if (TipoCaja==1)
		{
			list_remove(FIRST, Servidor_caja_rapida);

	/* Ver si hay trabajos en cola */

		if (list_size[Cola_caja_rapida] > 0)
		{

	   /* Sacar el primero de la cola y actualizar Demoras */

	   list_remove(FIRST, Cola_caja_rapida);
	   sampst(sim_time - transfer[1], Demora_cola_caja_rapida);

	   /* Cargar en el Servidor y generar la partida */
	   list_file(FIRST, Servidor_caja_rapida);

	   transfer[Tiempo_Evento] = sim_time + uniform(min_caja_rapida, max_caja_rapida, partida_caja);
	   transfer[Tipo_Evento] = partida_caja;
	   transfer[Caja] = 1;
	   list_file(INCREASING,LIST_EVENT);
		}
	}
	else
	{
		if (list_size[Servidor_caja_1]==0)
			{
			  if (list_size[Cola_caja_2] > 0)
				{
				  list_remove(FIRST, Cola_caja_2);
				  sampst(sim_time - transfer[1], Demora_cola_cajas_comunes);
				  list_file(FIRST, Cola_caja_1);

				}
			}
		else
		{
			if (list_size[Servidor_caja_2]==0)
				{
				  if (list_size[Cola_caja_1] > 0)
					 {
						 list_remove(FIRST, Cola_caja_1);
						 sampst(sim_time - transfer[1], Demora_cola_cajas_comunes);
						 list_file(FIRST, Cola_caja_2);
					 }
				}
		}
		else if (list_size[Servidor_caja_1] > 0)
			{
			  list_remove(FIRST, Servidor_caja_1)

			}
		else
		{
		   if (list_size[Servidor_caja_2] > 0)
			 {
			   list_remove(FIRST, Servidor_caja_2)
		     }
		}
		  sampst(sim_time - transfer[1], Demora_cola_cajas_comunes);
		  list_file(FIRST, Caja)
	}
	}


void estadisticos( void )  /* Generar Reporte de la Simulaci¢n */
{

	/* Mostrar Par metros de Entrada */

	/* -------- Por pantalla -------- */
	printf("Comienza reporte... \n\n");

	///*Demora media de los clientes  en Cola de Caja rápida.
	sampst(0.0, -Demora_cola_caja_rapida);
	printf("\nDemora en cola rápida : %f \n ", transfer[1]);

	// Demora media de los clientes en cajas comunes
	sampst(0.0, -Demora_cola_cajas_comunes);
	printf("\nDemora en cola común: %f \n ", transfer[1]);
	//	Número medio de clientes en la Cola de Cajas.

	filest(Cola_Cajas);
	printf("\nN£mero Promedio en Cola de cajas: %f \n ", transfer[1]);

	//	Número medio de clientes que vienen por primera vez en la Cola de Espera del área de Créditos

	//	Utilización promedio de las cajas.

	filest(Servidor_caja_rapida);
	printf("\nUtilizaci¢n caja rápida: %f \n ", transfer[1]);

	filest(Servidor_caja_1);
	printf("\nUtilizaci¢n caja 1: %f \n ", transfer[1]);

	filest(Servidor_caja_2);
	printf("\nUtilizaci¢n caja 2: %f \n ", transfer[1]);

	 // Cantidad de clientes que se retiran sin comprar
	printf("\nclientes sin comprar: %f \n ", transfer[1]);
	 
	//	Porcentaje de clientes que se cambian de cola antes de ser atendidos
	printf("\nporcentaje clientes cambiados de cola: %f \n ", transfer[1]);
	//	Número medio de clientes en el banco*/
}
}



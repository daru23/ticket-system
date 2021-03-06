#include "env.h"
#include<stdio.h>       // library io
#include<stdlib.h>      // library rand()
#include<time.h>        // library time()
#include <string.h>     // library strings
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "global.h"	// library MD5
#include "md5.h"   	// library MD5

unsigned char CODIFICADA[16];

/* Prints a message digest in hexadecimal. */
static void MDPrint (digest)
unsigned char digest[16];
{
	unsigned int i;
	for (i = 0; i < 16; i++)
		printf ("%02x", digest[i]);
}


/**
 * Take a string and codify it
 * @param string
 * @return digest.
 */
static void MDString (string)
char *string;
{
	MD5_CTX context;
	unsigned char digest[16];
	unsigned int len = strlen (string);

	MD5Init (&context);
	MD5Update (&context, string, len);
	MD5Final (digest, &context);

	sprintf (CODIFICADA, "%s",digest);
}

/* Prints a message digest in hexadecimal. */
static char * MDCadena (digest)
unsigned char digest[16];
{
	char *cadena;
	cadena = (char *)malloc(16);
	if (cadena == NULL)
		perror("Al reservar memoria.");		
	unsigned int i;
	for (i = 0; i < 16; i++){
		if (i == 0)
			sprintf (cadena,"%02x", digest[i]);
		else
			sprintf (cadena,"%s%02x", cadena, digest[i]);

	}
	return cadena;
}


char ** env_ip_1_svc(int * entero, struct svc_req * req){
        static char* ip;
        ip = (char *)malloc(255);
        if (ip == NULL)
                perror("Al reservar memoria.");
        /* Ask my IP */
        struct sockaddr whereto;
        struct hostent *hp;
        struct sockaddr_in *to;
        char *target;
        char *hostname;

        struct sockaddr_in host;
        char nombre[255];
        //char *ip;
        gethostname(nombre, 255);
        host.sin_addr = * (struct in_addr*) gethostbyname(nombre)->h_addr;
        sprintf(ip,"%s",inet_ntoa(host.sin_addr));

        return &ip;
}

char ** enviar_ticket_1_svc(char** cliente_ip, struct svc_req * req){

        int num;
        int hora = time(NULL);
        unsigned char codif;
	static char* ip_clt;
	ip_clt = (char *)malloc(16);
	sprintf(ip_clt,"%s",*cliente_ip);
        /* rand();  */
        srand(hora);

        /* Rand number */
        num = rand()%100;

        /* Ask the server for the date */

        time_t tiempo = time(0);
        struct tm *tlocal = localtime(&tiempo);
        char *fecha;
        fecha = (char *)malloc(128);
        if (fecha == NULL)
                perror("Al reservar memoria.");
        strftime(fecha,128,"%d/%m/%y %H:%M:%S",tlocal);

        /* Ask my IP */

        struct sockaddr whereto;
        struct hostent *hp;
        struct sockaddr_in *to;
        char *target;
        char *hostname;

        struct sockaddr_in host;
        char nombre[255];
        gethostname(nombre, 255);
        host.sin_addr = * (struct in_addr*) gethostbyname(nombre)->h_addr;
        char *ip;
        ip = (char *)malloc(255);
        if (ip == NULL)
                perror("Al reservar memoria.");
        sprintf(ip,"%s",inet_ntoa(host.sin_addr));

        /* Create ticket */

        static char *ticket;
        ticket = (char *)malloc(255);
        if (ticket == NULL)
                perror("Al reservar memoria.");
        sprintf(ticket,"%d&%s&%s&%s&%d",num,ip_clt,ip,fecha,0);
	
	/* Save tickets created */

	/* Folder of Reports */
	return &ticket;	

	FILE *fp;
	char *nomFich;
	char *linea;
	nomFich = (char *)malloc(10);
	sprintf(nomFich,"%s","histo.txt ");
	fp = fopen (nomFich, "w+" );
	linea = (char *)malloc(16);
	sprintf(linea,"%s",ticket);
	fputs(linea,fp);
	fclose (fp);

	/* Free Memory */	
	free(ip_clt);
	free(ip);
	free(ticket);
	free(linea);
	free(nomFich);



}

int * verificar_ticket_1_svc(char** ticket, struct svc_req * req){

	char* numCod;
	int num;
	int hora;
	char* serv;
	char* clt;
	char* fecha;
	char* cad;

	static char* t;
	t = (char *)malloc(300);
	//sprintf(t,"%s",*ticket);

	numCod = (char *)malloc(16);
        if (numCod == NULL)
                perror("Al reservar memoria.");


	serv = (char *)malloc(16);
        if (serv == NULL)
                perror("Al reservar memoria.");

	clt = (char *)malloc(16);
        if (clt == NULL)
                perror("Al reservar memoria.");

	fecha = (char *)malloc(16);
        if (fecha == NULL)
                perror("Al reservar memoria.");

	cad = (char *)malloc(16);
        if (cad == NULL)
                perror("Al reservar memoria.");



	/* Reserve Memoria*/

/*	sscanf(t,"%[^&]&%d&%[^&]&%[^&]&%[^&]&%d\n",numCod,&num,serv,clt,fecha,&hora);*/
	sscanf(*ticket,"%[^&]&%d",numCod,&num);
	/* Codificamos el numero aleatorio para saber la respuesta del desafio */  
	sprintf (cad, "%d",num);

	MDString (cad);

	static char* sverif;
	sverif = (char *)malloc(16);
	if (sverif == NULL)
                perror("Al reservar memoria.");

	sverif = MDCadena(CODIFICADA);


	static int v = 0;
	v = strcmp(sverif, numCod);

	/* free memory */

/*	free(numCod);*/
/*	free(serv);*/
/*	free(clt);*/
/*	free(fecha);*/
/*	free(cad);*/

	return &v;
}

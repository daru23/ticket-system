#include "env.h"
#include<stdio.h>  	// library io
#include<stdlib.h> 	// libary rand()   
#include<time.h>   	// library time()  
#include <string.h>	// library strings
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


/**
 * Take a string and codify it
 * @param string: es la cadena que codificara.
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

int main(int argc, char* argv[]) {
    	if (argc != 2) {
        	printf("Error en la Sintaxis de llamada.\n");
        	return 1;
    	}
	char *server = argv[1];

    	CLIENT *clnt = clnt_create(server, ENVPROG, ENVVERS, "tcp");
    	if (clnt == NULL) {
        	printf("Error al crear un cliente de rpc.\n");
        	clnt_pcreateerror(server);
        	return 1;
    	}

	char *cad;
	cad = (char *)malloc(16);  
	if (cad == NULL)
		perror("Al reservar memoria.");

    	char** ip_serv;
    	int *entero;
	char** tn;
	tn = (char **)malloc(10);

    	printf("Procesando peticion de IP.\n");

    	ip_serv = env_ip_1(entero,clnt);
    	if (ip_serv != NULL) {
    	    printf("%s\n",*ip_serv);
    	} else {
    	    printf("Error al ejecutar funcion remota 'ip_serv'.");
    	    clnt_perror(clnt, server);
    	}
		
    	printf("Procesando peticion de Ticket.\n");

/*	 Ask for my IP */

        struct sockaddr whereto;
        struct hostent *hp;
        struct sockaddr_in *to;
        char *target;
        char *hostname;
        struct sockaddr_in host;
        char *nombre;
        nombre = (char*)malloc(16);
        gethostname(nombre, 16);
        host.sin_addr = * (struct in_addr*) gethostbyname(nombre)->h_addr;
        static char* ip;
        ip = (char *)malloc(16);
        if (ip == NULL)
                perror("Al reservar memoria.");
        sprintf(ip,"%s",inet_ntoa(host.sin_addr));
        printf("Mi ip: %s\n",ip);

    	tn = enviar_ticket_1(&ip,clnt);

    	if (tn != NULL) {
    	    printf("Mi Ticket es: [%s]\n",*tn);
    	} else {
    	    printf("Error al ejecutar funcion remota 'ip_serv'.");
    	    clnt_perror(clnt, server);
    	}

	int num;
	int hora;
	char *serv;
	char *clt;
	char *fecha;

	serv = (char *)malloc(16);
        if (serv == NULL)
                perror("Al reservar memoria.");

	clt = (char *)malloc(16);
        if (clt == NULL)
                perror("Al reservar memoria.");

	fecha = (char *)malloc(16);
        if (fecha == NULL)
                perror("Al reservar memoria.");

	sscanf(*tn,"%d&%[^&]&%[^&]&%[^&]&%d\n",&num,serv,clt,fecha,&hora);

	printf("ip servidor %s\n",serv);
	/*We encrypt the random number for the answer the challenge*/  
	sprintf (cad, "%d",num);

	/* Print on screen */
	MDString (cad);
	printf ("MD%d (\"%s\") = ", 5, cad);
	MDPrint (CODIFICADA);
	
	printf ("\n");

	/* Verify the ticket */

	static int* verif;
	static char* tCodificado;
	tCodificado = (char *)malloc(16);
        if (tCodificado == NULL)
                perror("Al reservar memoria.");

	static char* tCompleto;
	tCompleto = (char *)malloc(300);
        if (tCompleto == NULL)
                perror("Al reservar memoria.");

	tCodificado = MDCadena(CODIFICADA);

/*	sprintf (tCompleto,"%s&%d&%s&%s&%s&%d",tCodificado,num,serv,clt,fecha,hora);*/
	sprintf (tCompleto,"%s&%d",tCodificado,num);

	printf("Mi Ticket Codificado es: [%s]\n",tCompleto);	

	verif = verificar_ticket_1(&tCompleto,clnt);

	printf("Respuesta del Verificador: %d\n",*verif);

	if (*verif == 0)
		printf("El ticket es correcto.\n");
	else
		printf("El ticket NO es correcto.\n");

	/* Free Memory */
	free(*ip_serv);
	free(nombre);
	free(ip);
	free(*tn);
 
   	return 0;
}

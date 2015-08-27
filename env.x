struct Ticket {
	int aleatorio;
	char *ip;
	char *fecha;
	int hora;
};

program ENVPROG {
	version ENVVERS {
		/* @return la IP del servidor*/
		string ENV_IP(int i) = 1;

		/* @return un ticket*/
		string ENVIAR_TICKET(string) = 2;

		/* @return un booleano*/
		int VERIFICAR_TICKET(string) = 3;

	} = 1;
} = 0x29988550;

#include "defines.h"

static void aggiorna_diagnosi(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[3];
	
	char codice[46];
	int codice_int;
	char diagnosi[257];
	
	if(!setup_prepared_stmt(&stmt, "call aggiorna_diagnosi(?,?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione aggiorna_diagnosi fallita\n", false);
	
	printf("\nCodice esame: ");
	getInput(46, codice, false);
	codice_int = atoi(codice);
	
	printf("\nInserisci diagnosi: \n");
	getInput(257, diagnosi, false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = (void *) &codice_int;
	
	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = conf.username;
	param[1].buffer_length = strlen(conf.username);
	
	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = diagnosi;
	param[2].buffer_length = strlen(diagnosi);
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding aggiorna_diagnosi fallito\n", true);
	
	if(mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Esecuzione aggiorna_diagnosi fallita\n");
	else
		printf("Diagnosi aggiornata\n");
	
	mysql_stmt_close(stmt);
}

static void inserisci_risultati(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[3], bind_risultati[4];
	
	char codice[46];
	int codice_int;
	char diagnosi[257];
	
	char nome_param[46];
	char valore[46];
	double valore_double;
	char misura[46];
	
	char options[2] = {'1', '2'};
	char op;
	bool continua = true;
	
	if(!setup_prepared_stmt(&stmt, "call esegui_esame(?,?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione esegui_esame fallita\n", false);
	
	printf("\nCodice esame: ");
	getInput(46, codice, false);
	codice_int = atoi(codice);
	
	printf("\nInserisci diagnosi: \n");
	getInput(257, diagnosi, false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = (void *) &codice_int;
	
	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = conf.username;
	param[1].buffer_length = strlen(conf.username);
	
	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = diagnosi;
	param[2].buffer_length = strlen(diagnosi);
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding esegui_esame fallito\n", true);
	
	if(mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "Esecuzione esegui_esame fallita\n");
		mysql_stmt_close(stmt);
		return;
	}
	
	mysql_stmt_close(stmt);
	
	// insert exam results
	printf("Inserisci parametri:\n");
	
	if(!setup_prepared_stmt(&stmt, "call inserisci_risultati(?,?,?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione inserisci_risultati fallita\n", false);
	
	memset(bind_risultati, 0, sizeof(bind_risultati));
	
	bind_risultati[0].buffer_type = MYSQL_TYPE_LONG;
	bind_risultati[0].buffer = (void *) &codice_int;
	
	bind_risultati[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	bind_risultati[1].buffer = nome_param;
	bind_risultati[1].buffer_length = strlen(nome_param);
	
	bind_risultati[2].buffer_type = MYSQL_TYPE_DOUBLE;
	bind_risultati[2].buffer = &valore_double;
	bind_risultati[2].buffer_length = sizeof(valore_double);
	
	bind_risultati[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	bind_risultati[3].buffer = misura;
	
	while(continua) {
		// chiedi input
		printf("\n\tNome parametro: ");
		getInput(46, nome_param, false);
		
		printf("\tValore parametro: ");
		getInput(46, valore, false);
		valore_double = atof(valore);
		
		printf("\tUnità di misura parametro: ");
		getInput(46, misura, false);
		
		// bind, execute inserisci_risultato
		bind_risultati[1].buffer_length = strlen(nome_param);
		bind_risultati[3].buffer_length = strlen(misura);
		
		if(mysql_stmt_bind_param(stmt, bind_risultati) != 0)
			finish_with_stmt_error(conn, stmt, "Binding inserisci_risultati fallito\n", true);
	
		if(mysql_stmt_execute(stmt) != 0)
			finish_with_stmt_error(conn, stmt, "Esecuzione inserisci_risultati fallita\n", true);
		
		printf("\nInserire altri parametri?\t1) SI\t2)NO\n");
		op = multiChoice("Scegli un'opzione", options, 2);
		switch(op) {
			case '1':
				continua = true;
				break;
			case '2':
				continua = false;
				break;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}
	}
	
	// close statement
	mysql_stmt_close(stmt);
}

void run_as_personale(MYSQL *conn) {
	char options[3] = {'1', '2', '3'};
	char op;
	
	printf("Switching to PERSONNEL role...\n\n");
	
	if(!parse_config("users/personale.json", &conf)) {
		fprintf(stderr, "Unable to load PERSONNEL configuration\n");
		exit(EXIT_FAILURE);
	}
	
	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		print_error(conn, "mysql_change_user() failed");
		exit(EXIT_FAILURE);
	}
	
	while(true) {
		printf("\033[2J\033[H");
		printf("PERSONALE\n");
		printf("*** COSA VUOI FARE ***\n\n");
		printf("1) Inserisci risultati esame\n");
		printf("2) Aggiorna diagnosi esame\n");
		printf("3) Esci\n");
		
		op = multiChoice("Scegli un'opzione", options, 3);
		
		switch(op) {
			case '1':
				inserisci_risultati(conn);
				break;
			case '2':
				aggiorna_diagnosi(conn);
				break;
			case '3':
				return;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}
		
		printf("\npremi il tasto INVIO per continuare\n");
		getchar();
	}
}

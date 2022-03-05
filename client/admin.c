#include "defines.h"

static void inserisci_tipo_esame(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[1];
	
	// input for the routine
	char nome[46];
	
	if(!setup_prepared_stmt(&stmt, "call inserisci_tipo_esame(?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione inserisci tipo esame fallita\n", false);
	
	// clean memory for param
	memset( (void *) param, 0, sizeof(param));
	
	// filling parameters
	printf("Nome del nuovo tipo di esame: ");
	getInput(46, nome, false);
	
	// set up parameters
	param[0].buffer_type = MYSQL_TYPE_STRING;
	param[0].buffer = (void *) nome;
	param[0].buffer_length = strlen(nome);
	
	// binding parameters
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding inserisci tipo esame fallito\n", true);
	
	// execute stmt
	if(mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute inserisci tipo esame fallito\n");
	else
		printf("\nNuovo tipo esame (%s) inserito correttamente.\n", nome);
	
	// deallocate stmt handler
	mysql_stmt_close(stmt);
}

static void cambia_stato_tipo_esame(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[2];
	
	// input for the routine
	char nome[46];
	my_bool disponibile;
	
	char options[2] = {'1', '2'};
	char r;
	
	if(!setup_prepared_stmt(&stmt, "call stato_tipo_esame(?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione cambia stato tipo esame fallita\n", false);
	
	// clean memory for param
	memset( (void *) param, 0, sizeof(param));
	
	// filling parameters
	printf("Nome del tipo di esame: ");
	getInput(46, nome, false);
	
	printf("Stato del tipo di esame:\n");
	printf("\t1) Disponibile\n");
	printf("\t2) NON disponibile\n");
	r = multiChoice("Seleziona stato", options, 2);
	
	// set true or false
	switch(r) {
		case '1':
			disponibile = true;
			break;
		case '2':
			disponibile = false;
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}
	
	// set up parameters
	param[0].buffer_type = MYSQL_TYPE_STRING;
	param[0].buffer = (void *) nome;
	param[0].buffer_length = strlen(nome);
	
	param[1].buffer_type = MYSQL_TYPE_TINY;
	param[1].buffer = (char *) &disponibile;
	param[1].buffer_length = sizeof(disponibile);
	
	// binding parameters
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding cambia stato tipo esame fallito\n", true);
	
	// execute stmt
	if(mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute cambia stato tipo esame fallita\n");
	else
		printf("\nStato del tipo di esame %s correttamente cambiato.\n", nome);
	
	// deallocate stmt handler
	mysql_stmt_close(stmt);
}

static void report_personale(MYSQL *conn, int opzione) {
	if(opzione != 0 && opzione != 1) {
		printf("Errore: report_personale(MYSQL *conn, int opzione)\t opzione deve essere 0 o 1\n");
		return;
	}
	
	MYSQL_STMT *stmt;
	MYSQL_BIND param[2];
	
	char cf[17];
	
	int num_exams;
	
	printf("\nInserisci codice fiscale: ");
	getInput(17, cf, false);
	
	if(!setup_prepared_stmt(&stmt, "call report_esami_eseguiti(?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione report_personale fallita\n", false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = cf;
	param[0].buffer_length = strlen(cf);
	
	param[1].buffer_type = MYSQL_TYPE_LONG;
	param[1].buffer = &opzione;
	param[1].buffer_length = sizeof(opzione);
	
	if (mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding report_personale fallito\n", true);

	if (mysql_stmt_execute(stmt) != 0)
		finish_with_stmt_error(conn, stmt, "Execute report_paziente fallita\n", true);
	
	num_exams = dump_result_set(conn, stmt, "\nEsami eseguiti:"); // my DE does not recognize MYSQL_BIND.is_null_value
	
	if(mysql_stmt_next_result(stmt) > 0)
		print_stmt_error(stmt, "Errore nello scorrere i result sets di report_personale()\n");
	
	if(opzione == 0)
		printf("\nEsami eseguiti nell'ultimo mese: %d\n", num_exams);
	if(opzione == 1)
		printf("\nEsami eseguiti nell'ultimo anno: %d\n", num_exams);
	
	mysql_stmt_close(stmt);
}

static void gestione_esami(MYSQL *conn) {
	char options[3] = {'1', '2', '3'};
	char op;
	
	while(true) {
		printf("\033[2J\033[H");
		printf("ADMIN - Gestione Esami\n");
		printf("*** COSA VUOI FARE ***\n\n");
		printf("1) Aggiungi tipo esame\n");
		printf("2) Cambia stato tipo esame\n");
		printf("3) Indietro\n");
		
		op = multiChoice("Scegli un'opzione", options, 3);
		
		switch(op) {
			case '1':
				inserisci_tipo_esame(conn);
				break;
			case '2':
				cambia_stato_tipo_esame(conn);
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

void run_as_admin(MYSQL *conn) {
	char options[6] = {'1', '2', '3', '4', '5', '6'};
	char op;
	
	printf("Switching to ADMIN role...\n\n");
	
	if(!parse_config("users/amministratore.json", &conf)) {
		fprintf(stderr, "Unable to load ADMIN configuration\n");
		exit(EXIT_FAILURE);
	}
	
	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		print_error(conn, "mysql_change_user() failed");
		exit(EXIT_FAILURE);
	}
	
	while(true) {
		printf("\033[2J\033[H");
		printf("ADMIN\n");
		printf("*** COSA VUOI FARE ***\n\n");
		printf("1) Gestione Personale\n");
		printf("2) Gestione Ospedali\n");
		printf("3) Gestione Esami\n");
		printf("4) Report mensile\n");
		printf("5) Report annuale\n");
		printf("6) Esci\n");
		op = multiChoice("Scegli un'opzione", options, 6);
		switch(op) {
			case '1':
				gestione_personale(conn);
				break;
			case '2':
				gestione_ospedali(conn);
				break;
			case '3':
				gestione_esami(conn);
				break;
			case '4':
				report_personale(conn, 0);
				break;
			case '5':
				report_personale(conn, 1);
				break;
			case '6':
				return;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}
		
		printf("\npremi il tasto INVIO per continuare\n");
		getchar();
	}
}

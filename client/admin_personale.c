#include "defines.h"

static void anagrafica_personale(MYSQL *conn, char *cf) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[1];
	
	int status;
	
	if(!setup_prepared_stmt(&stmt, "call anagrafica_personale(?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione anagrafica personale fallita\n", false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = cf;
	param[0].buffer_length = strlen(cf);
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding anagrafica personale fallita\n", true);
		
	if(mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "Execute anagrafica personale fallita\n");
		goto out;
	}
	
	dump_result_set(conn, stmt, "\nAnagrafica personale");
	
	// print the degrees of the ward responsible or the association of the volunteer
	do {
		status = mysql_stmt_next_result(stmt);
		dump_result_set(conn, stmt, "");
		
		if(status > 0)
			finish_with_stmt_error(conn, stmt, "Errore nello scorrere i result sets di anagrafica_personale()\n", true);
	} while (status == 0);
	
 out:
	mysql_stmt_close(stmt);
}

static void crea_utente(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[3];
	
	char options[5] = {'1', '2', '3'};
	char r;
	
	// input for the routine
	char username[46];
	char password[46];
	char ruolo[46];
	
	// Get the required information
	printf("\nUsername: ");
	getInput(46, username, false);
	printf("password: ");
	getInput(46, password, true);
	printf("Assegna un ruolo possibile:\n");
	printf("\t1) Amministratore\n");
	printf("\t2) CUP\n");
	printf("\t3) Personale\n");
	r = multiChoice("Seleziona ruolo", options, 3);
	
	// Convert role into enum value
	switch(r) {
		case '1':
			strcpy(ruolo, "amministratore");
			break;
		case '2':
			strcpy(ruolo, "cup");
			break;
		case '3':
			strcpy(ruolo, "personale");
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&stmt, "call crea_utente(?, ?, ?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione crea utente fallita\n", false);

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = username;
	param[0].buffer_length = strlen(username);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = password;
	param[1].buffer_length = strlen(password);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = ruolo;
	param[2].buffer_length = strlen(ruolo);

	if (mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding crea utente fallito\n", true);

	// Run procedure
	if (mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute crea utente fallita\n");
	else
		printf("Utente aggiunto correttamente...\n");

	mysql_stmt_close(stmt);
}

static void modifica_associazione_volontariato(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[2];
	
	char cf[17];
	char associazione[46];
	
	printf("\nCodice Fiscale del volontario: ");
	getInput(17, cf, false);
	printf("Nome associazione: ");
	getInput(46, associazione, false);
	
	if(!setup_prepared_stmt(&stmt, "call aggiorna_associazione_vol(?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione modifica associazione volontario fallita\n", false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = cf;
	param[0].buffer_length = strlen(cf);
	
	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = associazione;
	param[1].buffer_length = strlen(associazione);
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding modifica associazione volontario fallita\n", true);
		
	if(mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute modifica associazione volontario fallita\n");
	else
		printf("Modifica avvenuta correttamente.\n");
	
	mysql_stmt_close(stmt);
}

static void inserisci_personale(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[7]; // cf, nome, cognome, domicilio, tipo, reparto, username
	
	char options[2] = {'1', '2'};
	char r;
	
	// input for the routine
	char cf[17];
	char nome[46];
	char cognome[46];
	char domicilio[46];
	char tipo[46];
	char reparto[46];
	int reparto_int;
	char username[46];
	
	// Get the required information
	printf("\nCodice Fiscale: ");
	getInput(17, cf, false);
	printf("Nome: ");
	getInput(46, nome, false);
	printf("Cognome: ");
	getInput(46, cognome, false);
	printf("Domicilio: ");
	getInput(46, domicilio, false);
	printf("Username: ");
	getInput(46, username, false);
	printf("ID Reparto: ");
	getInput(46, reparto, false);
	reparto_int = atoi(reparto);
	printf("Assegna un tipo possibile:\n"); // to update to 'primario' you have to use 'aggiungi/modifica reparto'
	printf("\t1) Medico\n");
	printf("\t2) Volontario\n");
	r = multiChoice("Seleziona ruolo", options, 2);
	
	// Convert role into enum value
	switch(r) {
		case '1':
			strcpy(tipo, "medico");
			break;
		case '2':
			strcpy(tipo, "volontario");
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&stmt, "call inserisci_personale(?,?,?,?,?,?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione inserisci personale fallita\n", false);

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = cf;
	param[0].buffer_length = strlen(cf);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = nome;
	param[1].buffer_length = strlen(nome);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = cognome;
	param[2].buffer_length = strlen(cognome);
	
	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = domicilio;
	param[3].buffer_length = strlen(domicilio);
	
	param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[4].buffer = tipo;
	param[4].buffer_length = strlen(tipo);
	
	param[5].buffer_type = MYSQL_TYPE_LONG;
	param[5].buffer = (void *) &reparto_int;
	
	param[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[6].buffer = username;
	param[6].buffer_length = strlen(username);

	if (mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding inserisci personale fallito\n", true);

	// Run procedure
	if (mysql_stmt_execute(stmt) != 0)
		print_stmt_error (stmt, "Execute inserisci personale fallita\n");
	else
		printf("Utente aggiunto correttamente...\n");

	mysql_stmt_close(stmt);
}

static void modifica_personale(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[4]; // cf, colonna, var_str, var_int
	
	char cf[17];
	int colonna;
	char var_str[46];
	char reparto[46];
	int  var_int;
	my_bool var_str_is_null = true;
	my_bool var_int_is_null = true;
	
	// personale(cf varchar16, nome varchar45, cognome varchar45, domicilio varchar45, tipo enum, reparto int, username varchar45)
	// var_str is used to send the new value of one of this attributes: cf, nome, cognome, domicilio, tipo, username
	// var_int is used to send the new value of one of this attributes: reparto
	
	// ask which personnel member to update
	printf("\nInserisci codice fiscale: ");
	getInput(17, cf, false);
	
	// print her/his info
	anagrafica_personale(conn, cf);
	
	// select which attribute to update
	char options[7] = {'1', '2', '3', '4', '5', '6', '7'};
	char op;
	printf("\nQuale campo modificare?\n");
	printf("1) Codice Fiscale\n");
	printf("2) Nome\n");
	printf("3) Cognome\n");
	printf("4) Domicilio\n");
	printf("5) Tipo\n");
	printf("6) Reparto\n");
	printf("7) Username\n");
	op = multiChoice("Scegli un'opzione", options, 7);
	
	colonna = op - '0';
	
	switch(op) {
		case '1':
			var_str_is_null = false;
			printf("Nuovo Codice Fiscale del membro del personale: ");
			getInput(17, var_str, false);
			break;
		case '2':
			var_str_is_null = false;
			printf("\nInserisci nuovo nome: ");
			getInput(46, var_str, false);
			break;
		case '3':
			var_str_is_null = false;
			printf("\nInserisci nuovo cognome: ");
			getInput(46, var_str, false);
			break;
		case '4':
			var_str_is_null = false;
			printf("\nInserisci nuovo domicilio: ");
			getInput(46, var_str, false);
			break;
		case '5':
			var_str_is_null = false;
			char type[2] = {'1', '2'};
			char t;
			printf("\nScegli nuovo tipo\n");
			printf("1) Medico\t");
			printf("2) Volontario\n");
			t = multiChoice("Scegli un'opzione", type, 2);
			switch(t) {
				case '1':
					strcpy(var_str, "medico");
					break;
				case '2':
					strcpy(var_str, "volontario");
					break;
				default:
					fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
					abort();
			}
			break;
		case '6':
			var_int_is_null = false;
			printf("\nInserisci ID del nuovo reparto: ");
			getInput(46, reparto, false);
			var_int = atoi(reparto);
			break;
		case '7':
			var_str_is_null = false;
			printf("\nInserisci nuovo username: ");
			getInput(46, var_str, false);
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}
	
	// prepare, bind, execute, close
	if(!setup_prepared_stmt(&stmt, "call modifica_personale(?,?,?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione statement modifica personale fallita\n", false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_STRING;
	param[0].buffer = cf;
	param[0].buffer_length = strlen(cf);
	
	param[1].buffer_type = MYSQL_TYPE_LONG;
	param[1].buffer = &colonna;
	param[1].buffer_length = sizeof(colonna);
	
	param[2].buffer_type = MYSQL_TYPE_STRING;
	param[2].buffer = var_str;
	param[2].buffer_length = strlen(var_str);
	param[2].is_null = &var_str_is_null;
	
	param[3].buffer_type = MYSQL_TYPE_LONG;
	param[3].buffer = &var_int;
	param[3].buffer_length = sizeof(var_int);
	param[3].is_null = &var_int_is_null;
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding statement modifica personale fallita\n", true);
	
	if(mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute statement modifica personale fallita\n");
	else
		printf("\nModifica avvenuta correttamente.");
	
	mysql_stmt_close(stmt);
}

static void cancella_personale(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[1];
	
	char cf[17];
	printf("\nInserisci Codice Fiscale del membro del personale da cancellare: ");
	getInput(17, cf, false);
	
	if(!setup_prepared_stmt(&stmt, "call cancella_personale(?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione cancella_personale fallita\n", false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = cf;
	param[0].buffer_length = strlen(cf);
	
	if (mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding cancella_personale fallito\n", true);

	if (mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute cancella_paziente fallita\n");
	else
		printf("Cancellazione avvenuta correttamente...\n");

	mysql_stmt_close(stmt);
}

static void aggiungi_specializzazione(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[2];
	
	char options[2] = {'1', '2'};
	char r;
	bool continua = true;
	
	// input for the routine
	char primario[17];
	char specializzazione[46];
	
	// Prepare stored procedure call
	if(!setup_prepared_stmt(&stmt, "call aggiungi_specializzazione(?, ?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione aggiungi specializzazione fallita\n", false);
	
	printf("\nCodice Fiscale del primario: ");
	getInput(17, primario, false);
	
	while(continua) {
		printf("Specializzazione: ");
		getInput(46, specializzazione, false);

		// Prepare parameters
		memset(param, 0, sizeof(param));

		param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
		param[0].buffer = primario;
		param[0].buffer_length = strlen(primario);

		param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
		param[1].buffer = specializzazione;
		param[1].buffer_length = strlen(specializzazione);

		if (mysql_stmt_bind_param(stmt, param) != 0) {
			finish_with_stmt_error(conn, stmt, "Binding aggiungi specializzazione fallito\n", true);
		}

		// Run procedure
		if (mysql_stmt_execute(stmt) != 0) {
			print_stmt_error (stmt, "Esecuzione aggiungi specializzazione fallita\n");
			goto end;
		} else {
			printf("Specializzazione aggiunta correttamente\n");
		}
		
		// chiedi se continuare
		printf("Aggiungere un'altra specializzazione a questo primario?\n");
		printf("\t1) Sì\t2) No\n");
		r = multiChoice("Seleziona opzione", options, 2);
		switch(r) {
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
end:
	mysql_stmt_close(stmt);
}

void gestione_personale(MYSQL *conn) {
	char options[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
	char op;
	
	char cf[17];
	
	while(true) {
		printf("\033[2J\033[H");
		printf("ADMIN - Gestione Personale\n");
		printf("*** COSA VUOI FARE ***\n\n");
		printf("1) Aggiungi utente\n");
		printf("2) Aggiungi personale\n");
		printf("3) Modifica personale\n");
		printf("4) Elimina personale\n");
		printf("5) Anagrafica personale\n");
		printf("6) Aggiungi specializzazione di un primario\n");
		printf("7) Modifica associazione di un volontario\n");
		printf("8) Indietro\n");
		
		op = multiChoice("Scegli un'opzione", options, 8);
		
		switch(op) {
			case '1':
				crea_utente(conn);
				break;
			case '2':
				inserisci_personale(conn);
				break;
			case '3':
				modifica_personale(conn);
				break;
			case '4':
				cancella_personale(conn);
				break;
			case '5':
				printf("\nInserisci codice fiscale: ");
				getInput(17, cf, false);
				anagrafica_personale(conn, cf);
				break;
			case '6':
				aggiungi_specializzazione(conn);
				break;
			case '7':
				modifica_associazione_volontariato(conn);
				break;
			case '8':
				return;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}
		printf("\npremi il tasto INVIO per continuare\n");
		getchar();
	}
}

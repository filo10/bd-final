#include "defines.h"

static void modifica_info_ospedale(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[3]; // nome, indirizzo, responsabile
	
	char codice[46];
	int  codice_int;
	int colonna;
	char var_str[46]; // used to send the new value of the attribute to update
	
	// print hospitals
	stampa_lista_ospedali(conn);
	
	// select hospital to update
	printf("\nInserisci codice ospedale: ");
	getInput(46, codice, false);
	codice_int = atoi(codice);
	
	// select which attribute to update
	char options[3] = {'1', '2', '3'};
	char op;
	printf("\nQuale campo modificare?\n");
	printf("1) Nome Ospedale\n");
	printf("2) Indirizzo\n");
	printf("3) Responsabile Ospedale\n");
	op = multiChoice("Scegli un'opzione", options, 3);
	
	colonna = op - '0';
	
	switch(op) {
		case '1':
			printf("\nInserisci nuovo nome ospedale: ");
			getInput(46, var_str, false);
			break;
		case '2':
			printf("\nInserisci nuovo indirizzo ospedale: ");
			getInput(46, var_str, false);
			break;
		case '3':
			printf("\nRicordati che il nuovo responsabile deve essere un membro del personale di tipo medico o primario, assegnato ad un reparto dell'ospedale che stai modificando");
			printf("\nInserisci Codice Fiscale nuovo responsabile: ");
			getInput(17, var_str, false);
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}
	
	// prepare, bind, execute, close
	if(!setup_prepared_stmt(&stmt, "call modifica_ospedale(?,?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione statement modifica ospedale fallita\n", false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &codice_int;
	param[0].buffer_length = sizeof(codice_int);
	
	param[1].buffer_type = MYSQL_TYPE_LONG;
	param[1].buffer = &colonna;
	param[1].buffer_length = sizeof(colonna);
	
	param[2].buffer_type = MYSQL_TYPE_STRING;
	param[2].buffer = var_str;
	param[2].buffer_length = strlen(var_str);
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding statement modifica ospedale fallita\n", true);
	
	if(mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute statement modifica ospedale fallita\n");
	else
		printf("\nModifica avvenuta correttamente.");
	
	mysql_stmt_close(stmt);
}

static void aggiungi_reparto(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[5]; // osp., nome, tel., new primario, specializ.
	
	char ospedale[46];
	int  ospedale_int;
	char nome[46];
	char telefono[46];
	char primario[17];
	char specializzazione[46];
	
	// get input
	printf("\nInserisci codice ospedale: ");
	getInput(46, ospedale, false);
	ospedale_int = atoi(ospedale);
	printf("\nInserisci nome nuovo reparo: ");
	getInput(46, nome, false);
	printf("\nInserisci telefono nuovo reparto: ");
	getInput(46, telefono, false);
	printf("\nAttenzione: selezionare un medico dallo stesso ospedale che non sia già primario.");
	printf("\nInserisci codice fiscale del nuovo primario: ");
	getInput(17, primario, false);
	printf("\nInserisci specializzazione del nuovo primario: ");
	getInput(46, specializzazione, false);
	
	// prepare, bind, execute, close
	if(!setup_prepared_stmt(&stmt, "call inserisci_reparto(?,?,?,?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione statement inserisci reparto fallita\n", false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &ospedale_int;
	param[0].buffer_length = sizeof(ospedale_int);
	
	param[1].buffer_type = MYSQL_TYPE_STRING;
	param[1].buffer = nome;
	param[1].buffer_length = strlen(nome);
	
	param[2].buffer_type = MYSQL_TYPE_STRING;
	param[2].buffer = telefono;
	param[2].buffer_length = strlen(telefono);
	
	param[3].buffer_type = MYSQL_TYPE_STRING;
	param[3].buffer = primario;
	param[3].buffer_length = strlen(primario);
	
	param[4].buffer_type = MYSQL_TYPE_STRING;
	param[4].buffer = specializzazione;
	param[4].buffer_length = strlen(specializzazione);
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding statement inserisci reparto fallita\n", true);
	
	if(mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute statement inserisci reparto fallita\n");
	else
		printf("Reparto aggiunto correttamente\n");
	
	mysql_stmt_close(stmt);
}

static void modifica_reparto(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[4]; // id_rep, colonna, var_str, var_specializ
	
	char reparto[46];
	int  reparto_int;
	int colonna;
	char var_str[46]; // used to send the new value of the attribute to update
	char var_specializzazione[46];
	my_bool var_specializzazione_is_null = true;
	
	// ask which ward to update
	printf("\nInserisci ID reparto: ");
	getInput(46, reparto, false);
	reparto_int = atoi(reparto);
	
	// select the attribute to update
	char options[3] = {'1', '2', '3'};
	char op;
	printf("\nQuale campo modificare?\n");
	printf("1) Nome reparto\n");
	printf("2) Telefono\n");
	printf("3) Primario reparto\n");
	op = multiChoice("Scegli un'opzione", options, 3);
	
	colonna = op - '0';
	
	switch(op) {
		case '1':
			printf("\nInserisci nuovo nome reparto: ");
			getInput(46, var_str, false);
			break;
		case '2':
			printf("\nInserisci nuovo telefono reparto: ");
			getInput(46, var_str, false);
			break;
		case '3':
			printf("\nRicordati che il nuovo primario deve essere un membro del personale di tipo medico, assegnato ad un reparto dell'ospedale che stai modificando");
			printf("\nInserisci Codice Fiscale nuovo primario: ");
			getInput(17, var_str, false);
			var_specializzazione_is_null = false;
			printf("Inserisci specializzazione nuovo primario reparto: ");
			getInput(46, var_specializzazione, false);
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}
	
	// prepare, bind, execute, close
	if(!setup_prepared_stmt(&stmt, "call modifica_reparto(?,?,?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione statement modifica reparto fallita\n", false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &reparto_int;
	param[0].buffer_length = sizeof(reparto_int);
	
	param[1].buffer_type = MYSQL_TYPE_LONG;
	param[1].buffer = &colonna;
	param[1].buffer_length = sizeof(colonna);
	
	param[2].buffer_type = MYSQL_TYPE_STRING;
	param[2].buffer = var_str;
	param[2].buffer_length = strlen(var_str);
	
	param[3].buffer_type = MYSQL_TYPE_STRING;
	param[3].buffer = var_specializzazione;
	param[3].buffer_length = strlen(var_specializzazione);
	param[3].is_null = &var_specializzazione_is_null;
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding statement modifica reparto fallita\n", true);
	
	if(mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute statement modifica reparto fallita\n");
	else
		printf("\nModifica avvenuta correttamente.");
	
	mysql_stmt_close(stmt);
}

static void elimina_reparto(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[2];
	
	char reparto[46];
	int reparto_int;
	
	int new_reparto;
	
	// scan input
	printf("\nInserisci id reparto: ");
	getInput(46, reparto, false);
	reparto_int = atoi(reparto);
	
	if(!setup_prepared_stmt(&stmt, "call cancella_reparto(?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizialiazzazione cancella reparto fallita\n", false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &reparto_int;
	param[0].buffer_length = sizeof(reparto_int);
	
	param[1].buffer_type = MYSQL_TYPE_LONG;
	param[1].buffer = &new_reparto;
	param[1].buffer_length = sizeof(new_reparto);
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding cancella reparto fallito\n", true);
	
	if(mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "Execute cancella reparto fallita\n");
		goto out;
	}
	
	// get the new ward id where every personnel member from the deleted ward is moved
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &new_reparto;
	param[0].buffer_length = sizeof(new_reparto);
	
	if(mysql_stmt_bind_result(stmt, param))
		finish_with_stmt_error(conn, stmt, "Binding risultato cancella reparto fallito", true);
	
	if(mysql_stmt_store_result(stmt))
		finish_with_stmt_error(conn, stmt, "Store result cancella reparto fallita", true);
	
	while(!mysql_stmt_fetch(stmt)) // 1x1 result set = new_reparto
	
	if(mysql_stmt_next_result(stmt) > 0)
		finish_with_stmt_error(conn, stmt, "Errore nello scorrere i risultati in cancella reparto\n", true);
	
	printf("Reparto eliminato correttamente\nTutti i membri del personale sono stati spostati nel reparto con ID = %d\n", new_reparto);
	
 out:
	mysql_stmt_close(stmt);
}

bool aggiungi_laboratorio(MYSQL *conn, bool from_args, char *resp, int osp_code) {
	bool success = false;
	
	MYSQL_STMT *stmt;
	MYSQL_BIND param[5]; // osp., nome, piano, stanza, new responsabile
	
	char ospedale[46];
	int  ospedale_int;
	char nome[46];
	char piano[46];
	int piano_int;
	char stanza[46];
	int stanza_int;
	char responsabile[17];
	
	if(from_args) {
		ospedale_int = osp_code;
		strcpy(responsabile, resp);
	}
	
	// get input
	if(!from_args) {
		printf("\nInserisci codice ospedale: ");
		getInput(46, ospedale, false);
		ospedale_int = atoi(ospedale);
	}
	printf("\nInserisci nome nuovo laboratorio: ");
	getInput(46, nome, false);
	printf("Inserisci numero piano nuovo laboratorio: ");
	getInput(46, piano, false);
	piano_int = atoi(piano);
	printf("Inserisci numero stanza nuovo laboratorio: ");
	getInput(46, stanza, false);
	stanza_int = atoi(stanza);
	if(!from_args) {
		printf("\nAttenzione: selezionare un medico dallo stesso ospedale che non sia già responsabile di un laboratorio.");
		printf("\nInserisci codice fiscale del nuovo responsabile: ");
		getInput(17, responsabile, false);
	}
	
	// prepare, bind, execute, close
	if(!setup_prepared_stmt(&stmt, "call inserisci_laboratorio(?,?,?,?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione statement inserisci laboratorio fallita\n", false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &ospedale_int;
	param[0].buffer_length = sizeof(ospedale_int);
	
	param[1].buffer_type = MYSQL_TYPE_STRING;
	param[1].buffer = nome;
	param[1].buffer_length = strlen(nome);
	
	param[2].buffer_type = MYSQL_TYPE_LONG;
	param[2].buffer = &piano_int;
	param[2].buffer_length = sizeof(piano_int);
	
	param[3].buffer_type = MYSQL_TYPE_LONG;
	param[3].buffer = &stanza_int;
	param[3].buffer_length = sizeof(stanza_int);
	
	param[4].buffer_type = MYSQL_TYPE_STRING;
	param[4].buffer = responsabile;
	param[4].buffer_length = strlen(responsabile);
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding statement inserisci laboratorio fallita\n", true);
	
	if(mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute statement inserisci laboratorio fallita\n");
	else {
		printf("Nuovo laboratorio inserito correttamente.\n");
		success = true;
	}
	
	mysql_stmt_close(stmt);
	
	return success;
}

static void modifica_laboratorio(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[4]; // lab ID, colonna, var_str, var_int
	
	char laboratorio[46];
	int lab_int;
	int colonna;
	char var_str[46];
	char var[46];
	int  var_int;
	my_bool var_str_is_null = true;
	my_bool var_int_is_null = true;
	
	// laboratorio(id, codice, ospedale, nome, piano, stanza, respons)
	// var_str is used to send the new value of one of this attributes:	nome, responsabile
	// var_int is used to send the new value of one of this attributes: piano, stanza
	
	// get ID laboratorio
	printf("\nInserisci Id laboratorio: ");
	getInput(46, laboratorio, false);
	lab_int = atoi(laboratorio);
	
	// select which attribute to update
	char options[4] = {'1', '2', '3', '4'};
	char op;
	printf("\nQuale campo modificare?\n");
	printf("1) Nome laboratorio\n");
	printf("2) Piano\n");
	printf("3) Stanza\n");
	printf("4) Responsabile\n");
	op = multiChoice("Scegli un'opzione", options, 4);
	
	colonna = op - '0';
	
	switch(op) {
		case '1':
			var_str_is_null = false;
			printf("Nuovo nome del laboratorio: ");
			getInput(46, var_str, false);
			break;
		case '2':
			var_int_is_null = false;
			printf("\nInserisci nuovo numero di piano del laboratorio: ");
			getInput(46, var, false);
			var_int = atoi(var);
			break;
		case '3':
			var_int_is_null = false;
			printf("\nInserisci nuovo numero di stanza del laboratorio: ");
			getInput(46, var, false);
			var_int = atoi(var);
			break;
		case '4':
			var_str_is_null = false;
			printf("\nAttenzione: selezionare un medico dallo stesso ospedale che non sia già responsabile di un laboratorio.");
			printf("\nInserisci codice fiscale del nuovo responsabile del laboratorio: ");
			getInput(17, var_str, false);
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}
	
	// prepare, bind, execute, close
	if(!setup_prepared_stmt(&stmt, "call modifica_laboratorio(?,?,?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione statement modifica laboratorio fallita\n", false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &lab_int;
	param[0].buffer_length = sizeof(lab_int);
	
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
		finish_with_stmt_error(conn, stmt, "Binding statement modifica laboratorio fallita\n", true);
	
	if(mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute statement modifica laboratorio fallita\n");
	else
		printf("\nModifica avvenuta correttamente.");
	
	mysql_stmt_close(stmt);
}

static void elimina_laboratorio(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[1];
	
	char laboratorio[46];
	int laboratorio_int;
	
	printf("\nInserisci id laboratorio: ");
	getInput(46, laboratorio, false);
	laboratorio_int = atoi(laboratorio);
	
	if(!setup_prepared_stmt(&stmt, "call cancella_laboratorio(?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizialiazzazione cancella laboratorio fallita\n", false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &laboratorio_int;
	param[0].buffer_length = sizeof(laboratorio_int);
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding cancella laboratorio fallito\n", true);
	
	if(mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute cancella laboratorio fallita\n");
	else
		printf("Laboratorio eliminato correttamente.\n");
	
	mysql_stmt_close(stmt);
}

void gestione_interno_ospedale(MYSQL *conn) {
	char options[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
	char op;
	
	while(true) {
		printf("\033[2J\033[H");
		printf("ADMIN - Gestione Ospedali - Modifica Ospedale\n");
		printf("*** COSA VUOI FARE ***\n\n");
		printf("1) Modifica info ospedale\n");
		printf("2) Aggiungi reparto\n");
		printf("3) Modifica reparto\n");
		printf("4) Elimina reparto\n");
		printf("5) Aggiungi laboratorio\n");
		printf("6) Modifica laboratorio\n");
		printf("7) Elimina laboratorio\n");
		printf("8) Indietro\n");
		
		op = multiChoice("Scegli un'opzione", options, 8);
		
		switch(op) {
			case '1':
				modifica_info_ospedale(conn);
				break;
			case '2':
				aggiungi_reparto(conn);
				break;
			case '3':
				modifica_reparto(conn);
				break;
			case '4':
				elimina_reparto(conn);
				break;
			case '5':
				aggiungi_laboratorio(conn, false, NULL, 0);
				break;
			case '6':
				modifica_laboratorio(conn);
				break;
			case '7':
				elimina_laboratorio(conn);
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

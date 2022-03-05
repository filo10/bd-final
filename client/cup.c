#include "defines.h"

static void stampa_recapiti(MYSQL *conn, int num_ts) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[1];
	
	// initialize statement
	if(!setup_prepared_stmt(&stmt, "call lista_recapiti(?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione lista recapiti paziente fallita", false);
	
	// set-up parameters
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = (void *) &num_ts;
	
	// bind param
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding lista recapiti paziente fallita\n", true);
	
	// run procedure
	if(mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "Execute lista recapiti paziente fallita\n");
		goto out;
	}

	dump_result_set(conn, stmt, "\nRecapiti del paziente");	// print result set
	
	if(mysql_stmt_next_result(stmt) > 0)
		finish_with_stmt_error(conn, stmt, "Errore nello scorrere i result sets di stampa_recapiti()\n", true);
	
 out:
	mysql_stmt_close(stmt);
}

static int stampa_anagrafica(MYSQL *conn, bool print_contacts) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[1];
	
	int num_ts_int;
	char num_ts[46];
	
	// print patient info
	if(!setup_prepared_stmt(&stmt, "call anagrafica_paziente(?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione anagrafica paziente fallita", false);
	
	printf("\nNumero tessera sanitaria: ");
	getInput(46, num_ts, false);
	num_ts_int = atoi(num_ts);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = (void *) &num_ts_int;
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding anagrafica paziente fallita\n", true);
	
	if(mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "Execute anagrafica paziente fallita\n");
		mysql_stmt_close(stmt);
		return -1;
	}

	dump_result_set(conn, stmt, "\nAnagrafica paziente");
	
	if(mysql_stmt_next_result(stmt) > 0)
		finish_with_stmt_error(conn, stmt, "Errore nello scorrere i result sets di stampa_anagrafica()\n", true);
	
	mysql_stmt_close(stmt);
	
	if(print_contacts) // print patient's contacts?
		stampa_recapiti(conn, num_ts_int);
	
	return num_ts_int;
}

static void inserisci_paziente(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[7];
	
	// input for the routine
	char num_ts[46];
	int num_ts_int;
	char nome[46];
	char cognome[46];
	char luogo_nascita[46];
	char residenza[46];
	char recapito[46];
	
	MYSQL_TIME data_nascita;
	char anno[5];
	int anno_int;
	char mese[3];
	int mese_int;
	char giorno[3];
	int giorno_int;

	if(!setup_prepared_stmt(&stmt, "call inserisci_paziente(?,?,?,?,?,?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione statement inserisci paziente fallita\n", false);
	
	// clean memory for param
	memset( (void *) param, 0, sizeof(param));
	
	// filling parameters
	printf("\nNumero tessera sanitaria: ");
	getInput(46, num_ts, false);
	num_ts_int = atoi(num_ts);
	printf("Nome: ");
	getInput(46, nome, false);
	printf("Cognome: ");
	getInput(46, cognome, false);
	
	printf("Giorno di nascita: ");
	getInput(3, giorno, false);
	giorno_int = atoi(giorno);
	data_nascita.day = giorno_int;
	
	printf("Mese di nascita: ");
	getInput(3, mese, false);
	mese_int = atoi(mese);
	data_nascita.month = mese_int;
	
	printf("Anno di nascita: ");
	getInput(5, anno, false);
	anno_int = atoi(anno);
	data_nascita.year = anno_int;
	
	printf("Luogo di nascita: ");
	getInput(46, luogo_nascita, false);
	printf("Residenza: ");
	getInput(46, residenza, false);
	printf("Recapito: ");
	getInput(46, recapito, false);
	
	// set up parameters
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = (void *) &num_ts_int;
	
	param[1].buffer_type = MYSQL_TYPE_STRING;
	param[1].buffer = (void *) nome;
	param[1].buffer_length = strlen(nome);
	
	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = cognome;
	param[2].buffer_length = strlen(cognome);

	param[3].buffer_type = MYSQL_TYPE_DATE;
	param[3].buffer = (char *)&data_nascita;
	param[3].is_null = 0;
	param[3].length= 0;
	
	param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[4].buffer = luogo_nascita;
	param[4].buffer_length = strlen(luogo_nascita);
	
	param[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[5].buffer = residenza;
	param[5].buffer_length = strlen(residenza);
	
	param[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[6].buffer = recapito;
	param[6].buffer_length = strlen(recapito);
	
	// binding parameters
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding statement inserisci paziente fallito\n", true);
	
	// execute stmt
	if(mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute statement inserisci paziente fallita\n");
	else
		printf("Paziente inserito correttamente\n");
	
	// deallocate stmt handler
	mysql_stmt_close(stmt);
}

static void modifica_paziente(MYSQL *conn) {
	// print patient info and save her/his tessera sanitaria number
	int num_ts = stampa_anagrafica(conn, false);
	if(num_ts == -1)
		return;
	
	// update procedure
	MYSQL_STMT *stmt;
	MYSQL_BIND param[5]; // int numTS, int colonna, int new_numTS, varchar str, data
	
	int colonna;
	int new_ts_int;
	char new_ts[46];
	char var_str[46];
	MYSQL_TIME var_data;
	char giorno[3], mese[3], anno[5];
	my_bool newts_is_null = true;
	my_bool varstr_is_null = true;
	my_bool data_is_null = true;
	
	if(!setup_prepared_stmt(&stmt, "call modifica_paziente(?,?,?,?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione statement modifica paziente fallita\n", false);
	
	// which attribute to update?
	char options[6] = {'1', '2', '3', '4', '5', '6'};
	char op;
	printf("\nQuale campo modificare?\n");
	printf("1) Numero Tessera Sanitaria\n");
	printf("2) Nome\n");
	printf("3) Cognome\n");
	printf("4) Data di nascita\n");
	printf("5) Luogo di nascita\n");
	printf("6) Residenza\n");
	op = multiChoice("Scegli un'opzione", options, 6);
	
	colonna = op - '0';
	
	switch(op) {
		case '1':
			newts_is_null = false;
			printf("Nuovo numero Tessera Sanitaria del paziente: ");
			getInput(46, new_ts, false);
			new_ts_int = atoi(new_ts);
			break;
		case '2':
			varstr_is_null = false;
			printf("\nInserisci nuovo nome: ");
			getInput(46, var_str, false);
			break;
		case '3':
			varstr_is_null = false;
			printf("\nInserisci nuovo cognome: ");
			getInput(46, var_str, false);
			break;
		case '4':
			data_is_null = false;
			printf("Inserisci nuova data di nascita\n\tGiorno : ");
			getInput(3, giorno, false);
			printf("\tMese : ");
			getInput(3, mese, false);
			printf("\tAnno [formato YYYY] : ");
			getInput(5, anno, false);
			var_data.year = atoi(anno);
			var_data.month = atoi(mese);
			var_data.day = atoi(giorno);
			break;
		case '5':
			varstr_is_null = false;
			printf("\nInserisci nuovo luogo di nascita: ");
			getInput(46, var_str, false);
			break;
		case '6':
			varstr_is_null = false;
			printf("\nInserisci nuova residenza: ");
			getInput(46, var_str, false);
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}
	
	// param[2,3,4] initially NULL, the selected column was changed to not NULL in the switch-case
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &num_ts;
	param[0].buffer_length = sizeof(num_ts);
	
	param[1].buffer_type = MYSQL_TYPE_LONG;
	param[1].buffer = &colonna;
	param[1].buffer_length = sizeof(colonna);
	
	param[2].buffer_type = MYSQL_TYPE_LONG;
	param[2].buffer = &new_ts_int;
	param[2].buffer_length = sizeof(new_ts_int);
	param[2].is_null = &newts_is_null;
	
	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = var_str;
	param[3].buffer_length = strlen(var_str);
	param[3].is_null = &varstr_is_null;
	
	param[4].buffer_type = MYSQL_TYPE_DATE;
	param[4].buffer = &var_data;
	param[4].buffer_length = sizeof(var_data);
	param[4].is_null = &data_is_null;
	
	// bind param
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding statement modifica paziente fallita\n", true);
	
	// run procedure
	if(mysql_stmt_execute(stmt) != 0){
		if( strcmp( mysql_stmt_sqlstate(stmt), "45004") == 0 ) // deleting a non existing patient
			printf("\nERROR: il paziente da modificare non esiste!\n");
		
		print_stmt_error(stmt, "Execute statement modifica paziente fallita\n");
	} else {
		printf("Paziente modificato correttamente\n");
	}
	
	mysql_stmt_close(stmt);
}

static void cancella_paziente(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[1];
	
	// input for the routine
	char num_ts[46];
	int num_ts_int;
	
	if(!setup_prepared_stmt(&stmt, "call cancella_paziente(?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione statement cancella paziente fallita\n", false);
	
	// clean memory for param
	memset( (void *) param, 0, sizeof(param));
	
	// filling parameters
	printf("\nNumero tessera sanitaria: ");
	getInput(46, num_ts, false);
	num_ts_int = atoi(num_ts);
	
	// set up parameters
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = (void *) &num_ts_int;

	// binding parameters
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding statement cancella paziente fallito\n", true);
	
	// execute stmt
	if(mysql_stmt_execute(stmt) != 0) {
		if(strcmp(mysql_stmt_sqlstate(stmt), "45003") == 0) // deleting a non existing patient
			printf("Il paziente da eliminare non esiste!\n");
		print_stmt_error(stmt, "Execute statement cancella paziente fallita\n");
	} else {
		printf("Paziente con tessera sanitaria n° %d eliminato correttamente.\n", num_ts_int);
	}
	
	// deallocate stmt handler
	mysql_stmt_close(stmt);
}

static void inserisci_recapito(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[2];
	
	// inputs for the routine
	char num_ts[46];
	int num_ts_int;
	char recapito[46];
	
	// get the information
	printf("\nNumero Tessera Sanitaria: ");
	getInput(46, num_ts, false);
	num_ts_int = atoi(num_ts);
	printf("\nRecapito: ");
	getInput(46, recapito, false);
	
	// prepare stored procedure call
	if(!setup_prepared_stmt(&stmt, "call inserisci_recapito(?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione statement inserisci recapito fallita\n", false);
	
	// prepare parameters
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = (void *) &num_ts_int;
	
	param[1].buffer_type = MYSQL_TYPE_STRING;
	param[1].buffer = (void *) recapito;
	param[1].buffer_length = strlen(recapito);
	
	// bind param
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding statement inserisci recapito fallita\n", true);
	
	// run procedure
	if(mysql_stmt_execute(stmt) != 0)
		print_stmt_error(stmt, "Execute statement inserisci recapito fallita\n");
	else
		printf("Recapito aggiunto correttamente\n");
	
	mysql_stmt_close(stmt);
}

static void cancella_recapito(MYSQL *conn) {
	int num_ts_int;
	char num_ts[46];
	
	char recapito[46];
	
	// ask for tessera sanitaria number
	printf("\nNumero tessera sanitaria: ");
	getInput(46, num_ts, false);
	num_ts_int = atoi(num_ts);
	
	// print contacts for that patient
	stampa_recapiti(conn, num_ts_int);
	
	// ask which to delete
	printf("\nInserire recapito da cancellare: ");
	getInput(46, recapito, false);
	
	// prepare stmt
	MYSQL_STMT *stmt;
	MYSQL_BIND param[2]; // int numTS, varchar campo
	
	if(!setup_prepared_stmt(&stmt, "call cancella_recapito(?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione cancella recapito fallita", false);
	
	// set param
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = (void *) &num_ts_int;
	param[0].buffer_length = sizeof(num_ts_int);
	
	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = recapito;
	param[1].buffer_length = strlen(recapito);
	
	// bind
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding cancella recapito fallita\n", true);
	
	// exec and close
	if(mysql_stmt_execute(stmt) != 0) {
		if( strcmp( mysql_stmt_sqlstate(stmt), "45001") == 0 ) // deleting a non existing contact
			printf("\nERROR: il contatto da eliminare non esiste!\n");
		if(strcmp(mysql_stmt_sqlstate(stmt), "45002") == 0) // the user can't delete the only contact of a patient
			printf("\nERROR: impossibile cancellare l'unico contatto di un paziente!\n");
		print_stmt_error(stmt, "Execute cancella recapito fallita\n");
	} else {
		printf("Contatto eliminato correttamente.\n");
	}
	
	mysql_stmt_close(stmt);
}

static void prenota_esame(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[8];
	
	// inputs
	MYSQL_TIME data_ts;		// param[0]
	char giorno[3], mese[3], anno[5];
	char orario[6];			// param[1]
	char ore[3], minuti[3];
	float costo_f;			// param[2]
	char costo[46];
	my_bool urgente;		// param[3]
	int prenotazione = 0;	// param[4]
	my_bool pren_is_null = true;
	int paziente_int;		// param[5]
	char paziente[46];
	char tipo[46];			// param[6]
	int laboratorio_int;	// param[7]
	char laboratorio[46];
	
	char options[2] = {'1', '2'};
	char u;
	char continua[2] = {'1', '2'};
	char c;

	bool prenota_ancora = true;
	
	while(prenota_ancora) {
		if(prenotazione != 0)
			pren_is_null = false;
		
		/* print reservable exams */
		if(!setup_prepared_stmt(&stmt, "call esami_disponibili()", conn))
			finish_with_stmt_error(conn, stmt, "Inizialiazzazione lista esami disponibili fallita\n", false);
		if(mysql_stmt_execute(stmt) != 0)
			finish_with_stmt_error(conn, stmt, "Execute lista esami disponibili fallita\n", true);
		dump_result_set(conn, stmt, "\nLista degli esami disponibili");
		if(mysql_stmt_next_result(stmt) > 0)
			finish_with_stmt_error(conn, stmt, "Errore nello scorrere i result sets lista esami disponibili\n", true);
		mysql_stmt_close(stmt);
		
		// select exam type
		printf("\nTipo esame: ");
		getInput(46, tipo, false);
		
		/* show laboratories */
		if(!setup_prepared_stmt(&stmt, "call lista_laboratori()", conn))
			finish_with_stmt_error(conn, stmt, "Inizializzazione lista laboratori fallita\n", false);
		if(mysql_stmt_execute(stmt) != 0)
			finish_with_stmt_error(conn, stmt, "Execute lista laboratori fallita\n", true);
		dump_result_set(conn, stmt, "\nLista dei laboratori");
		if(mysql_stmt_next_result(stmt) > 0)
			finish_with_stmt_error(conn, stmt, "Errore nello scorrere i result sets lista laboratori\n", true);
		mysql_stmt_close(stmt);
		
		// select lab
		printf("\nInserisci ID laboratorio: ");
		getInput(46, laboratorio, false);
		laboratorio_int = atoi(laboratorio);
		
		// inputs
		printf("Data esame\n\tGiorno [formato GG] : ");
		getInput(3, giorno, false);
		printf("\tMese [formato MM] : ");
		getInput(3, mese, false);
		printf("\tAnno [formato YYYY] : ");
		getInput(5, anno, false);
		data_ts.year = atoi(anno);
		data_ts.month = atoi(mese);
		data_ts.day = atoi(giorno);
		
		printf("Orario esame\n\tOre [formato HH] : ");
		getInput(3, ore, false);
		printf("\tMinuti [formato MM] : ");
		getInput(3, minuti, false);
		// send time as a "HH:MM" string...
		sprintf(orario, "%d:%d", atoi(ore), atoi(minuti));
		
		printf("Costo esame [formato € xx.xx ] :  € ");
		getInput(46, costo, false);
		costo_f = atof(costo);
		
		printf("Numero Tessera Sanitaria del paziente: ");
		getInput(46, paziente, false);
		paziente_int = atoi(paziente);
		
		printf("\nEsame prescritto con urgenza?\n");
		printf("\t1) URGENTE");
		printf("\t2) non urgente\n");
		u = multiChoice("Seleziona", options, 2);
		switch(u) {
			case '1':
				urgente = true;
				break;
			case '2':
				urgente = false;
				break;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}
		
		// prepare
		if(!setup_prepared_stmt(&stmt, "call prenota_esame(?,?,?,?,?,?,?,?)", conn))
			finish_with_stmt_error(conn, stmt, "Inizializzazione statement prenota esame fallita\n", false);
		
		// memset, parameters
		memset(param, 0, sizeof(param));
		
		param[0].buffer_type = MYSQL_TYPE_DATE;
		param[0].buffer = &data_ts;
		param[0].buffer_length = sizeof(data_ts);
		
		// send the time as a "HH:MM" string
		param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
		param[1].buffer = orario;
		param[1].buffer_length = strlen(orario);
		
		param[2].buffer_type = MYSQL_TYPE_FLOAT;
		param[2].buffer = &costo_f;
		param[2].buffer_length = sizeof(costo_f);
		
		param[3].buffer_type = MYSQL_TYPE_TINY;
		param[3].buffer = (char *) &urgente;
		param[3].buffer_length = sizeof(urgente);
		
		// for the first iteration param[4] is null
		// then set the reservation number with the value returned by the prepared statement
		// see the sql routine implementation for more info
		param[4].buffer_type = MYSQL_TYPE_LONG;
		param[4].buffer = &prenotazione;
		param[4].buffer_length = sizeof(prenotazione);
		param[4].is_null = &pren_is_null;
		
		param[5].buffer_type = MYSQL_TYPE_LONG;
		param[5].buffer = &paziente_int;
		param[5].buffer_length = sizeof(paziente_int);
		
		param[6].buffer_type = MYSQL_TYPE_VAR_STRING;
		param[6].buffer = tipo;
		param[6].buffer_length = strlen(tipo);
		
		param[7].buffer_type = MYSQL_TYPE_LONG;
		param[7].buffer = &laboratorio_int;
		param[7].buffer_length = sizeof(laboratorio_int);
		
		// bind
		if(mysql_stmt_bind_param(stmt, param) != 0)
			finish_with_stmt_error(conn, stmt, "Binding prenota_esame() fallito\n", true);
		
		// execute
		if(mysql_stmt_execute(stmt) != 0) {
			if(mysql_stmt_errno(stmt) == 1062)
				printf("\nUSER ERROR: impossibile prenotare lo stesso tipo di esame nella stessa data per lo stesso paziente!\n");
			if(mysql_stmt_errno(stmt) == 1644)
				printf("\nUSER ERROR: impossibile prenotare un esame nel passato!\n");
			print_stmt_error(stmt, "Errore nell'esecuzione dello statement prenota_esame()\n");
			mysql_stmt_close(stmt);
			return;
		}
		
		// save the reservation number returned by the prepared statement
		memset(param, 0, sizeof(param));
		param[0].buffer_type = MYSQL_TYPE_LONG;
		param[0].buffer = &prenotazione;
		param[0].buffer_length = sizeof(prenotazione);
		
		if(mysql_stmt_bind_result(stmt, param))
			finish_with_stmt_error(conn, stmt, "Binding prenota esame fallita", true);
		
		if(mysql_stmt_store_result(stmt))
			finish_with_stmt_error(conn, stmt, "Store result prenota esame fallita", true);
		
		while(!mysql_stmt_fetch(stmt)) // 1x1 result set = prenotazione
			printf("\n\t*** NUMERO DI PRENOTAZIONE ASSEGNATO:  %d  ***\n", prenotazione);
		
		// ask to continue
		printf("\nPrenotare altri esami?\n");
		printf("\t1) Sì");
		printf("\t2) No\n");
		c = multiChoice("Seleziona", continua, 2);
		switch(c) {
			case '1':
				prenota_ancora = true;
				break;
			case '2':
				prenota_ancora = false;
				break;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}
		
		if(mysql_stmt_next_result(stmt) > 0)
			finish_with_stmt_error(conn, stmt, "Errore nello scorrere i risultati in prenota_esame()\n", true);
		
		// close stmt
		mysql_stmt_close(stmt);
	}
}

void gestisci_anagrafica(MYSQL *conn) {
	char options[7] = {'1', '2', '3', '4', '5', '6', '7'};
	char op;
	
	while(true) {
		printf("\033[2J\033[H");
		printf("CUP - Gestione Anagrafica\n");
		printf("*** COSA VUOI FARE ***\n\n");
		printf("1) Stampa anagrafica\n");
		printf("2) Inserisci paziente\n");
		printf("3) Modifica paziente\n");
		printf("4) Cancella paziente\n");
		printf("5) Aggiungi recapito\n");
		printf("6) Cancella recapito\n");
		printf("7) Indietro\n");
		
		op = multiChoice("Scegli un'opzione", options, 7);
		
		switch(op) {
			case '1':
				stampa_anagrafica(conn, true);
				break;
			case '2':
				inserisci_paziente(conn);
				break;
			case '3':
				modifica_paziente(conn);
				break;
			case '4':
				cancella_paziente(conn);
				break;
			case '5':
				inserisci_recapito(conn);
				break;
			case '6':
				cancella_recapito(conn);
				break;
			case '7':
				return;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}
		printf("\npremi il tasto INVIO per continuare\n");
		getchar();
	}
}

void run_as_cup(MYSQL *conn) {
	char options[5] = {'1', '2', '3', '4', '5'};
	char op;
	
	printf("Switching to CUP role...\n\n");
	
	if(!parse_config("users/cup.json", &conf)) {
		fprintf(stderr, "Unable to load CUP configuration\n");
		exit(EXIT_FAILURE);
	}
	
	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		print_error(conn, "mysql_change_user() failed");
		exit(EXIT_FAILURE);
	}
	
	while(true) {
		printf("\033[2J\033[H");
		printf("CUP\n");
		printf("*** COSA VUOI FARE ***\n\n");
		printf("1) Gestisci anagrafica\n");
		printf("2) Prenota esame\n");
		printf("3) Report prenotazione\n");
		printf("4) Report storico paziente\n");
		printf("5) Esci\n");
		
		op = multiChoice("Scegli un'opzione", options, 5);
		
		switch(op) {
			case '1':
				gestisci_anagrafica(conn);
				break;
			case '2':
				prenota_esame(conn);
				break;
			case '3':
				report_prenotazione(conn);
				break;
			case '4':
				report_paziente(conn);
				break;
			case '5':
				return;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}
		printf("\npremi il tasto INVIO per continuare\n");
		getchar();
	}
}

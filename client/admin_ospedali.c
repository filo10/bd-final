#include "defines.h"

void stampa_lista_ospedali(MYSQL *conn) {
	MYSQL_STMT *stmt;
	
	if(!setup_prepared_stmt(&stmt, "call lista_ospedali()", conn))
		finish_with_stmt_error(conn, stmt, "Inizialiazzazione lista ospedali fallita\n", false);
	
	if(mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "Execute lista ospedali fallita\n");
		goto out;
	}
	
	dump_result_set(conn, stmt, "\nLista degli ospedali della ASL");
	if(mysql_stmt_next_result(stmt) > 0)
		finish_with_stmt_error(conn, stmt, "Errore nello scorrere i result sets lista ospedali di stampa_info_ospedale()\n", true);
 out:
	mysql_stmt_close(stmt);
}

static void stampa_info_ospedale(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[1];
	
	char ospedale[46];
	int ospedale_int;
	
	// print hospitals
	stampa_lista_ospedali(conn);
	
	// select hospital to print
	printf("\nInserisci codice ospedale: ");
	getInput(46, ospedale, false);
	ospedale_int = atoi(ospedale);
	
	if(!setup_prepared_stmt(&stmt, "call info_ospedale(?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizialiazzazione info ospedale fallita\n", false);
	
	// bind, execute
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &ospedale_int;
	param[0].buffer_length = sizeof(ospedale_int);
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding info ospedale fallito\n", true);
	
	if(mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "Execute info ospedale fallita\n");
		goto out;
	}
	
	// print the 3 result sets returned
	dump_result_set(conn, stmt, "\nInfo degll'ospedale");
	if(mysql_stmt_next_result(stmt) > 0)
		finish_with_stmt_error(conn, stmt, "Errore nello scorrere i result sets info ospedale di stampa_info_ospedale()\n", true);
	
	dump_result_set(conn, stmt, "\nLista dei reparti dell'ospedale");
	if(mysql_stmt_next_result(stmt) > 0)
		finish_with_stmt_error(conn, stmt, "Errore nello scorrere i result sets dei reparti di stampa_info_ospedale()\n", true);
	
	dump_result_set(conn, stmt, "\nLista dei laboratori dell'ospedale");
	if(mysql_stmt_next_result(stmt) > 0)
		finish_with_stmt_error(conn, stmt, "Errore nello scorrere i result sets dei lab di stampa_info_ospedale()\n", true);
	
 out:
	mysql_stmt_close(stmt);
}

static void aggiungi_ospedale(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[7]; // nome_osp, indir_osp, medico, nome_rep, tel_rep, specializzaz, OUT osp_code
	
	// input for the routine
	char nome_osp[46];
	char indirizzo_osp[46];
	char cf_medico[17];
	char nome_rep[46];
	char tel_rep[46];
	char specializzazione[46];
	
	int osp_code;
	
	bool success = false;
	
	if(!setup_prepared_stmt(&stmt, "call inserisci_ospedale(?,?,?,?,?,?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione inserisci ospedale fallita\n", false);
	
	// clean memory for param
	memset( (void *) param, 0, sizeof(param));
	
	// filling parameters
	printf("\nAttenzione: è necessario inserire il codice fiscale di un membro del personale di tipo 'medico' già presente che non sia responsabile di un ospedale o un laboratorio.");
	printf("\nIl medico scelto verrà trasferito nel nuovo ospedale e ne sarà responsabile, e sarà primario del reparto che verrà inserito contestualmente al nuovo ospedale\n");
	printf("\nInserisci Codice Fiscale del responsabile del nuovo ospedale e primario del nuovo reparto: ");
	getInput(17, cf_medico, false);
	printf("Inserisci specializzazione del nuovo primario: ");
	getInput(46, specializzazione, false);
	
	printf("Nome del nuovo ospedale: ");
	getInput(46, nome_osp, false);
	printf("Indirizzo del nuovo ospedale: ");
	getInput(46, indirizzo_osp, false);
	printf("Nome del nuovo reparto: ");
	getInput(46, nome_rep, false);
	printf("Telefono del nuovo reparto: ");
	getInput(46, tel_rep, false);
	
	// set up parameters
	param[0].buffer_type = MYSQL_TYPE_STRING;
	param[0].buffer = (void *) nome_osp;
	param[0].buffer_length = strlen(nome_osp);
	
	param[1].buffer_type = MYSQL_TYPE_STRING;
	param[1].buffer = (void *) indirizzo_osp;
	param[1].buffer_length = strlen(indirizzo_osp);
	
	param[2].buffer_type = MYSQL_TYPE_STRING;
	param[2].buffer = (void *) cf_medico;
	param[2].buffer_length = strlen(cf_medico);
	
	param[3].buffer_type = MYSQL_TYPE_STRING;
	param[3].buffer = (void *) nome_rep;
	param[3].buffer_length = strlen(nome_rep);
	
	param[4].buffer_type = MYSQL_TYPE_STRING;
	param[4].buffer = (void *) tel_rep;
	param[4].buffer_length = strlen(tel_rep);
	
	param[5].buffer_type = MYSQL_TYPE_STRING;
	param[5].buffer = (void *) specializzazione;
	param[5].buffer_length = strlen(specializzazione);
	
	param[6].buffer_type = MYSQL_TYPE_LONG;
	param[6].buffer = &osp_code;
	param[6].buffer_length = sizeof(osp_code);
	
	// binding parameters
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding inserisci ospedale fallito\n", true);
	
	// execute stmt
	if(mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "Execute inserisci ospedale fallito\n");
		mysql_stmt_close(stmt);
		return;
	}
	
	// save the hospital code
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &osp_code;
	param[0].buffer_length = sizeof(osp_code);
	
	if(mysql_stmt_bind_result(stmt, param))
		finish_with_stmt_error(conn, stmt, "Binding risultato inserisci ospedale fallito", true);
	
	if(mysql_stmt_store_result(stmt))
		finish_with_stmt_error(conn, stmt, "Store result inserisci ospedale fallita", true);
	
	while(!mysql_stmt_fetch(stmt)) // 1x1 result set = osp_code
	
	if(mysql_stmt_next_result(stmt) > 0)
		finish_with_stmt_error(conn, stmt, "Errore nello scorrere i risultati in inserisci ospedale\n", true);
	
	// deallocate stmt handler
	mysql_stmt_close(stmt);
	
	// inserisci un laboratorio in questo ospedale
	success = aggiungi_laboratorio(conn, true, cf_medico, osp_code);
	
	if(success)
		printf("\nNuovo ospedale inserito correttamente (codice ospedale = %d).\n", osp_code);
	else
		printf("\nATTENZIONE qualcosa è andato storto ed è stato inserito un ospedale senza alcun laboratorio\n");
}

static void elimina_ospedale(MYSQL *conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[2];
	
	char ospedale[46];
	int ospedale_int;
	
	int new_reparto;
	
	// get input
	printf("\nInserisci codice ospedale: ");
	getInput(46, ospedale, false);
	ospedale_int = atoi(ospedale);
	
	if(!setup_prepared_stmt(&stmt, "call cancella_ospedale(?,?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizialiazzazione cancella ospedale fallita\n", false);
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &ospedale_int;
	param[0].buffer_length = sizeof(ospedale_int);
	
	param[1].buffer_type = MYSQL_TYPE_LONG;
	param[1].buffer = &new_reparto;
	param[1].buffer_length = sizeof(new_reparto);
	
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding cancellla ospedale fallito\n", true);
	
	if(mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "Execute cancella ospedale fallita\n");
		goto out;
	}
	
	// get the new ward id where every personnel member from the deleted hospital is moved
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &new_reparto;
	param[0].buffer_length = sizeof(new_reparto);
	
	if(mysql_stmt_bind_result(stmt, param))
		finish_with_stmt_error(conn, stmt, "Binding risultato cancella ospedale fallito", true);
	
	if(mysql_stmt_store_result(stmt))
		finish_with_stmt_error(conn, stmt, "Store result cancella ospedale fallita", true);
	
	while(!mysql_stmt_fetch(stmt)) // 1x1 result set = new_reparto
	
	if(mysql_stmt_next_result(stmt) > 0)
		finish_with_stmt_error(conn, stmt, "Errore nello scorrere i risultati in cancella ospedale\n", true);
	
	printf("Ospedale eliminato correttamente\nTutti i membri del personale sono stati spostati nel reparto con ID = %d\n", new_reparto);
	
 out:
	mysql_stmt_close(stmt);
}

void gestione_ospedali(MYSQL *conn) {
	char options[5] = {'1', '2', '3', '4', '5'};
	char op;
	
	while(true) {
		printf("\033[2J\033[H");
		printf("ADMIN - Gestione Ospedali\n");
		printf("*** COSA VUOI FARE ***\n\n");
		printf("1) Stampa info ospedale\n");
		printf("2) Aggiungi ospedale\n");
		printf("3) Modifica ospedale\n");
		printf("4) Elimina ospedale\n");
		printf("5) Indietro\n");
		
		op = multiChoice("Scegli un'opzione", options, 5);
		
		switch(op) {
			case '1':
				stampa_info_ospedale(conn);
				break;
			case '2':
				aggiungi_ospedale(conn);
				break;
			case '3':
				gestione_interno_ospedale(conn);
				break;
			case '4':
				elimina_ospedale(conn);
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

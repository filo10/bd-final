#include "defines.h"

struct exam_list {
	char type[46];
	char date[11];
	char doctor[92];
	char diagnosis[257];
};

struct exam_list_big {
	char type[46];
	char date[11];
	char doctor[92];
	char diagnosis[257];
	char patient[92];
};

struct reservation_list {
	char type[46];
	char patient[92];
	char date[11];
	char time[6];
	char lab[46];
	char hosp[46];
	bool urgent;
	double cost;
};

struct exam_param {
	char name[46];
	double value;
	char measure[46];
};

static size_t parse_exparam(MYSQL *conn, MYSQL_STMT *stmt, struct exam_param **params) {
	int status;
	size_t row = 0;
	MYSQL_BIND param[3];
	
	char nome_param[46];
	double valore;
	char misura[46];
	my_bool is_null;
	
	if(mysql_stmt_store_result(stmt))
		finish_with_stmt_error(conn, stmt, "Store result parse_exparam fallita", true);
	
	*params = malloc(mysql_stmt_num_rows(stmt) * sizeof(struct exam_param));
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = nome_param;
	param[0].buffer_length = 46;

	param[1].buffer_type = MYSQL_TYPE_DOUBLE;
	param[1].buffer = &valore;
	param[1].buffer_length = sizeof(valore);
	
	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = misura;
	param[2].buffer_length = 46;
	param[2].is_null = &is_null;
	
	if(mysql_stmt_bind_result(stmt, param))
		finish_with_stmt_error(conn, stmt, "Binding di parse_exparam fallita\n", true);
	
	// assemble struct exam_param
	while(true) {
		status = mysql_stmt_fetch(stmt);
		
		if(status == 1 || status == MYSQL_NO_DATA)
			break;
		
		strcpy((*params)[row].name, nome_param);
		(*params)[row].value = valore;
		if(is_null)
			strcpy((*params)[row].measure, "");
		else
			strcpy((*params)[row].measure, misura);
		
		row++;
	}
	
	return row;
}

static size_t parse_exams(MYSQL *conn, MYSQL_STMT *stmt, struct exam_list **list) {
	int status;
	size_t row = 0;
	my_bool is_null;
	MYSQL_BIND param[5];
	
	char nome_esame[46];
	MYSQL_TIME data;
	char nome_dottore[46];
	char cognome_dottore[46];
	char diagnosi[257];
	
	if(mysql_stmt_store_result(stmt))
		finish_with_stmt_error(conn, stmt, "Store result parse_exams fallita", true);
	
	*list = malloc(mysql_stmt_num_rows(stmt) * sizeof(struct exam_list));
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = nome_esame;
	param[0].buffer_length = 46;

	param[1].buffer_type = MYSQL_TYPE_DATE;
	param[1].buffer = &data;
	param[1].buffer_length = sizeof(data);
	
	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = nome_dottore;
	param[2].buffer_length = 46;
	
	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = cognome_dottore;
	param[3].buffer_length = 46;
	
	param[4].buffer_type = MYSQL_TYPE_STRING;
	param[4].buffer = diagnosi;
	param[4].buffer_length = 257;
	param[4].is_null = &is_null;
	
	if(mysql_stmt_bind_result(stmt, param))
		finish_with_stmt_error(conn, stmt, "Binding di parse_exams fallita\n", true);
	
	// assemble struct exam_list
	while(true) {
		status = mysql_stmt_fetch(stmt);
		
		if(status == 1 || status == MYSQL_NO_DATA)
			break;
		
		strcpy((*list)[row].type, nome_esame);
		sprintf((*list)[row].date, "%02d/%02d/%4d", data.day, data.month, data.year);
		sprintf((*list)[row].doctor, "%s %s", cognome_dottore, nome_dottore);
		if(is_null) {
			sprintf((*list)[row].diagnosis, "Nessuna diagnosi inserita.");
		} else {
			sprintf((*list)[row].diagnosis, "%s", diagnosi);
		}
		
		row++;
	}
	return row;
}

static size_t parse_exams_with_patient_info(MYSQL *conn, MYSQL_STMT *stmt, struct exam_list_big **list) {
	int status;
	size_t row = 0;
	my_bool is_null;
	MYSQL_BIND param[7];
	
	char nome_esame[46];
	MYSQL_TIME data;
	char nome_dottore[46];
	char cognome_dottore[46];
	char diagnosi[257];
	char nome_paz[46];
	char cognome_paz[46];
	
	if(mysql_stmt_store_result(stmt))
		finish_with_stmt_error(conn, stmt, "Store result parse_exams_with_patient_info fallita", true);
	
	*list = malloc(mysql_stmt_num_rows(stmt) * sizeof(struct exam_list_big));
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = nome_esame;
	param[0].buffer_length = 46;

	param[1].buffer_type = MYSQL_TYPE_DATE;
	param[1].buffer = &data;
	param[1].buffer_length = sizeof(data);
	
	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = nome_dottore;
	param[2].buffer_length = 46;
	
	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = cognome_dottore;
	param[3].buffer_length = 46;
	
	param[4].buffer_type = MYSQL_TYPE_STRING;
	param[4].buffer = diagnosi;
	param[4].buffer_length = 257;
	param[4].is_null = &is_null;
	
	param[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[5].buffer = nome_paz;
	param[5].buffer_length = 46;
	
	param[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[6].buffer = cognome_paz;
	param[6].buffer_length = 46;
	
	if(mysql_stmt_bind_result(stmt, param))
		finish_with_stmt_error(conn, stmt, "Binding di parse_exams fallita\n", true);
	
	// assemble struct exam_list
	while(true) {
		status = mysql_stmt_fetch(stmt);
		
		if(status == 1 || status == MYSQL_NO_DATA)
			break;
		
		strcpy((*list)[row].type, nome_esame);
		sprintf((*list)[row].date, "%02d/%02d/%4d", data.day, data.month, data.year);
		sprintf((*list)[row].doctor, "%s %s", cognome_dottore, nome_dottore);
		if(is_null)
			sprintf((*list)[row].diagnosis, "Nessuna diagnosi inserita.");
		else
			sprintf((*list)[row].diagnosis, "%s", diagnosi);
		sprintf((*list)[row].patient, "%s %s", cognome_paz, nome_paz);
		
		row++;
	}
	
	return row;
}

static size_t parse_reservations(MYSQL *conn, MYSQL_STMT *stmt, struct reservation_list **list) {
	int status;
	size_t row = 0;
	MYSQL_BIND param[9];
	
	char tipo[46];
	char nome[46];
	char cognome[46];
	MYSQL_TIME data;
	MYSQL_TIME ora;
	char laboratorio[46];
	char ospedale[46];
	my_bool urgente;
	double costo;
	
	if(mysql_stmt_store_result(stmt))
		finish_with_stmt_error(conn, stmt, "Store result parse_reservations fallita", true);
	
	*list = malloc(mysql_stmt_num_rows(stmt) * sizeof(struct reservation_list));
	
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = tipo;
	param[0].buffer_length = 46;
	
	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = nome;
	param[1].buffer_length = 46;
	
	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = cognome;
	param[2].buffer_length = 46;

	param[3].buffer_type = MYSQL_TYPE_DATE;
	param[3].buffer = &data;
	param[3].buffer_length = sizeof(data);
	
	param[4].buffer_type = MYSQL_TYPE_DATE;
	param[4].buffer = &ora;
	param[4].buffer_length = sizeof(ora);
	
	param[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[5].buffer = laboratorio;
	param[5].buffer_length = 46;
	
	param[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[6].buffer = ospedale;
	param[6].buffer_length = 46;
	
	param[7].buffer_type = MYSQL_TYPE_TINY;
	param[7].buffer = (char *) &urgente;
	param[7].buffer_length = sizeof(urgente);
	
	param[8].buffer_type = MYSQL_TYPE_DOUBLE;
	param[8].buffer = &costo;
	param[8].buffer_length = sizeof(costo);
	
	
	if(mysql_stmt_bind_result(stmt, param))
		finish_with_stmt_error(conn, stmt, "Binding di parse_exams fallita\n", true);
	
	// assemble struct reservation_list
	while(true) {
		status = mysql_stmt_fetch(stmt);
		
		if(status == 1 || status == MYSQL_NO_DATA)
			break;
		
		strcpy((*list)[row].type, tipo);
		sprintf((*list)[row].patient, "%s %s", cognome, nome);
		sprintf((*list)[row].date, "%02d/%02d/%4d", data.day, data.month, data.year);
		sprintf((*list)[row].time, "%02d:%02d", ora.hour, ora.minute);
		strcpy((*list)[row].lab, laboratorio);
		strcpy((*list)[row].hosp, ospedale);
		if(urgente) {
			(*list)[row].urgent = true;
		} else {
			(*list)[row].urgent = false;
		}
		(*list)[row].cost = costo;
		
		row++;
	}
	
	return row;
}

void report_paziente(MYSQL* conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[1];
	
	int status;
	bool first = true;
	struct exam_list *exams;
	size_t i = 0;
	size_t tabelle = 0;
	
	char num_ts[46];
	int num_ts_int;
	
	// preapre stmt
	if(!setup_prepared_stmt(&stmt, "call report_storico_paz(?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione report storico paziente fallita\n", false);
	
	// prepare params
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = (void *) &num_ts_int;
	
	// fill param
	printf("\nNumero tessera sanitaria: ");
	getInput(46, num_ts, false);
	num_ts_int = atoi(num_ts);
	
	// bind
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding report storico paziente fallita\n", true);
	
	// run procedure
	if(mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "Execute report storico paziente fallita\n");
		goto out;
	}
	
	// multiple reuslt sets
	// first table: examType | date | doctor | diagnosys
	// next: parameter | value
	
	do {
		// skip OUT variables even if there are not
		if(conn->server_status & SERVER_PS_OUT_PARAMS)
			goto next;
		
		if(first) {
			tabelle = parse_exams(conn, stmt, &exams);
			first = false;
		} else {
			if(i >= tabelle)
				goto next;
			
			printf("\n+-----------------------------------------------------------------------------------");
			printf("\n| Esame:\t%s\n| Eseguito il:\t%s\n| Eseguito da:\t%s\n| Diagnosi:\t%s\n|", exams[i].type, exams[i].date, exams[i].doctor, exams[i].diagnosis);
			printf("\n|  # Valori:\n|  #\n");
			
			// dump_result_set() makes float colunmn header 331 wide... printing without a table layout
			struct exam_param *params;
			size_t righe;
			righe = parse_exparam(conn, stmt, &params);

			for(unsigned int j = 0; j < righe; j++) {
				printf("|  #  - %s : %.02f %s\n", params[j].name, params[j].value, params[j].measure);
			}
			printf("|\n+-----------------------------------------------------------------------------------\n");
			i++;
		}
		
	next: // -1 = no more results; 0 = more results, keep looking; >0 = error
		status = mysql_stmt_next_result(stmt);
		if(status > 0)
			finish_with_stmt_error(conn, stmt, "Errore nello scorrere i risultati di report storico paziente\n", true);
	} while (status == 0);
	
 out:
	mysql_stmt_close(stmt);
}

void report_prenotazione(MYSQL* conn) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[1];
	
	int status;
	int giro = 1;
	struct reservation_list *reservations;
	struct exam_list_big *exams;
	size_t i = 0;
	size_t done_exams = 0;
	
	char num_prenotazione[46];
	int num_prenotazione_int;
	
	// preapre stmt
	if(!setup_prepared_stmt(&stmt, "call report_prenotazione(?)", conn))
		finish_with_stmt_error(conn, stmt, "Inizializzazione report storico paziente fallita\n", false);
	
	// prepare params
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = (void *) &num_prenotazione_int;
	
	// fill param
	printf("\nNumero prenotazione: ");
	getInput(46, num_prenotazione, false);
	num_prenotazione_int = atoi(num_prenotazione);
	
	// bind
	if(mysql_stmt_bind_param(stmt, param) != 0)
		finish_with_stmt_error(conn, stmt, "Binding report storico paziente fallita\n", true);
	
	// run procedure
	if(mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "Execute report storico paziente fallita\n");
		goto out;
	}
	
	// multiple reuslt sets
	// first table: type | patient (n+s)| date | time | lab name | H name | cost | urgency
	// secont table: examType | date | doctor | diagnosys | name | surname
	// next: parameter | value
	
	do {
		// skip OUT variables even if there are not
		if(conn->server_status & SERVER_PS_OUT_PARAMS)
			goto next;
		
		if(giro == 1) {
			size_t n_reservations = parse_reservations(conn, stmt, &reservations);
			printf("### ESAMI PRENOTATI ANCORA DA ESEGUIRE (%zd)\n", n_reservations);
			
			// tipo, paz, data, ora, lab, osp, urg, costo
			for(unsigned int r = 0; r < n_reservations; r++) {
				printf("\n %d) Esame: %s\n", r+1, reservations[r].type);
				printf("    Paziente: %s\n", reservations[r].patient);
				printf("    %s ore: %s\n", reservations[r].date, reservations[r].time);
				printf("    Presso: %s, %s\n", reservations[r].lab, reservations[r].hosp);
				printf("    Costo: € %.02f\n", reservations[r].cost);
				if(reservations[r].urgent)
					printf("    *** URGENTE ***\n");
			}
		}
		if(giro == 2) {
			done_exams = parse_exams_with_patient_info(conn, stmt, &exams);
			printf("\n\n### ESAMI ESEGUITI (%zd)\n", done_exams);
		}
		if(giro > 2) {
			if(i >= done_exams) {
				goto next;
			}
			
			printf("\n+-----------------------------------------------------------------------------------");
			printf("\n| Esame:\t%s\n| Eseguito il:\t%s\n| Eseguito da:\t%s\n| Diagnosi:\t%s\n|", exams[i].type, exams[i].date, exams[i].doctor, exams[i].diagnosis);
			printf(" Paziente: %s\n|", exams[i].patient);
			printf("\n|  # Valori:\n|  #\n");
			
			// dump_result_set() makes float colunmn header 331 wide... printing without a table layout
			struct exam_param *params;
			size_t righe;
			righe = parse_exparam(conn, stmt, &params);
			
			for(unsigned int j = 0; j < righe; j++) {
				printf("|  #  - %s: %.02f %s\n", params[j].name, params[j].value, params[j].measure);
			}
			printf("|\n+-----------------------------------------------------------------------------------\n");
			i++;
		}
		
	next: // -1 = no more results; 0 = more results, keep looking; >0 = error
		status = mysql_stmt_next_result(stmt);
		giro++;	
		if(status > 0)
			finish_with_stmt_error(conn, stmt, "Errore nello scorrere i risultati di report storico paziente\n", true);
	} while (status == 0);
	
 out:
	mysql_stmt_close(stmt);
}

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <my_global.h>
#include <my_sys.h>
#include <stdbool.h>
#include <mysql.h>


struct configuration {
	char *host;
	char *db_username;
	char *db_password;
	unsigned int port;
	char *database;

	char username[128];
	char password[128];
};

extern struct configuration conf;

// parse.c
extern int parse_config(char *path, struct configuration *conf);

// inout.c
extern char *getInput(unsigned int lung, char *stringa, bool hide);
extern bool yesOrNo(char *domanda, char yes, char no, bool predef, bool insensitive);
extern char multiChoice(char *domanda, char choices[], int num);

// utils.c
extern void print_error (MYSQL *conn, char *message);
extern void print_stmt_error (MYSQL_STMT *stmt, char *message);
extern void finish_with_error(MYSQL *conn, char *message);
extern void finish_with_stmt_error(MYSQL *conn, MYSQL_STMT *stmt, char *message, bool close_stmt);
extern bool setup_prepared_stmt(MYSQL_STMT **stmt, char *statement, MYSQL *conn);
extern int dump_result_set(MYSQL *conn, MYSQL_STMT *stmt, char *title);

// cup.c
extern void run_as_cup(MYSQL *conn);

// cup_report.c
extern void report_paziente(MYSQL *conn);
extern void report_prenotazione(MYSQL *conn);

// admin.c
extern void run_as_admin(MYSQL *conn);

// admin_personale.c
extern void gestione_personale(MYSQL *conn);

// admin_ospedali.c
extern void stampa_lista_ospedali(MYSQL *conn);
extern void gestione_ospedali(MYSQL *conn);

// admin_modifica_ospedale.c
extern void gestione_interno_ospedale(MYSQL *conn);
extern bool aggiungi_laboratorio(MYSQL *conn, bool from_args, char *resp, int osp_code);

// personale.c
extern void run_as_personale(MYSQL *conn);

#include "defines.h"

typedef enum{
	AMMINISTRATORE = 1,
	CUP,
	PERSONALE,
	FAILED_LOGIN
} role_t;

struct configuration conf;

static MYSQL *conn; // pointer to connection handler

static role_t attempt_login(MYSQL *conn, char *username, char *password) {
	MYSQL_STMT *stmt;
	MYSQL_BIND param[3]; // IN, IN, OUT
	
	int role = 0;
	
	if(!setup_prepared_stmt(&stmt, "call login(?,?,?)", conn)) {
		print_stmt_error(stmt, "Unable to initialize login statement\n");
		goto err2;
	}
	
	// clean memory for param
	memset((void *)param, 0, sizeof(param));
	
	// set up param
	param[0].buffer_type = MYSQL_TYPE_STRING;	// IN
	param[0].buffer = username;
	param[0].buffer_length = strlen(username);
	
	param[1].buffer_type = MYSQL_TYPE_STRING;	// IN
	param[1].buffer = password;
	param[1].buffer_length = strlen(password);
	
	param[2].buffer_type = MYSQL_TYPE_LONG;	// OUT
	param[2].buffer = &role;
	param[2].buffer_length = sizeof(role);
	
	// binding param
	if(mysql_stmt_bind_param(stmt, param) != 0) {
		print_stmt_error(stmt, "mysql_stmt_bind_param() failed");
		exit(EXIT_FAILURE);
	}
	
	// execute stmt
	if(mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "mysql_stmt_execute() failed");
		exit(EXIT_FAILURE);
	}
	
	// prepare output parameters
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &role;
	param[0].buffer_length = sizeof(role);
	
	// binding out param
	if(mysql_stmt_bind_result(stmt, param)) {
		print_stmt_error(stmt, "mysql_stmt_bind_result() failed\n");
		goto err;
	}
	
	// retrieve output param
	if(mysql_stmt_fetch(stmt)) {
		print_stmt_error(stmt, "mysql_stmt_fetch() failed\n");
		goto err;
	}
		
	// deallocate stmt handler
	mysql_stmt_close(stmt);
	return role;
	
err:
	mysql_stmt_close(stmt);
err2:
	return FAILED_LOGIN;
}

int main() {
	role_t role;
	
	if(!parse_config("users/login.json", &conf)) {
		fprintf(stderr, "Unable to load login configuration\n");
		exit(EXIT_FAILURE);
	}
	
	// init connection handler
	conn = mysql_init(NULL);
	if (conn == NULL) {
		print_error(conn, "mysql_init() failed\n");
		exit(EXIT_FAILURE);
	}
	
	// connect to server with db login account
	if(mysql_real_connect(conn, conf.host, conf.db_username, conf.db_password, conf.database, conf.port, NULL, CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS) == NULL) {
		print_error(conn, "mysql_real_connect() failed\n");
		mysql_close(conn);
		exit(EXIT_FAILURE);
	}
	
	// input login
	printf("Username: ");
	getInput(128, conf.username, false);
	printf("Password: ");
	getInput(128, conf.password, true);
	
	role = attempt_login(conn, conf.username, conf.password);
	
	// change db account and run account specific code
	switch(role) {
		case AMMINISTRATORE:
			run_as_admin(conn);
			break;
		case CUP:
			run_as_cup(conn);
			break;
		case PERSONALE:
			run_as_personale(conn);
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}
	
	// disconnect from server
	mysql_close(conn);
	printf("\nA presto!\n\n");
	exit(EXIT_SUCCESS);
}

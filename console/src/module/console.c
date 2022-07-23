/**
 * i-mongo-store.c
 *
 * @file  Módulo iMongo Store
 * @author Tomás Sánchez
 * @since  05.02.2021
 */

#include "console.h"
#include "log.h"
#include "cfg.h"
#include "lib.h"
#include "context.h"
#include "instruction.h"
#include "instruction_lambdas.h"
#include "conexion.h"
#include "accion.h"
#include "smartlist.h"
#include "parser.h"
#include "scanner.h"
#include "module.h"
#include <signal.h>

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

// El modulo servidor propiamente dicho
static console_t this;

t_list *input_file_commands;

#define MODULE_NAME "console"

int yylexerrs = 0;

extern int yynerrs;

// ============================================================================================================
//                                   ***** Funciones Privadas - Declaraciones  *****
// ============================================================================================================

/**
 * @brief Realiza la liberación de memoria. De detectarse la señal de interrupción.
 *
 * @param signal la signal detectada.
 */
static void cierre_forzoso(int signal);

/**
 * @brief Connects the console to a Kernel.
 *
 * @return int
 */
static int on_connect(void);

/**
 * @brief Bison Parser Wrapper. Runs a syntax analysis.
 *
 * @param instructions_file a file path.
 * @return unsigned int
 */
static unsigned int parse(char *instructions_file);

/**
 * @brief Sends a package with a lsit of instructions.
 *
 * @param conexion the connection to send the instructions
 * @param instructions the list
 * @return ssize_t
 */
ssize_t on_send_instructions(void *conexion, t_list *instructions);
// ============================================================================================================
//                                   ***** Funciones Privadas - Definiciones  *****
// ============================================================================================================

static void cierre_forzoso(int signal)
{
	if (signal == SIGINT)
	{
		this.signal = signal;
		LOG_WARNING("SIGNINT was received.");
		on_before_exit();
		exit(SIGINT);
	}
}

static int on_connect(void)
{
	char *port = puerto_kernel();
	char *ip = ip_kernel();

	LOG_WARNING("Created <Console> connection at %s:%s", ip, port);
	this.conexion = conexion_cliente_create(ip, port);

	if (on_module_connect(&this.conexion, false) EQ SUCCESS)
	{
		LOG_DEBUG("Connected to <Kernel> as CLIENT at %s:%s", ip, port);
	}

	return SUCCESS;
}

static unsigned int parse(char *instructions_file)
{
	// Parser return result.
	unsigned int yyresult = 0;

	if (instructions_file)
	{
		// Bison input file.
		yyin = NULL;
		yyin = fopen(instructions_file, "r");

		// When files did open.
		if (yyin)
		{
			switch (yyparse())
			{
			case YYACCEPT:
				LOG_DEBUG("No errors were found");
				yyresult = YYACCEPT;
				break;
			case YYABORT:
				LOG_ERROR("Syntax errors were found. Aborting...");
				yyresult = YYABORT;
				break;
			case YYNOMEM:
				LOG_ERROR("Not enough memory to process");
				yyresult = YYNOMEM;
				break;
			}
		}
		else
		{
			LOG_ERROR("Could not open <'%s'> : No such file or directory", instructions_file);
		}

		fclose(yyin);
	}

	yylex_destroy();

	return yyresult;
}

// ============================================================================================================
//                                   ***** Funciones Públicas  *****
// ============================================================================================================

// ------------------------------------------------------------
//  Life Cycle
// ------------------------------------------------------------

int on_init(void)
{
	if (log_init(MODULE_NAME, true) EQ ERROR)
		return ERROR;

	LOG_DEBUG("Logger started.");

	if (config_init(MODULE_NAME) EQ ERROR)
	{
		LOG_ERROR("Could not open Configuration file.");
		log_close();
		return ERROR;
	}

	LOG_DEBUG("Configurations loaded.");

	LOG_TRACE("Configurations loaded %s", ip());
	// Attach del evento de interrupcion forzada.
	signal(SIGINT, cierre_forzoso);

	input_file_commands = list_smart_create(input_file_commands);

	LOG_DEBUG("Module started SUCCESSFULLY");
	this.status = RUNNING;
	return EXIT_SUCCESS;
}

int on_run(char *instructions_file_name, uint32_t process_size)
{
	LOG_TRACE("Running a Syntax Analysis...");
	unsigned int analysis_result = parse(instructions_file_name);

	if (analysis_result != YYACCEPT)
		this.status = not RUNNING;

	if (yynerrs == 0 && yylexerrs == 0)
	{
		on_connect();

		while (this.status EQ RUNNING)
		{

			if (list_size(input_file_commands) == 0)
			{
				LOG_ERROR("No instructions were found");
				return EXIT_FAILURE;
			}

			LOG_WARNING("Streaming Action...")
			if (on_send_action(this.conexion, SYS, NEW_PROCESS, process_size) > 0)
			{
				LOG_DEBUG("Action sent");
			}

			LOG_WARNING("Streaming Instructions (%d)", list_size(input_file_commands));

			if (!list_is_empty(input_file_commands))
			{
				on_send_instructions(&this.conexion, input_file_commands);
			}

			// while (input_file_commands->elements_count > 0)
			// {
			// 	if (on_send_instruction(
			// 			&this.conexion,
			// 			list_remove(input_file_commands, 0)) > 0)
			// 	{
			// 		LOG_DEBUG("Instruction %d sent.", list_size(input_file_commands));
			// 	}
			// }

			this.status = not RUNNING;
		}
	}

	return analysis_result;
}

int on_before_exit(void)
{
	int exit_code = EXIT_SUCCESS;

	list_smart_fast_destroy(input_file_commands);

	LOG_WARNING("Closing Module...");

	config_close();
	LOG_WARNING("Configurations unloaded.");

	conexion_destroy(&this.conexion);
	LOG_WARNING("Ended connection.");

	LOG_TRACE("Program ended.");
	log_close();

	if (this.signal == SIGINT)
		exit_code = SIGINT;

	return exit_code;
}

// ------------------------------------------------------------
//  Event Handlers
// ------------------------------------------------------------

ssize_t on_send_instruction(void *conexion, instruction_t *inst)
{
	if (inst)
	{
		// Si envio algo sera mayor a 0
		LOG_TRACE("Sending Instruction=[code: %d, param0: %d, param1: %d]...", inst->icode, inst->param0, inst->param1);
		ssize_t result = instruction_send(*(conexion_t *)conexion, inst) > 0 ? SUCCESS : ERROR;

		instruction_destroy(inst);

		return result;
	}

	return (ssize_t)ERROR;
}

ssize_t on_send_instructions(void *conexion, t_list *instructions)
{
	void *stream = instructions_fold(instructions);
	size_t size = list_size(instructions) * sizeof(instruction_t) + sizeof(uint32_t);
	ssize_t ret = conexion_enviar_stream(*(conexion_t *)conexion, CMD, stream, size);
	free(stream);
	return ret;
}

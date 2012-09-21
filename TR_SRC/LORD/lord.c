#include  <stdio.h>

void log_debug(char *frmt)
{
	(void) fprintf(stdout, "\ndebug\t");
	(void) fprintf(stdout, frmt);
}

void log_info(char *frmt)
{
	(void) fprintf(stdout, "\ninfo\t");
	(void) fprintf(stdout, frmt);
}
	
void log_warning(char *frmt)
{
	(void) fprintf(stdout, "\nwarning\t");
	(void) fprintf(stdout, frmt);
}

void log_error(char *frmt)
{
	(void) fprintf(stdout, "\nerror\t");
	(void) fprintf(stdout, frmt);
}

void log_critical(char *frmt)
{
	(void) fprintf(stdout, "\ncritical\t");
	(void) fprintf(stdout, frmt);
}
	
void log_setQuitError(char *frmt)
{
	(void) fprintf(stdout, frmt);
}

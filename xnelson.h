#ifndef XNELSON_H
#define XNELSON_H

#include <stdio.h>
#include <stdlib.h>

#define FATAL_ERROR(str, code) \
	do { \
		fprintf(stderr, "Fatal (%s:%d): %s\n", __FILE__, __LINE__, str); \
		exit(code); \
	} while(0)

int xnelson(void);

#endif /* XNELSON_H */

/*
 * formatter.c
 *
 *  Created on: 23/08/2013
 *      Author: phm
 */
#include <stdio.h>
#include <stdlib.h>
#include "formatter.h"

/*
 * HTML sub-class of formatter class
 */

void html_preamble_(formatter *cthis, char* string){
	char html_start[]= {"<!DOCTYPE html>"};
	char html_header_start[]={"<HEAD>\n<TITLE>\n"};
	char html_header_end[]={"</TITLE>\n</HEAD>\n"};
	char html_body_start[]={"<BODY>\n"};

	fprintf(cthis->fp, "%s", html_start);
	fprintf(cthis->fp, "%s", html_header_start);
	fprintf(cthis->fp, "%s\n", string);
	fprintf(cthis->fp, "%s", html_header_end);
	fprintf(cthis->fp, "%s", html_body_start);
}

void html_postamble_(formatter *cthis, char* string){
	char html_body_end[]={"</BODY>\n"};
	char html_end[]={"</HTML>\n"};
	fprintf(cthis->fp, "%s", html_body_end);
	fprintf(cthis->fp, "%s", html_end);
}

void html_header_(formatter *cthis, char* string){
	fprintf(cthis->fp, "<H1>%s</H1>\n", string);
}

void html_body_(formatter *cthis, char* string){
	fprintf(cthis->fp, "<P>%s</P>\n", string);
}

/*
 * TEXT sub-class of formatter class
 */


 // Place your Text sub class here
 

/*
 * Formatter class Constructor & Destructor
 */

void formatter_init(formatter *cthis, formatter_strategy strategy, char *pathname){

	switch(strategy) {
	case HTML:
		puts("Formatting HTML\n");
		
		break;
	case TEXT:
		puts("Formatting TXT\n");
		puts("......but it hasn't been implemented yet!\n");
		
		break;
	default:
		break;
	}

	cthis->fp=fopen(pathname, "w");
	if(cthis->fp <= 0) {
		printf("Invalid file pointer!\n");
		exit(0);
		}
	html_preamble_(cthis, pathname);
}

void formatter_release(formatter *cthis){
	html_postamble_(cthis, "Thanks Folks!");
	fclose(cthis->fp);
}

/*
 * Formatter class public methods 
 * (Prototypes are placed in formatter.h)
 */

void formatter_header(formatter *cthis, char* string){
	html_header_(cthis, string);
}

void formatter_body(formatter *cthis, char* string){
	html_body_(cthis, string);
}

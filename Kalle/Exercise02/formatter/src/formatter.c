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


void text_preamble_(formatter *cthis, char* string){
	char text_hash[] = {"########"};

	fprintf(cthis->fp, "\n%s", text_hash);
	fprintf(cthis->fp, "%s", string);
	fprintf(cthis->fp, "%s\n\n", text_hash);
}

void text_postamble_(formatter *cthis, char* string){
	char text_hash[] = {"########"};

	fprintf(cthis->fp, "%s", text_hash);
	fprintf(cthis->fp, "%s", string);
	fprintf(cthis->fp, "%s\n\n", text_hash);
}

void text_header_(formatter *cthis, char* string){
	char text_line[] = {"====================================\n"};

	fprintf(cthis->fp, "%s\n", text_line);
	fprintf(cthis->fp, "%s\n\n", string);
	fprintf(cthis->fp, "%s", text_line);
}

void text_body_(formatter *cthis, char* string) {
	char text_line[] = {"------------------------------------\n\n"};

		fprintf(cthis->fp, "%s", text_line);
		fprintf(cthis->fp, "%s\n\n", string);
		fprintf(cthis->fp, "%s", text_line);
}

/*
 * Formatter class Constructor & Destructor
 */

void formatter_init(formatter *cthis, formatter_strategy strategy, char *pathname){


	cthis->fp=fopen(pathname, "w");
	if(cthis->fp <= 0) {
		printf("Invalid file pointer!\n");
		exit(0);
	}


	switch(strategy) {
	case HTML:
		puts("Formatting HTML\n");
		cthis->header = html_header_;
		cthis->body = html_body_;
		cthis->postamble = html_postamble_;
		html_preamble_(cthis, pathname);
		break;
	case TEXT:
		puts("Formatting TXT\n");
		cthis->header = text_header_;
		cthis->body = text_body_;
		cthis->postamble = text_postamble_;
		text_preamble_(cthis, pathname);
		break;
	default:
		break;
	}




}

void formatter_release(formatter *cthis){
	cthis->postamble(cthis, "Thanks Folks!");
	fclose(cthis->fp);
}

/*
 * Formatter class public methods 
 * (Prototypes are placed in formatter.h)
 */

void formatter_header(formatter *cthis, char* string){
	cthis->header(cthis, string);
}

void formatter_body(formatter *cthis, char* string){
	cthis->body(cthis, string);
}

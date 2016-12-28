/*
 ============================================================================
 Name        : text_formatter.c
 Author      : PHM
 Version     :
 Copyright   : Your copyright notice
 Description : Text Formatter C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "formatter.h"

int main(int narg, char *argp[]) {
	char default_header[]="This is my Default Header";
	char default_body[]="This is my default Body";
	char default_filename[]="/home/stud/test.txt";
	formatter_strategy default_format=TEXT;
	char *header=NULL, *body=NULL, *filename=NULL;
	formatter_strategy format=default_format;
	size_t i = 0;

	puts("Running my formatting tool!");
	
	// Use run arguments as text to display
	for (i = 1; i < narg; ++i) {
		if (strcmp(argp[i],"-f") == 0) {
			++i;
			filename = argp[i];
			continue;
		}
		if (strcmp(argp[i],"-t") == 0) {
			++i;
			if (strcmp(argp[i], "HTML") == 0) {
				format = HTML;
			} else {
				format = TEXT;
			}
			continue;
		}
		if (body == NULL) {
			body = argp[i];
			continue;
		}
		if (header == NULL) {
			header = argp[i];
			continue;
		}
		
	}
	if (body == NULL) {
		body = default_body;
	}
	if (header == NULL) {
		header = default_header;
	}
	if (filename == NULL) {
		filename = default_filename;
	}

	// Create formatter object
	formatter my_formatter;

	// Invoke formatter constructor
	formatter_init(&my_formatter, format, filename); // Text file in home dir
//	formatter_init(&my_formatter1, TEXT, "/dev/tty1");            // Current console
//	formatter_init(&my_formatter2, HTML, "/var/www/html/index.html"); // Main html page (use "chmod 666  /var/www/index.html" before running first time)
	
	// Use formatter methods
	formatter_header(&my_formatter, header);

	formatter_body(&my_formatter, body);

	// Invoke formatter destructor
	formatter_release(&my_formatter);

	puts("Done Formatting!!!");
	
	return EXIT_SUCCESS;
}

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
#include "formatter.h"

int main(int narg, char *argp[]) {
	char default_header[]="This is my Default Header";
	char default_body[]="This is my default Body";
	char *header, *body;

	puts("Running my formatting tool!");
	
	// Use run arguments as text to display
	if (narg == 2){         // text_formatter <body text>
		header = default_header;
		body = argp[1];
	}
	else if (narg == 3){    // text_formatter <header text> <body text>
		header = argp[1];
		body = argp[2];
	}
	else {					// text_formatter
		header = default_header;
		body = default_body;
	}

	// Create formatter object
	formatter my_formatter; 

	// Invoke formatter constructor
//	formatter_init(&my_formatter, TEXT, "/home/stud/text.txt"); // Text file in home dir
//	formatter_init(&my_formatter, TEXT, "/dev/tty");            // Current console
	formatter_init(&my_formatter, HTML, "/var/www/html/index.html"); // Main html page (use "chmod 666  /var/www/index.html" before running first time)
	
	// Use formatter methods
	formatter_header(&my_formatter, header);
	formatter_body(&my_formatter, body);

	// Invoke formatter destructor
	formatter_release(&my_formatter);

	puts("Done Formatting!!!");
	
	return EXIT_SUCCESS;
}

/*
CIS 3150:   Assignment 3
Author:     Kyle Lukaszek
ID:         1113798
Due:        November 8th, 2023
*/

/* 
         Parse tree using ASCII graphics
        -original NCurses code from "Game Programming in C with the Ncurses Library"
         https://www.viget.com/articles/game-programming-in-c-with-the-ncurses-library/
         and from "NCURSES Programming HOWTO"
         http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <string.h>

#define MAXREGEXSIZE 1000

// Function prototypes
void print(int depth, char *str);
int drawTree(char *regex);
int regexp(char *regex, int depth);
int concat(char *regex, int depth);
int term(char *regex, int depth);
int star(char *regex, int depth);
int element(char *regex, int depth);
int endofline(char *regex, int depth);
int character(char *regex, int depth);
int group(char *regex, int depth);
int is_symbol_or_white(char c);
char *read_regex_from_file(const char *filename);

// Keeps track of current position in regex string
int position;

// Keeps track of the current width in the parse tree
int width;

// Draws text on the screen at location (depth, width) using ncurses
void print(int depth, char *str)
{
   // offset in depth, used to break line and move it down the screen
   // when the max line length is reached
   static int offset = 0;

   // if the output reaches the max width of the window then
   // move down and back to the left edge (carriage return and newline)
   if (width > 150)
   {
      width = 0;
      offset += 15;
   }

   // ncurses command to draw textstr
   mvprintw(depth + offset, width, str);

   // update the width of the current row to the width we just drew so we can update the position of the neighbor node
   width = width;
}

// Draws the parse tree using ncurses (provided in assignment)
int drawTree(char *regex)
{
   char c;
   int depth = 0;
   // ncurses clear screen
   clear();
   // parse tree
   // parsing functions calls print to draw output
   // -regex is the string containing the regular expression to be parsed
   // -depth contains the current depth in the parse tree, it is incremented with each recursive call
   regexp(regex, depth);
   refresh();
   // read keyboard and exit if 'q' pressed
   while (1)
   {
      c = getch();
      if (c == 'q')
         return (1);
   }
}

// Start of regex recursive descent parser
int regexp(char *regex, int depth)
{
   print(depth, "regexp");

   return (concat(regex, depth + 1));
}

// Check if a term is followed by a term or the end of line
int concat(char *regex, int depth)
{
   print(depth, "concat");

   // Save the original position of the regex pointer when entering the function
   int original_position = position;

   // Check for the presence of a term
   if (term(regex, depth + 1))
   {
      // Recursively call the concat function for the next term
      if (!concat(regex, depth + 1))
      {
         // Reset the regex position if a concat match was not found
         position = original_position;
         if (term(regex, depth + 1))
         {
            return 1;
         }
         else
         {
            // Reset the regex position if a match is not found
            position = original_position;
            return 0;
         }
      }
      else
      {
         return 1;
      }
   }
   // Check for the presence of a term
   else if (term(regex, depth + 1))
   {
      return 1;
   }
   // Check for the end of line
   else if (endofline(regex, depth + 1))
   {
      return 1;
   }
   else
   {
      // Reset the regex position if a match is not found
      position = original_position;
      return 0;
   }
}

// Check if a term is followed by a star symbol, or if it is an element
int term(char *regex, int depth)
{
   print(depth, "term");

   // Save the original position of the regex pointer when entering the function
   int original_position = position;

   // Check if a star match was found
   if (star(regex, depth + 1))
   {
      return 1;
   }
   // If a star match was not found, check if an element match was found
   else
   {
      // Reset the regex position if a star match was not found
      position = original_position;

      // Return the result of the element match
      return (element(regex, depth + 1));
   }
}

// Check if a term is followed by a star symbol
int star(char *regex, int depth)
{
   print(depth, "star");

   if (element(regex, depth + 1))
   {
      // Check if the star symbol is present
      if (regex[position] == '*')
      {
         // Increment the position marker
         position++;
         return 1;
      }
   }

   return 0;

   // Check for the presence of an element followed by a star symbol, and then increment the position marker if the previous conditions are met. 
   // Return 1 if the conditions are met, otherwise return 0.
   //return (element(regex, depth + 1) && (regex[position] == '*') && (position++));
}

// Check if the element is a character or group
int element(char *regex, int depth)
{
   print(depth, "element");

   if (group(regex, depth + 1))
   {
      return 1;
   }
   else if (character(regex, depth + 1))
   {
      return 1;
   }

   return 0;

   // Check if the element is a group, or if it is a character
   // Return 1 if the element is a group or character, otherwise return 0
   //return (group(regex, depth + 1) || character(regex, depth + 1));
}

// Check if the element is a character
int character(char *regex, int depth)
{
   print(depth, "char");

   // Check if the character is a letter
   if (isalnum(regex[position]) || is_symbol_or_white(regex[position]))
   {
      // Print match
      print(depth + 1, "match");

      // increase width
      width += 10;

      // Increment the position marker
      position++;
      return 1;
   }

   // Print failure if the character is not not alnum or symbol
   print(depth + 1, "fail");

   // increase width
   width += 10;

   return 0;
}

// Check if the element is a group
int group(char *regex, int depth)
{
   print(depth, "group");

   // Check if the group is enclosed in parentheses
   if (regex[position] == '(')
   {
      // Increment the position marker
      position++;

      // Continue with the next concatenation
      if (regexp(regex, depth + 1))
      {
         // Check if the group is closed with a closing parenthesis
         if (regex[position] == ')')
         {
            // Increment the position marker
            position++;
            return 1;
         }
      }
   }

   // Print failure if any of the above conditions are not met
   print(depth + 1, "fail");

   // Increase width
   width += 10;

   return 0;
}

// Check if the end of the line has been reached
int endofline(char *regex, int depth)
{
   print(depth, "eoln");

   // Check if the end of the line has been reached
   if (regex[position] == '\0')
   {
      // Print match
      print(depth + 1, "match");

      // Increase width
      width += 10;

      return 1;
   }

   // Print failure
   print(depth + 1, "fail");

   // Increase width
   width += 10;

   return 0;
}

// Check if the character is a symbol or whitespace/escape character
int is_symbol_or_white(char c)
{
   // Array of symbols to check against from assignment outline, including whitespace, metacharacters, and escape characters
   char symbols[] = {'!', '"', '#', '$', '%', '&', '\'', '+', ',', '-', '.', '/', 
                     ':', ';', '<', '=', '>', '?', '@', '[', ']', '^',
                     '_', '`', '{', '}', '~',     // ASCII symbols
                     ' ',                         // Whitespace
                     '\\', '|',                   // Metacharacters
                     '\t', '\v', '\n'};           // Whitespace escape characters
   
   // Iterate through the array of symbols and check if the character is a symbol
   for (int i = 0; i < sizeof(symbols) / sizeof(symbols[0]); i++) {
      if (c == symbols[i]) {
         return 1;
      }
   }

   // Return 0 if the character is not a symbol or whitespace
   return 0;
}

// Read the regex from the file
char *read_regex_from_file(const char *filename)
{
   // Check if filename is NULL
   if (filename == NULL)
   {
      return NULL;
   }

   // Open the file
   FILE *file = fopen(filename, "r");

   // Check if fopen failed
   if (file == NULL)
   {
      return NULL;
   }

   // Allocate memory for the regex
   char *regex = malloc(MAXREGEXSIZE * sizeof(char) + 1);

   // Check if malloc failed
   if (regex == NULL)
   {
      return NULL;
   }

   int c;
   int n = 0;

   // Read the regex from the file
   while ((c = fgetc(file)) != EOF && n < MAXREGEXSIZE)
   {
      regex[n++] = c;
   }
   // Close the file
   fclose(file);

   // Add a null terminator to the end of the regex
   regex[n] = '\0';

   return regex;
}

// Main function
int main(int argc, char *argv[])
{
   // Check if the number of arguments is correct
   if (argc != 2)
   {
      printf("Usage: ./a3 <filename>\n");
      return 1;
   }

   const char *filename = argv[1];

   // Read the regex from the file
   char *regex = read_regex_from_file(filename);

   // Check if the function returns NULL
   if (regex == NULL)
   {
      printf("Error: Could not read regex from file\n");
      return 1;
   }

   // Set initial width and regex position to 0 for the drawTree function
   width = 0;
   position = 0;

   // initialize ncurses
   initscr();
   noecho();
   cbreak();
   timeout(0);
   curs_set(FALSE);

   // Draw the parse tree
   drawTree(regex);

   // shut down ncurses
   endwin();

   // Free the regex memory
   free(regex);
}

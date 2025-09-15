#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Scanner
{
  FILE* __source;
  char __current_char;
  bool hadError;
}
Scanner;

Scanner NewScanner(FILE* source) {
  Scanner newScanner = (Scanner) {
    .__source = source,
    .hadError = false
  };

  return newScanner;
}

void ScannerClose(Scanner* scanner) {
  int c;
  while (
    (c = fgetc(scanner->__source)) != '\n' &&
    c != EOF
  );
}

static void __Scanner_Error__(Scanner* scanner, const char* errorMessage) {
  ScannerClose(scanner);
  fprintf(stderr, "[ERROR] Scanner: %s\n", errorMessage);
  scanner->hadError = true;
}

static bool __Scanner_Advance__(Scanner* scanner) {
  int c; // Must use int to handle EOF
  if ((c = fgetc(scanner->__source)) != EOF)
  {
    if (0 <= c && c <= 255)
    {
      scanner->__current_char = (char) c;
      return true;
    }

    __Scanner_Error__(scanner, "Unknown character!");
    return false;
  }

  if (ferror(scanner->__source))
    {__Scanner_Error__(scanner, "I/O error when reading!");}
  return false;
}

static inline bool
__Scanner_Is_White_Space__(const char checkChar) {
  return (
    checkChar == '\n' ||
    checkChar == '\t' ||
    checkChar == ' '
  );
}

static void __Scanner_Skip_White_Space__(Scanner* scanner) {
  while (
    __Scanner_Advance__(scanner) &&
    __Scanner_Is_White_Space__(scanner->__current_char)
  );
}

bool ScannerNext(Scanner* scanner, char* stringInput, const uint64_t length) {
  __Scanner_Skip_White_Space__(scanner);

  for (size_t charIndex=0; charIndex < length-1; charIndex++) {
    stringInput[charIndex] = scanner->__current_char;

    if (
      __Scanner_Is_White_Space__(scanner->__current_char) ||
      !(__Scanner_Advance__(scanner))
    ) {
      if (!(scanner->hadError))
        {stringInput[charIndex] = '\0';}

      return !(scanner->hadError);
    }
  }

  __Scanner_Error__(scanner, "Input string is out of length!");
  return false;
}

bool ScannerNextQuote(Scanner* scanner, char* stringInput, const uint64_t length) {
  __Scanner_Skip_White_Space__(scanner);
  
  if (scanner->__current_char != '"')
  {
    __Scanner_Error__(scanner, "Expected string in quotation marks!");
    return false;
  }

  stringInput[0] = '"';
  for (size_t charIndex=1; charIndex < length-1; charIndex++) {
    if (!(__Scanner_Advance__(scanner)))
    {
      if (!(scanner->hadError))
        {__Scanner_Error__(scanner, "Expected \" at the end!");}
      return false;
    }

    stringInput[charIndex] = scanner->__current_char;

    if (scanner->__current_char == '"')
    {
      stringInput[charIndex+1] = '\0';
      return true;
    }
  }

  __Scanner_Error__(scanner, "Input string is out of length!");
  return false;
}

int main() {
  char inputStr[50];
  Scanner testScanner = NewScanner(stdin);

  printf("Type: ");
  ScannerNext(&testScanner, inputStr, 50);
  printf("You've type: %s\n", inputStr);
  printf("Type string in quotation marks: ");
  ScannerNextQuote(&testScanner, inputStr, 50);
  printf("You've type: %s\n", inputStr);
  ScannerClose(&testScanner);

  return 0;
}

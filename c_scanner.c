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

static void __Scanner_Error__(Scanner* scanner, const char* errorMessage) {
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

  stringInput[length-1] = '\0';
  return !(scanner->hadError);
}

void ScannerClose(Scanner* scanner) {
  int c;
  while (
    (c = fgetc(scanner->__source)) != '\n' &&
    c != EOF
  );
}

Scanner NewScanner(FILE* source) {
  Scanner newScanner = (Scanner) {
    .__source = source,
    .hadError = false
  };

  return newScanner;
}

int main() {
  char inputStr[5];
  Scanner testScanner = NewScanner(stdin);

  printf("Type: ");
  ScannerNext(&testScanner, inputStr, 5);
  printf("You've type: %s\n", inputStr);
  ScannerClose(&testScanner);

  return 0;
}

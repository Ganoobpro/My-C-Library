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

static bool __Scanner_Advance_EOF__(Scanner* scanner, const char* errorMessage) {
  if (!(__Scanner_Advance__(scanner)))
  {
    if (!(scanner->hadError))
      {__Scanner_Error__(scanner, errorMessage);}
    return false;
  }

  return true;
}

static inline bool
__Scanner_Is_White_Space__(const char checkChar) {
  return (
    checkChar == '\n' ||
    checkChar == '\t' ||
    checkChar == ' '
  );
}

static inline bool
__Scanner_Is_Number__(const char checkChar) {
  return ('0' <= checkChar && checkChar <= '9');
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

  if (scanner->__current_char == '\n')
    {return true;}

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
    if (!(__Scanner_Advance_EOF__(scanner, "Expected \" at the end!")))
      {return false;}

    stringInput[charIndex] = scanner->__current_char;

    if (scanner->__current_char == '"')
    {
      stringInput[charIndex+1] = '\0';
      return true;
    }
  }

  if (scanner->__current_char == '\n')
    {return true;}

  __Scanner_Error__(scanner, "Input string is out of length!");
  return false;
}

int64_t ScannerNextNumber(Scanner* scanner) {
  int64_t answer = 0, numberSign = 1;
  __Scanner_Skip_White_Space__(scanner);

  if (scanner->__current_char == '-') {
    numberSign = -1;

    if (!(__Scanner_Is_Number__(scanner->__current_char)))
    {
      __Scanner_Error__(scanner, "Expected number after '-'!");
      return answer;
    }

    if (!(__Scanner_Advance_EOF__(scanner, "Expected number after '-'!")))
      {return answer;}
  }

  while (
    __Scanner_Is_Number__(scanner->__current_char) &&
    (!(__Scanner_Is_White_Space__(scanner->__current_char)))
  ) {
    int64_t digit = (int64_t)(scanner->__current_char - '0');

    if (
      INT64_MIN <= answer * 10 + digit * numberSign &&
      INT64_MAX >= answer * 10 + digit * numberSign
    ) {answer = answer * 10 + digit * numberSign;}
    else
    {
      __Scanner_Error__(scanner, "Number input is out of range!");
      return answer;
    }

    if (!(__Scanner_Advance__(scanner)))
      {break;}
  }

  if (!(
    scanner->hadError ||
    __Scanner_Is_Number__(scanner->__current_char)
  ))
  {__Scanner_Error__(scanner, "Unknown char in number input!");}

  return answer;
}

int main() {
  char inputStr[5];
  Scanner testScanner = NewScanner(stdin);

  printf("Type: ");
  ScannerNext(&testScanner, inputStr, 5);
  printf("You've type: %s\n", inputStr);
  printf("Type a number: ");
  
  printf("You've type: %ld\n",
         ScannerNextNumber(&testScanner));

  if (!(testScanner.hadError))
    {ScannerClose(&testScanner);}

  return 0;
}

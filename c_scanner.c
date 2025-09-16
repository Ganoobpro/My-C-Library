#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>

typedef struct Scanner
{
  FILE* __source;
  char __current_char;
  bool hadError;
}
Scanner;



static void __Scanner_Error__(Scanner* scanner, const char* errorMessage);
static bool __Scanner_Advance__(Scanner* scanner);
static bool __Scanner_Advance_EOF__(Scanner* scanner, const char* errorMessage);

static inline bool __Scanner_Is_White_Space__(const char checkChar);
static inline bool __Scanner_Is_Number__(const char checkChar);
static void __Scanner_Skip_White_Space__(Scanner* scanner);

static long double __Scanner_Number__(Scanner* scanner, bool scanFloat);
static long double __Scanner_Get_Sign__(Scanner* scanner);



Scanner NewScanner(FILE* source) {
  Scanner newScanner = (Scanner) {
    .__source = source,
    .hadError = false
  };

  return newScanner;
}

void ScannerClean(Scanner* scanner) {
  if (scanner->hadError || __Scanner_Is_White_Space__(scanner->__current_char)) return;

  int c;
  while (
    (c = fgetc(scanner->__source)) != '\n' &&
    c != EOF
  );
}

void ScannerNext(Scanner* scanner, char* stringInput, const uint64_t length) {
  __Scanner_Skip_White_Space__(scanner);

  for (size_t charIndex=0; charIndex < length-1; charIndex++) {
    stringInput[charIndex] = scanner->__current_char;

    if (!(__Scanner_Advance__(scanner)))
    {
      stringInput[charIndex+1] = '\0';
      return !(scanner->hadError);
    }

    if (__Scanner_Is_White_Space__(scanner->__current_char))
    {
      stringInput[charIndex+1] = '\0';
      return true;
    }
  }

  if (__Scanner_Is_White_Space__(scanner->__current_char))
    {return true;}

  __Scanner_Error__(scanner, "Input string is out of length!");
  return false;
}

void ScannerNextQuote(Scanner* scanner, char* stringInput, const uint64_t length) {
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

int64_t ScannerNextNumber(Scanner* scanner,
                          const int64_t scannerMin,
                          const int64_t scannerMax)
{
  __Scanner_Skip_White_Space__(scanner);

  int64_t sign = __Scanner_Get_Sign__(scanner);
  if (sign == 0) return 0;

  long double numberInput = __Scanner_Number__(scanner, false);
  if (scanner->hadError) return 0;
  if (numberInput * sign < scannerMin ||
      numberInput * sign > scannerMax)
  {
    __Scanner_Error__(scanner, "Expected number in the range!");
    return 0;
  }

  return (int64_t) (numberInput * sign);
}

uint64_t ScannerNextPositive(Scanner* scanner,
                             const uint64_t scannerMax)
{
  __Scanner_Skip_White_Space__(scanner);

  long double numberInput = __Scanner_Number__(scanner, false);
  if (scanner->hadError) return 0;
  if (numberInput < 0 ||
      numberInput > scannerMax)
  {
    __Scanner_Error__(scanner, "Expected number in the range!");
    return 0;
  }

  return (int64_t) numberInput;
}

long double ScannerNextFloat(Scanner* scanner,
                             const long double scannerMin,
                             const long double scannerMax)
{
  __Scanner_Skip_White_Space__(scanner);

  long double sign = __Scanner_Get_Sign__(scanner);
  if (sign == 0) return 0;

  long double numberInput = __Scanner_Number__(scanner, true);
  if (scanner->hadError) return 0;
  if (numberInput * sign < scannerMin ||
      numberInput * sign > scannerMax)
  {
    __Scanner_Error__(scanner, "Expected number in the range!");
    return 0;
  }

  return numberInput * sign;
}



static void __Scanner_Error__(Scanner* scanner, const char* errorMessage) {
  ScannerClean(scanner);
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

static long double
__Scanner_Number__(Scanner* scanner, bool scanFloat)
{
  long double answer = 0, digit,
              decimalDigit = 10,
              floatDigit = 1;

  do {
    if (scanFloat &&
        scanner->__current_char == '.' &&
        floatDigit == 1)
    {
      decimalDigit = 1;
      floatDigit = 10;
    }
    else if (scanner->__current_char < '0' || scanner->__current_char > '9')
    {
      __Scanner_Error__(scanner, "Unknown char in number!");
      return answer;
    }
    else
    {
      digit = scanner->__current_char - '0';
      answer = answer * decimalDigit + digit / floatDigit;

      if (floatDigit != 1)
        {floatDigit *= 10;}
    }
  }
  while (__Scanner_Advance__(scanner) &&
        (!(__Scanner_Is_White_Space__(scanner->__current_char))));

  return answer;
}

static long double
__Scanner_Get_Sign__(Scanner* scanner)
{
  if (scanner->__current_char == '-')
  {
    if (!(__Scanner_Advance__(scanner)))
      {return 0;}
    else if ('0' > scanner->__current_char || scanner->__current_char > '9')
    {
      __Scanner_Error__(scanner, "Expected number after '-'!");
      return 0;
    }

    return -1;
  }

  return 1;
}

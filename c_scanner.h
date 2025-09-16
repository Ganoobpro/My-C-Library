#ifndef C_SCANNER_H
#define C_SCANNER_H

#include "./c_scanner.c"

#define ScannerNextNumber8(scannerPtr) \
  (int8_t)  ScannerNextNumber(scannerPtr, INT8_MIN, INT8_MAX)
#define ScannerNextNumber16(scannerPtr) \
  (int16_t) ScannerNextNumber(scannerPtr, INT16_MIN, INT16_MAX)
#define ScannerNextNumber32(scannerPtr) \
  (int32_t) ScannerNextNumber(scannerPtr, INT32_MIN, INT32_MAX)
#define ScannerNextNumber64(scannerPtr) \
  ScannerNextNumber(scannerPtr, INT64_MIN, INT64_MAX)

#define ScannerNextPositive8(scannerPtr) \
  (uint8_t)  ScannerNextPositive(scannerPtr, UINT8_MAX)
#define ScannerNextPositive16(scannerPtr) \
  (uint16_t) ScannerNextPositive(scannerPtr, UINT16_MAX)
#define ScannerNextPositive32(scannerPtr) \
  (uint32_t) ScannerNextPositive(scannerPtr, UINT32_MAX)
#define ScannerNextPositive64(scannerPtr) \
  ScannerNextPositive(scannerPtr, UINT64_MAX)


#define ScannerNextDouble(scannerPtr) \
  (double) ScannerNextFloat(scannerPtr, -DBL_MAX, DBL_MAX);
#define ScannerNextLongDouble(scannerPtr) \
  ScannerNextFloat(scannerPtr, -LDBL_MAX, LDBL_MAX);
#define ScannerNextLD(scannerPtr) \
  ScannerNextFloat(scannerPtr, -LDBL_MAX, LDBL_MAX);

#endif

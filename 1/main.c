#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum {
  kBufferSize = 6,              // The MAX length of a number, in digits, plus newline
  kNumTopSaved = 3,             // Top number of calories to track and sum
};

/*
 * This program makes the following assumptions:
 * 1. Each line contains either a newline, a 4-digit number, or a 5-digit number
 * 2. All data is clean
 * 3. Data is not sorted
 */
int main()
{
  // We know the file is there, no need to NULL check
  FILE *pstFile = fopen("input.txt", "r");

  // Create a buffer that is twice the length of the largest expected number.
  // We can use this as A/B to read data in 
  uint8_t aBuffer[ kBufferSize ];

  uint32_t auMaxCalories[ kNumTopSaved ] = { 0 };
  uint32_t uCurrentCalories = 0;

  while ( true )
  {
      // CASE 1:  There is a newline
      int numBytesRead = fread( aBuffer, 1, 1, pstFile );
      if ( numBytesRead != 1 )
      {
        break;
      }

      if ( aBuffer[ 0 ] == ( uint8_t ) '\n' )
      {
        // We're done counting for this one
        if ( uCurrentCalories > auMaxCalories[ kNumTopSaved - 1 ] )
        {
          // The Current calories is > than the kNumTopSaved - 1 amount, so kick it out and re-order
          auMaxCalories[ kNumTopSaved - 1 ] = uCurrentCalories;

          // Perform a sort based on the new insertion
          for ( uint8_t i = kNumTopSaved - 1; i > 0; i-- )
          {
            if ( auMaxCalories[ i ] > auMaxCalories[ i - 1 ] )
            {
              uint32_t temp = auMaxCalories[ i - 1 ];
              auMaxCalories[ i - 1 ]  = auMaxCalories[ i ];
              auMaxCalories[ i ] = temp;
            }
          }
        }
        uCurrentCalories = 0;
        continue;
      }

      // CASE 2: There is a 4-digit number.  Read 4 more bytes and look for a newline on the 5th byte
      numBytesRead = fread( &aBuffer[ 1 ], 1, 4, pstFile );
      if ( numBytesRead != 4 )
      {
        break;
      }

      if ( aBuffer[ 4 ] != ( uint8_t ) '\n' )
      {
        // Must be a 5-digit char, but we can skip the new-line
        // We don't care if the seek fails, since the next read will also fail
        ( void ) fseek( pstFile, 1, SEEK_CUR );
      }
      else
      {
        // atol expects a null-terminated string, enforce that here
        aBuffer [ 4 ] = 0;
      }
      long numCalories = atol( ( const char* ) aBuffer );
      uCurrentCalories += numCalories;
  }

  // Calculate the total
  uint32_t uMax = 0;
  for ( uint8_t i = 0; i < kNumTopSaved; i++ )
  {
    uMax += auMaxCalories[ i ];
  }
  printf( "Max Calories: %d\r\n", uMax );
  return 0;
}

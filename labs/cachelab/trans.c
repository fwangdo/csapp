/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include "cachelab.h"
#include <stdio.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit_32(int M, int N, int A[N][M], int B[M][N]) {
  int t0, t1, t2, t3, t4, t5, t6, t7;
  for (int i = 0; i < N; i += 8) {
    for (int j = 0; j < M; j += 8) {
      for (int di = 0; di < 8; di++) {
        for (int dj = 0; dj < 8; dj++) {
          if (di == dj) {
            if (di == 0) {
              t0 = A[i + di][j + dj];
            } else if (di == 1) {
              t1 = A[i + di][j + dj];
            } else if (di == 2) {
              t2 = A[i + di][j + dj];
            } else if (di == 3) {
              t3 = A[i + di][j + dj];
            } else if (di == 4) {
              t4 = A[i + di][j + dj];
            } else if (di == 5) {
              t5 = A[i + di][j + dj];
            } else if (di == 6) {
              t6 = A[i + di][j + dj];
            } else if (di == 7) {
              t7 = A[i + di][j + dj];
            }
          } else {
            B[j + dj][i + di] = A[i + di][j + dj];
          }
        }
      }

      B[j][i] = t0;
      B[j + 1][i + 1] = t1;
      B[j + 2][i + 2] = t2;
      B[j + 3][i + 3] = t3;
      B[j + 4][i + 4] = t4;
      B[j + 5][i + 5] = t5;
      B[j + 6][i + 6] = t6;
      B[j + 7][i + 7] = t7;
      // for (int di = 0; di < 8; di++) {
      // B[j + di][i + di] = A[i + di][j + di];
      // }
    }
  }
}

void transpose_submit_64(int M, int N, int A[N][M], int B[M][N]) {
  for (int j = 0; j < M; j += 8) {
    for (int i = 0; i < N - 8; i += 8) {
      //
      if (i == j) {
        continue;
      }

      for (int di = 0; di < 4; di++) {
        for (int dj = 0; dj < 4; dj++) {
          B[j + dj][i + di] = A[i + di][j + dj];
        }
      }

      for (int di = 0; di < 4; di++) {
        for (int dj = 4; dj < 8; dj++) {
          B[j + dj - 4][i + di + 8] = A[i + di][j + dj];
        }
      }

      for (int di = 4; di < 8; di++) {
        for (int dj = 0; dj < 4; dj++) {
          B[j + dj][i + di] = A[i + di][j + dj];
        }
      }

      for (int di = 4; di < 8; di++) {
        for (int dj = 4; dj < 8; dj++) {
          B[j + dj][i + di] = A[i + di][j + dj];
        }
      }

      for (int di = 0; di < 4; di++) {
        for (int dj = 4; dj < 8; dj++) {
          // B[j + dj - 4][i + di + 8] = B[j + dj][i + di];
          B[j + dj][i + di] = B[j + dj - 4][i + di + 8];
        }
      }
    }
  }

  for (int j = 0; j < M; j += 8) {
    for (int i = 56; i < N; i += 8) {
      if (i == j) {
        continue;
      }

      for (int di = 0; di < 4; di++) {
        for (int dj = 0; dj < 4; dj++) {
          B[j + dj][i + di] = A[i + di][j + dj];
        }
      }

      for (int di = 0; di < 4; di++) {
        for (int dj = 4; dj < 8; dj++) {
          B[j + dj][i + di] = A[i + di][j + dj];
        }
      }

      for (int di = 4; di < 8; di++) {
        for (int dj = 4; dj < 8; dj++) {
          B[j + dj][i + di] = A[i + di][j + dj];
        }
      }

      for (int di = 4; di < 8; di++) {
        for (int dj = 0; dj < 4; dj++) {
          B[j + dj][i + di] = A[i + di][j + dj];
        }
      }
    }
  }

  for (int i = 0; i < N; i += 16) {
    for (int j = 0; j < M; j += 16) {
      if (i != j) {
        continue;
      }

      // 1.
      for (int di = 0; di < 4; di++) {
        for (int dj = 0; dj < 4; dj++) {
          B[j + dj + 8][i + di + 8] = A[i + di][j + dj];
        }
      }

      for (int di = 0; di < 4; di++) {
        for (int dj = 4; dj < 8; dj++) {
          B[j + dj + 8][i + di + 8] = A[i + di][j + dj];
        }
      }

      for (int di = 4; di < 8; di++) {
        for (int dj = 4; dj < 8; dj++) {
          B[j + dj + 8][i + di + 8] = A[i + di][j + dj];
        }
      }

      for (int di = 4; di < 8; di++) {
        for (int dj = 0; dj < 4; dj++) {
          B[j + dj + 8][i + di + 8] = A[i + di][j + dj];
        }
      }

      // 2.
      for (int di = 0; di < 4; di++) {
        for (int dj = 0; dj < 4; dj++) {
          B[j + dj][i + di] = A[i + di + 8][j + dj + 8];
        }
      }

      for (int di = 0; di < 4; di++) {
        for (int dj = 4; dj < 8; dj++) {
          B[j + dj][i + di] = A[i + di + 8][j + dj + 8];
        }
      }

      for (int di = 4; di < 8; di++) {
        for (int dj = 4; dj < 8; dj++) {
          B[j + dj][i + di] = A[i + di + 8][j + dj + 8];
        }
      }

      for (int di = 4; di < 8; di++) {
        for (int dj = 0; dj < 4; dj++) {
          B[j + dj][i + di] = A[i + di + 8][j + dj + 8];
        }
      }

      // 3. change.
      int tmp;
      for (int di = 0; di < 8; di++) {
        for (int dj = 0; dj < 8; dj++) {
          tmp = B[i + di][j + dj];
          B[i + di][j + dj] = B[i + di + 8][j + dj + 8];
          B[i + di + 8][j + dj + 8] = tmp;
        }
      }
    }
  }
}

void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
  // N -> 67, M = 61.
  // 64, 61
  for (int i = 0; i < N - 8; i += 8) {
    for (int j = 0; j < M - 8; j += 8) {
      for (int di = 0; di < 8; di++) {
        for (int dj = 0; dj < 8; dj++) {
          B[j + dj][i + di] = A[i + di][j + dj];
        }
      }
    }

    for (int j = 56; j < M; j++) {
      B[j][i] = A[i][j];
      B[j][i + 1] = A[i + 1][j];
      B[j][i + 2] = A[i + 2][j];
      B[j][i + 3] = A[i + 3][j];
      B[j][i + 4] = A[i + 4][j];
      B[j][i + 5] = A[i + 5][j];
      B[j][i + 6] = A[i + 6][j];
      B[j][i + 7] = A[i + 7][j];
    }
  }

  // 67, 61
  for (int j = 0; j < 61; j++) {
    B[j][64] = A[64][j];
    B[j][65] = A[65][j];
    B[j][66] = A[66][j];
  }
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the
 * cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
  for (int i = 0; i < N - 8; i += 8) {
    for (int j = 0; j < M - 8; j += 8) {
      for (int di = 0; di < 8; di++) {
        for (int dj = 0; dj < 8; dj++) {
          B[j + dj][i + di] = A[i + di][j + dj];
        }
      }
    }
  }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
  /* Register your solution function */
  registerTransFunction(transpose_submit, transpose_submit_desc);

  /* Register any additional transpose functions */
  registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
  int i, j;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; ++j) {
      if (A[i][j] != B[j][i]) {
        return 0;
      }
    }
  }
  return 1;
}

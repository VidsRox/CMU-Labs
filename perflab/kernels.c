/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "Star Platinum",              /* Team name */

    "Vidyun Agarwal",     /* First member full name */
    "vidyunagarwal@gmail.com",  /* First member email address */

    "",                   /* Second member full name (leave blank if none) */
    ""                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

/* 
 * rotate_2x - 2x unrolling
 */
char rotate_2x_descr[] = "rotate_2x: implementation for 2x unrolling";
void rotate_2x(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++) {
	for (j = 0; j < dim; j+=2){
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
        dst[RIDX(dim-1-(j+1), i, dim)] = src[RIDX(i, j+1, dim)];
    }
  }
}

/* 
 * rotate_4x - 4x unrolling
 */
char rotate_4x_descr[] = "rotate_4x: implementation for 4x unrolling";
void rotate_4x(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++) {
	for (j = 0; j < dim; j+=4){
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
        dst[RIDX(dim-1-(j+1), i, dim)] = src[RIDX(i, j+1, dim)];
        dst[RIDX(dim-1-(j+2), i, dim)] = src[RIDX(i, j+2, dim)];
        dst[RIDX(dim-1-(j+3), i, dim)] = src[RIDX(i, j+3, dim)];
    }
  }
}

/* 
 * rotate_8x - 8x unrolling
 */
char rotate_8x_descr[] = "rotate_8x: implementation for 8x unrolling";
void rotate_8x(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++) {
	for (j = 0; j < dim; j+=8){
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
        dst[RIDX(dim-1-(j+1), i, dim)] = src[RIDX(i, j+1, dim)];
        dst[RIDX(dim-1-(j+2), i, dim)] = src[RIDX(i, j+2, dim)];
        dst[RIDX(dim-1-(j+3), i, dim)] = src[RIDX(i, j+3, dim)];
        dst[RIDX(dim-1-(j+4), i, dim)] = src[RIDX(i, j+4, dim)];
        dst[RIDX(dim-1-(j+5), i, dim)] = src[RIDX(i, j+5, dim)];
        dst[RIDX(dim-1-(j+6), i, dim)] = src[RIDX(i, j+6, dim)];
        dst[RIDX(dim-1-(j+7), i, dim)] = src[RIDX(i, j+7, dim)];
    }
  }
}

/*
    rotate_blocked - implementing blocking mechanism
*/
char rotate_blocked_descr[] = "rotate_blocked: implementation for blocked rotate function";
void rotate_blocked(int dim, pixel *src, pixel *dst){
    int bi, bj, i, j;
    int block = 8;
    for (bi = 0; bi<dim; bi+=block){
     for(bj = 0; bj<dim; bj+=block){
        for(i = bi; i< bi + block; i++){
            for(j = bj; j < bj + block; j++){
                dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
            }
        }
     }
    }
}

char rotate_blocked2_descr[] = "rotate_blocked: implementation for blocked rotate function w strength reduction and restrict experiment";
void rotate_blocked2(int dim, pixel *__restrict__ src, pixel *__restrict__ dst){
    int bi, bj, i, j;
    int block = 8;
    for (bi = 0; bi<dim; bi+=block){
     for(bj = 0; bj<dim; bj+=block){
        for(i = bi; i< bi + block; i++){

            // compute i * dim exactly once, store it as a pointer offset
            pixel *src_row = src + i * dim;

            for(j = bj; j < bj + block; j++){
                // now src_row[j] is just src_row + j — an addition, not a multiply
                dst[RIDX(dim-1-j, i, dim)] = src_row[j];
            }
        }
     }
    }
}



/*
    rotate_blocked3 - implementing blocking mechanism
*/
char rotate_blocked3_descr[] = "rotate_blocked3: implementation for blocked rotate function w size 32";
void rotate_blocked_3(int dim, pixel *src, pixel *dst){
    int bi, bj, i, j;
    int block = 32;
    for (bi = 0; bi<dim; bi+=block){
     for(bj = 0; bj<dim; bj+=block){
        for(i = bi; i< bi + block; i++){
            for(j = bj; j < bj + block; j++){
                dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
            }
        }
     }
    }
}


/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) 
{
    rotate_blocked(dim, src, dst);
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&rotate_2x, rotate_2x_descr);
    add_rotate_function(&rotate_4x, rotate_4x_descr);
    add_rotate_function(&rotate_8x, rotate_8x_descr);
    add_rotate_function(&rotate, rotate_descr);   
    add_rotate_function(&rotate_blocked,  rotate_blocked_descr);
    add_rotate_function(&rotate_blocked2, rotate_blocked2_descr);
    /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*

*/
char smooth_optimized_descr[] = "Optimized smooth: handles the four corners first, then loop through the four edges, then loop through the interior.";
void smooth_optimized(int dim, pixel *src, pixel *dst){
    // Four corners - just hardcoded assignments, no loops
    dst[RIDX(0,0,dim)].red = (src[RIDX(0,0,dim)].red + src[RIDX(0,1,dim)].red 
                        + src[RIDX(1,0,dim)].red + src[RIDX(1,1,dim)].red) / 4;

    dst[RIDX(0,dim-1,dim)].red = ( src[RIDX(0, dim-1, dim)].red + src[RIDX(0, dim-2, dim)].red 
                            + src[RIDX(1, dim-1, dim)].red + src[RIDX(1, dim-2, dim)].red ) / 4;

    dst[RIDX(dim-1, 0, dim)].red = (src[RIDX(dim-1,0,dim)].red + src[RIDX(dim-2, 0, dim)].red
                              + src[RIDX(dim-1, 1, dim)].red + src[RIDX(dim-2, 1, dim)].red) / 4;

    dst[RIDX(dim-1, dim-1, dim)].red = (src[RIDX(dim-1,dim-1,dim)].red + src[RIDX(dim-2, dim-1, dim)].red
                                + src[RIDX(dim-1, dim-2, dim)].red + src[RIDX(dim-2, dim-2, dim)].red) / 4;

    dst[RIDX(0,0,dim)].blue = (src[RIDX(0,0,dim)].blue + src[RIDX(0,1,dim)].blue 
                        + src[RIDX(1,0,dim)].blue + src[RIDX(1,1,dim)].blue) / 4;

    dst[RIDX(0,dim-1,dim)].blue = ( src[RIDX(0, dim-1, dim)].blue + src[RIDX(0, dim-2, dim)].blue 
                            + src[RIDX(1, dim-1, dim)].blue + src[RIDX(1, dim-2, dim)].blue ) / 4;

    dst[RIDX(dim-1, 0, dim)].blue = (src[RIDX(dim-1,0,dim)].blue + src[RIDX(dim-2, 0, dim)].blue
                              + src[RIDX(dim-1, 1, dim)].blue + src[RIDX(dim-2, 1, dim)].blue) / 4;

    dst[RIDX(dim-1, dim-1, dim)].blue = (src[RIDX(dim-1,dim-1,dim)].blue + src[RIDX(dim-2, dim-1, dim)].blue
                                + src[RIDX(dim-1, dim-2, dim)].blue + src[RIDX(dim-2, dim-2, dim)].blue) / 4;

    dst[RIDX(0,0,dim)].green = (src[RIDX(0,0,dim)].green + src[RIDX(0,1,dim)].green 
                        + src[RIDX(1,0,dim)].green + src[RIDX(1,1,dim)].green) / 4;

    dst[RIDX(0,dim-1,dim)].green = ( src[RIDX(0, dim-1, dim)].green + src[RIDX(0, dim-2, dim)].green 
                            + src[RIDX(1, dim-1, dim)].green + src[RIDX(1, dim-2, dim)].green ) / 4;

    dst[RIDX(dim-1, 0, dim)].green = (src[RIDX(dim-1,0,dim)].green + src[RIDX(dim-2, 0, dim)].green
                              + src[RIDX(dim-1, 1, dim)].green + src[RIDX(dim-2, 1, dim)].green) / 4;

    dst[RIDX(dim-1, dim-1, dim)].green = (src[RIDX(dim-1,dim-1,dim)].green + src[RIDX(dim-2, dim-1, dim)].green
                                + src[RIDX(dim-1, dim-2, dim)].green + src[RIDX(dim-2, dim-2, dim)].green) / 4;

    // Top edge - loop j from 1 to dim-2, i=0
    for (int j = 1; j<dim-1; j++){
        dst[RIDX(0,j,dim)].red = (src[RIDX(0,j,dim)].red + src[RIDX(0,j-1,dim)].red + src[RIDX(0,j+1,dim)].red +
                                    src[RIDX(1,j,dim)].red +src[RIDX(1,j+1,dim)].red + src[RIDX(1,j-1,dim)].red) / 6;
                                
        dst[RIDX(0,j,dim)].green = (src[RIDX(0,j,dim)].green + src[RIDX(0,j-1,dim)].green + src[RIDX(0,j+1,dim)].green +
                                src[RIDX(1,j,dim)].green +src[RIDX(1,j+1,dim)].green + src[RIDX(1,j-1,dim)].green) / 6;

        dst[RIDX(0,j,dim)].blue = (src[RIDX(0,j,dim)].blue + src[RIDX(0,j-1,dim)].blue + src[RIDX(0,j+1,dim)].blue +
                                src[RIDX(1,j,dim)].blue +src[RIDX(1,j+1,dim)].blue + src[RIDX(1,j-1,dim)].blue) / 6;
    }
    
    // Bottom edge - loop j from 1 to dim-2, i=dim-1
    for(int j = 1; j<dim-1; j++){
        dst[RIDX(dim-1,j,dim)].red = (src[RIDX(dim-1,j,dim)].red + src[RIDX(dim-1,j-1,dim)].red + src[RIDX(dim-1,j+1,dim)].red
                                + src[RIDX(dim-2,j,dim)].red + src[RIDX(dim-2,j-1,dim)].red + src[RIDX(dim-2,j+1,dim)].red) / 6;

        dst[RIDX(dim-1,j,dim)].green = (src[RIDX(dim-1,j,dim)].green + src[RIDX(dim-1,j-1,dim)].green + src[RIDX(dim-1,j+1,dim)].green
                                + src[RIDX(dim-2,j,dim)].green + src[RIDX(dim-2,j-1,dim)].green + src[RIDX(dim-2,j+1,dim)].green) / 6;

        dst[RIDX(dim-1,j,dim)].blue = (src[RIDX(dim-1,j,dim)].blue + src[RIDX(dim-1,j-1,dim)].blue + src[RIDX(dim-1,j+1,dim)].blue
                                + src[RIDX(dim-2,j,dim)].blue + src[RIDX(dim-2,j-1,dim)].blue + src[RIDX(dim-2,j+1,dim)].blue) / 6;
    }
    
    // Left edge - loop i from 1 to dim-2, j=0
    for(int i = 1; i<dim-1; i++){
        dst[RIDX(i, 0, dim)].red = (src[RIDX(i,0,dim)].red + src[RIDX(i-1,0,dim)].red + src[RIDX(i+1,0,dim)].red 
                                + src[RIDX(i,1,dim)].red + src[RIDX(i+1,1,dim)].red + src[RIDX(i-1,1,dim)].red) / 6;

        dst[RIDX(i, 0, dim)].green = (src[RIDX(i,0,dim)].green + src[RIDX(i-1,0,dim)].green + src[RIDX(i+1,0,dim)].green 
                                + src[RIDX(i,1,dim)].green + src[RIDX(i+1,1,dim)].green + src[RIDX(i-1,1,dim)].green) / 6;

        dst[RIDX(i, 0, dim)].blue = (src[RIDX(i,0,dim)].blue + src[RIDX(i-1,0,dim)].blue + src[RIDX(i+1,0,dim)].blue 
                                + src[RIDX(i,1,dim)].blue + src[RIDX(i+1,1,dim)].blue + src[RIDX(i-1,1,dim)].blue) / 6;
    }
    
    // Right edge - loop i from 1 to dim-2, j=dim-1
    for(int i = 1; i<dim-1; i++){
        dst[RIDX(i, dim-1, dim)].red = (src[RIDX(i, dim-1, dim)].red + src[RIDX(i-1, dim-1, dim)].red + src[RIDX(i+1, dim-1, dim)].red
                                    + src[RIDX(i, dim-2, dim)].red + src[RIDX(i-1, dim-2, dim)].red + src[RIDX(i+1, dim-2, dim)].red) / 6;

        dst[RIDX(i, dim-1, dim)].green = (src[RIDX(i, dim-1, dim)].green + src[RIDX(i-1, dim-1, dim)].green + src[RIDX(i+1, dim-1, dim)].green
                                    + src[RIDX(i, dim-2, dim)].green + src[RIDX(i-1, dim-2, dim)].green + src[RIDX(i+1, dim-2, dim)].green) / 6;

        dst[RIDX(i, dim-1, dim)].blue = (src[RIDX(i, dim-1, dim)].blue + src[RIDX(i-1, dim-1, dim)].blue + src[RIDX(i+1, dim-1, dim)].blue
                                    + src[RIDX(i, dim-2, dim)].blue + src[RIDX(i-1, dim-2, dim)].blue + src[RIDX(i+1, dim-2, dim)].blue) / 6;
    }

    // Interior - nested loop i from 1 to dim-2, j from 1 to dim-2
    for(int i = 1; i<dim-1; i++){
        for(int j = 1; j<dim-1; j++){
            dst[RIDX(i,j,dim)].red = (src[RIDX(i,j,dim)].red + src[RIDX(i-1,j,dim)].red + src[RIDX(i+1,j,dim)].red
                                  + src[RIDX(i,j+1,dim)].red + src[RIDX(i,j-1,dim)].red + src[RIDX(i+1,j+1,dim)].red
                                  + src[RIDX(i+1,j-1,dim)].red + src[RIDX(i-1,j-1,dim)].red + src[RIDX(i-1,j+1,dim)].red) / 9;
            
            dst[RIDX(i,j,dim)].green = (src[RIDX(i,j,dim)].green + src[RIDX(i-1,j,dim)].green + src[RIDX(i+1,j,dim)].green
                                  + src[RIDX(i,j+1,dim)].green + src[RIDX(i,j-1,dim)].green + src[RIDX(i+1,j+1,dim)].green
                                  + src[RIDX(i+1,j-1,dim)].green + src[RIDX(i-1,j-1,dim)].green + src[RIDX(i-1,j+1,dim)].green) / 9;

            dst[RIDX(i,j,dim)].blue = (src[RIDX(i,j,dim)].blue + src[RIDX(i-1,j,dim)].blue + src[RIDX(i+1,j,dim)].blue
                                  + src[RIDX(i,j+1,dim)].blue + src[RIDX(i,j-1,dim)].blue + src[RIDX(i+1,j+1,dim)].blue
                                  + src[RIDX(i+1,j-1,dim)].blue + src[RIDX(i-1,j-1,dim)].blue + src[RIDX(i-1,j+1,dim)].blue) / 9;
        }
    }
}



/*
    prefix sum approach
*/
char smooth_prefix_sum_descr[] = "Smooth prefix: prefix sum based smooth";

void smooth_prefix_sum(int dim, pixel *src, pixel *dst){
    
//matrices initialized with first row/column as all zeros, similar to how sumMat was initialized in the leetcode problem
    int *sumR = calloc((dim+1) * (dim+1), sizeof(int));
    int *sumG = calloc((dim+1) * (dim+1), sizeof(int));
    int *sumB = calloc((dim+1) * (dim+1), sizeof(int));

    //build prefix tables, same constructor logic as in range sum query problem(s)
    for(int r = 0; r<dim; r++){
        
        int pR = 0, pG = 0, pB = 0; //running row sums, reset at the start of each row
                                    //same as the prefix variable from the sumMat constructor
        
        for(int c = 0; c<dim; c++){
            pixel p = src[RIDX(r, c, dim)];
            
    //After processing column c, pR holds the sum of the red channel of the current row from column 0 through column c
            pR += p.red; pG += p.green; pB += p.blue;

            int idx = (r+1)*(dim+1) + (c+1);//idx  is the flat 1D index for position (r+1, c+1) 
                                            //in the (dim+1) × (dim+1) prefix table — the shift in action.
            
            int above = r*(dim+1) + (c+1);//'above' is the flat index for (r, c+1), which by the definition of 
                                          //sumMat holds the sum of all rows above the current row, same column range.

            sumR[idx] = pR + sumR[above]; // Adding pR and sumR[above] is identical to
            sumG[idx] = pG + sumG[above]; //'sumMat[r+1][c+1] = prefix + above' from the constructor
            sumB[idx] = pB + sumB[above]; //just written with flat array indexing instead of 2D indexing because 
                                          //calloc gives you a 1D array. This is done separately for all three channels.
        
// After this loop, sumR, sumG, sumB are three complete prefix tables, one per channel, equivalent to three separate sumMat matrices.       
        }
    }

    //answer each pixel's neighbourhood query with four lookups
    for(int i = 0; i<dim; i++){
        for(int j = 0; j<dim; j++){
            
        //clamping the neighbourhood boundaries
            int r1 = max(i-1, 0), c1 = max(j-1, 0);
            int r2 = min(i+1, dim-1), c2 = min(j+1, dim-1);
            
        //counting how many pixels are in the neighbourhood
            int count = (r2 - r1 + 1) * (c2 - c1 + 1);

        // translate to sumMat space: bottom right = (r2+1, c2+1), top left = (r1, c1)
            int bottomRight = (r2+1)*(dim+1) + (c2+1);
            int above = r1*(dim+1) + (c2+1);
            int left = (r2+1)*(dim+1) + c1;
            int topLeft = r1*(dim+1) + c1;

        //inclusion-exclusion formula appplied to each color channel separately
            dst[RIDX(i,j,dim)].red = (sumR[bottomRight] - sumR[above] - sumR[left] + sumR[topLeft])/count;
            dst[RIDX(i,j,dim)].green = (sumG[bottomRight] - sumG[above] - sumG[left] + sumG[topLeft])/count;
            dst[RIDX(i,j,dim)].blue = (sumB[bottomRight] - sumB[above] - sumB[left] + sumB[topLeft])/count;
        }
    }

    free(sumR); free(sumG); free(sumB);
}


/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version- the manual arithmetic version";
void smooth(int dim, pixel *src, pixel *dst) 
{
    smooth_optimized(dim, src, dst);
}


/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    add_smooth_function(&smooth_optimized, smooth_optimized_descr);
    add_smooth_function(&smooth_prefix_sum, smooth_prefix_sum_descr);
    /* ... Register additional test functions here */
}


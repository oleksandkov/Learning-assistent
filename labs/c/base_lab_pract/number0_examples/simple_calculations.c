#include <stdio.h> 
 
int main() { 
    float a, b, area; 
     
    printf("Enter the length of the rectangle: "); 
    scanf("%f", &a); 
    if (a < 0) {  
        printf("Length must be positive.\n"); 
        return 33; 
    } 
 
    printf("Enter the width of the rectangle: "); 
    scanf("%f", &b); 
    if (b < 0) {  
        printf("Width must be positive.\n"); 
        return 33; 
    } 
 
    area = a * b; 
    printf("Area of the rectangle: %.2f\n", area); 
     
    return 0; 
}

// Example of using the task Analyze and Explain Code (Shell)

// --- Code Explanation ---
// File extension: .c
// Total lines: 18
// Comment lines: 0
// Empty lines: 6
// Functions detected: 1
// This file contains code in .c. Add more details as needed.

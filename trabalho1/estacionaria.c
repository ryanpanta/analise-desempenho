#include <stdio.h>

int main() {

    float matriz[3][3] = {
        0.6, 0.2, 0.2, 
        0.1, 0.8, 0.1,
        0.6, 0.0, 0.4
    };

    float result[3][3];
    float temp[3][3];

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i][j] = matriz[i][j];
        }
    }

    for(int i = 1; i < 100; i++){
        for(int j = 0; j < 3; j++){
            for(int k = 0; k < 3; k++){
                temp[j][k] = 0;
                for(int l = 0; l < 3; l++){
                    temp[j][k] += matriz[j][l] * result[l][k];
                }
            }
        }

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result[i][j] = temp[i][j];
            }
        }
        
    }
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            printf("%.2f ", result[i][j]);
        }
        printf("\n");
    }
}
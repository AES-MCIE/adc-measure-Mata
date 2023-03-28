/* 
 * Pedro Mata
 * 06/06/2023
 * Implement a data logger for the ADC using C language to program a function to collect between 500-1,000 samples and computes the next data:
 *		- [x] Minimum value
 *		- [x] Maximum value
 *		- [x] Mean value
 *		- [x] RMS value
 *		- [x] Median value
 *		- [ ] Histogram plot
 * 
 *		-----
 *     |     |
 *     |     |
 *     | ADC0|	<--- input
 *     -------
 *
 * */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>

#define ADC0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

int n = 0;
int mean = 0;
int maximum = 0;
int minimum = 0;
int median = 0;
unsigned long int sum_rms = 0;
int rms = 0;
int samples[1000];
int sorted_data[1000];
int aux = 0;
int half = 0;
float v_min = 0;
float v_max = 0;
float v_mean = 0;
float v_median = 0;
float v_rms = 0;
char help[4]="help";

void sorting(){
	for (int i=0; i < n; i++){
			sorted_data[i] = samples[i];
	}
	for (int j=0; j < n-1; j++){
		for (int k=0; k < n; k++){
			if(sorted_data[k] < sorted_data[k-1]){
				aux = sorted_data[k];
				sorted_data[k] = sorted_data[k-1];
				sorted_data[k-1] = aux;
			}
		}
	}
	half = n/2;
	median = sorted_data[half];
	v_median = median*1.8;
	v_median = v_median/4095;
	printf("\n");
	printf("The median value is: ");
	printf("%f", v_median);
	printf("V.");
}

void mean_value(){
	for (int i=0; i < n; i++){
		if(i == 0){
			mean = samples[i];
		} else {
			mean = (mean*i) + samples[i];
			mean = mean / (i+1);
		}
	}
	v_mean = mean*1.8;
	v_mean = v_mean/4095;
	printf("\nThe number of samples is: ");
	printf("%i", n);
	printf("\nThe mean value is: ");
	printf("%f", v_mean);
	printf("V.");
}

void calculate_rms(){
	for(int i = 0; i < n; i++){
		aux = samples[i];
		aux = aux * aux;
		sum_rms = sum_rms + aux;
	}
	sum_rms = sum_rms/n;
	rms = sqrt(sum_rms);
	v_rms = rms*1.8;
	v_rms = v_rms/4095;
	printf("\n");
	printf("The rms value is: ");
	printf("%f", v_rms);
	printf("V.");
}

void histogram(){
	int range[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	int inf_range = 0;
	int sup_range = 256;
	for(int i = 0; i < 16; i++){
		for(int j = 0; j < n; j++){
				if( samples[j] >= inf_range && samples[j] < sup_range){
					range[i] = range[i] + 1;
				}
		}
		inf_range = inf_range + 256;
		sup_range = sup_range + 256;
	}
	int x_times;
	printf("\n\nHistogram: ");
	for( int k = 0; k < 16; k++){
		//x_times = range[k] / 5;
		x_times = range[k];
		printf("\nRange ");
		printf("%i", k);
		if( k < 10){
			printf("  |");
		} else {
			printf(" |");
		}
		for (int x = 0; x < x_times; x++){
			printf("x");
		}
	}
	inf_range = 0;
	sup_range = 256;
	printf("\n\n");
	for (int l = 0; l < 16; l++){
		printf("Range ");
		printf("%i", l);
		printf(": ");
		printf("%i", inf_range);
		printf(" - ");
		printf("%i", sup_range);
		printf("\n");
		inf_range = inf_range + 256;
		sup_range = sup_range + 256;
	}
}

int main(int argc, char *argv[]){
	printf("Executing Script\n");

	if(argc !=2){
		printf("Arguments are: %d\n", argc);
		printf("the command is used: ./adc-meas n\n");
		printf("n is the number of measurements, it must be between 500 - 1,000\n");
	return 2;
	}

	if(argv[1][0] == 'h' && argv[1][1] == 'e' && argv[1][2]=='l' && argv[1][3]=='p'){
		printf("How to use the ADC program in C:");
		printf("\nThe function of this program is to read an analog signal in the ADC0 of the BBB.");
		printf("\nThe ADC ping for this case is the ADC0, so you have to connect the signal with this pin");
		printf("\nand the GND pin with the GND of the circuit");
		printf("\n\nTo compile this C program, you have to download the compiler gcc:");
		printf("\nsudo apt-get install gcc");
		printf("\n\nThen compile the C program to create a new archive to execute the code, the recommended name is:");
		printf("\ngcc main.c -o adc-meas -lm");
		printf("\n\nFinally, just execute the new archive with a command with the cuantity of samples.");
		printf("\nThat must be between 500 and 1000 samples.");
		printf("\n\n./adc-meas 500");
		printf("\n\nFor more information, please visit: https://github.com/AES-MCIE/adc-measures-Mata");
		return 2;
	} 
	
	//if(argc != 2){
	//	printf("Arguments are: %d\n, argc");
	//	printf("the command is used: ./adc-meas n\n");
	//	printf("n is the number of measurements, it must be between 500 - 1000\n");
	//	return 2;
	//}

	n = (int) strtol(argv[1],NULL,10);
	
	if(n < 500 || n > 1000){
		printf("The cuantity of samples must be between 500 - 1,000\n");
		printf("Try again with a number in that range\n");
		return 2;
	} else {

		FILE *fp;					/*Se crea el file pointer*/
		char reads[80];			    /*Se crea un arreglo para guardar resultados*/

		for (int i=0; i<n; i++){
			fp = fopen(ADC0, "rt");
			fgets(reads,20,fp);		//Lee el valor y lo guarda en una cadena
			samples[i] = (int) strtol(reads,NULL,10);	
			if(i == 0){
				minimum = samples[i];
				maximum = samples[i];
			} else {
				if(samples[i] < minimum){
					minimum = samples[i];
				}
				if(samples[i] > maximum){
					maximum = samples[i];
				}
			}
			sleep(0.005);
		}
		
		v_min = minimum * 1.8;
		v_min = v_min/4095;
		v_max = maximum * 1.8;
		v_max = v_max/4095;

		printf("The maximum value is: ");
		printf("%f", v_max);
		printf("V.");
		printf("\nThe minimum value is: ");
		printf("%f", v_min);
		printf("V.");
		mean_value();
		sorting();
		calculate_rms();
		histogram();
		//printf("%s", reads);

		return 0;
	}
}


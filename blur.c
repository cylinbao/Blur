#include "bmp.h"

#define kern_size 5
#define max_weight 273

int gaussian_filter[5][5] = {
	{1, 4, 7, 4, 1},
	{4, 16, 26, 16, 4},
	{7, 26, 41, 26, 7},
	{4, 16, 26, 16, 4},
	{1, 4, 7, 4, 1}
};

void blur(BMP *bmp, BMP *blurbmp)
{
	int i, j, k, l, idx1, idx2, weight;
	int acc[3];
	int kern_len = kern_size/2;
	Pixel *pixel;

	for(i = 0; i < bmp->height; i++){
		for(j = 0; j < bmp->width; j++){
			acc[0] = 0; acc[1] = 0; acc[2] = 0;
			weight = max_weight;
      for(k = 0; k < kern_size; k++){                                          
        for(l = 0; l < kern_size; l++){                                        
          idx1 = j - kern_len + l;                                             
          idx2 = i - kern_len + k;                                             
          if((idx1 >= 0) && (idx2 >=0) && 
						(idx1 < bmp->width) && (idx2 < bmp->height)){
            pixel = (Pixel *) &bmp->data[(idx2*bmp->width + idx1)*sizeof(Pixel)];
						acc[0] += gaussian_filter[k][l] * (pixel->R - 0);
						acc[1] += gaussian_filter[k][l] * (pixel->G - 0);
						acc[2] += gaussian_filter[k][l] * (pixel->B - 0);
					}
					else
						weight -= gaussian_filter[k][l];
        }                                                                        
      }		
			pixel = (Pixel *) &blurbmp->data[(i*blurbmp->width + j)*sizeof(Pixel)];
			pixel->R = acc[0]/weight;
			pixel->G = acc[1]/weight;
			pixel->B = acc[2]/weight;
		}
	}
}

int main(int argc, char *argv[]) {
		char input[] = "input.bmp";
		char output[] = "blur.bmp";

		BMP bmp, blurbmp;

    bmpLoad(&bmp, input);
		blurbmp = bmp;
		blur(&bmp, &blurbmp);
    bmpSave(&blurbmp, output);

    return 0;
}

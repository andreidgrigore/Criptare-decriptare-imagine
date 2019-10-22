#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//xorshift32 1)
unsigned int *xorshift32(int valori, char *calecheie){
    FILE *fin=fopen(calecheie,"r");
    unsigned int i,seed, *r;
    r=malloc((valori*2-1)* sizeof(unsigned int));
    fscanf(fin,"%d",&seed);
    for(i=0;i<valori;i++){
        seed=seed^seed<<13;
        seed=seed^seed>>17;
        seed=seed^seed<<5;
        r[i]=seed;
    }
    return r;
}

//linializare 2)
int *liniarizare(char *caleimagine,int **header){
    FILE *fin=fopen(caleimagine,"rb");
    unsigned int length,height,i,j,padding=0,pixel;
    if(fin==NULL)printf("fisierul nu a fost gasit");
    (*header)=malloc(54* sizeof(int));
    fread(*header,54,1,fin);
    fseek(fin,18,SEEK_SET);
    fread(&length,4,1,fin);
    fread(&height,4,1,fin);
    int *v;
    v=malloc(length*height* sizeof(int));
    fseek(fin,54,SEEK_SET);
    if(length%4!=0) padding=4-(3*length)%4;
    for(i=0;i<height;i++){
        for(j=0;j<length;j++){
            fread(&pixel,3,1,fin);
            v[j+i*length]=pixel;
        }
        fseek(fin,padding,SEEK_CUR);
    }
    fclose(fin);
    return v;
}

//vector in imagine 3)
void creareimagine(char *caleimagine, int *header, int *v){
    FILE *fout=fopen(caleimagine,"wb+");
    fwrite(header,54,1,fout);
    unsigned char abc=0,k;
    unsigned int length,height,padding=0,i,j;
    fseek(fout,18,SEEK_SET);
    fread(&length,4,1,fout);
    fread(&height,4,1,fout);
    fseek(fout,54,SEEK_SET);
    if(length%4!=0)padding=4-(3*length)%4;
    for(i=0;i<height;i++){
        for(j=0;j<length;j++) {
            fwrite(&v[j+i*length],3,1,fout);
            fflush(fout);
        }
        for(k=0;k<padding;k++)
            fwrite(&abc,1,1,fout);
//        fseek(fout,padding,SEEK_CUR);
    }
    fclose(fout);
}

//criptare imagine 4)
void criptare(char *caleimagine, char *caleimaginecreata, char *calecheie){
    unsigned int *header, *v, *p, *v1, *c;
    unsigned int *r;
    unsigned int nrValori,height,length,i,ran,aux,seed;
    FILE *f=fopen(caleimagine,"rb");
    fseek(f,18,SEEK_SET);
    fread(&length,1,sizeof(unsigned int),f);
    fread(&height,1, sizeof(unsigned int),f);
    nrValori=length*height;
    fclose(f);
    v=liniarizare(caleimagine,&header);
    r=xorshift32(nrValori,calecheie);

    p=malloc((height*length)* sizeof(unsigned int));
    for(i=height*length-1;i>=0;i--)
    {
        p[i]=i;
        if(i==0)break;
    }
    for(i=height*length-1;i>0;i--){
        ran=r[height*length-1-i]%(i+1);
        aux=p[ran];
        p[ran]=p[i];
        p[i]=aux;
    }
    v1=malloc(height*length* sizeof(unsigned int));
    for(i=0;i<height*length;i++)
        v1[p[i]]=v[i];

    f=fopen(calecheie,"r");
    fscanf(f,"%d",&seed);
    fscanf(f,"%d",&seed);
    fclose(f);
    c=malloc(height*length* sizeof(unsigned int));
    c[0]=seed^v1[0]^r[height*length];
    for(i=1;i<=height*length-1;i++){
        c[i]=c[i-1]^v1[i]^r[height*length+i];
    }
    creareimagine(caleimaginecreata,header,c);
    free(v1);
    free(v);
    free(p);
    free(r);
    free(header);
}
//decriptare 5)
void decriptare(char *caleimagine, char *caleimaginecreata, char *calecheie){
    unsigned int *header, *v, *p, *v1, *p1, *c;
    unsigned int *r;
    unsigned int length,height,nrValori,i,ran,aux,seed;
    FILE *f=fopen(caleimagine,"rb");
    fseek(f,18,SEEK_SET);
    fread(&length,4,1,f);
    fread(&height,4,1,f);
    fclose(f);
    nrValori=length*height;
    c=liniarizare(caleimagine,&header);
    r=xorshift32(nrValori,calecheie);
    p=malloc((height*length)* sizeof(unsigned int));
    for(i=height*length-1;i>=0;i--) {
        p[i] = i;
        if (i == 0)break;
    }
    for(i=height*length-1;i>0;i--){
        ran=r[height*length-1-i]%(i+1);
        aux=p[ran];
        p[ran]=p[i];
        p[i]=aux;
    }
    p1=malloc(height*length* sizeof(unsigned int));
    for(i=0;i<height*length;i++)
        p1[p[i]]=i;
    free(p);
    p=p1;
    /////vezi seed;
    FILE *f5=fopen(calecheie,"r");
    fscanf(f5,"%d",&seed);
    fscanf(f5,"%d",&seed);
    v=malloc(height*length* sizeof(unsigned int));
    v[0]=seed^c[0]^r[length*height];
    for(i=1;i<length*height;i++)
        v[i]=c[i-1]^c[i]^r[length*height+i];
    v1=malloc(height*length* sizeof(unsigned int));
    for(i=0;i<height*length;i++)
        v1[p1[i]]=v[i];
    creareimagine(caleimaginecreata,header,v1);
}
//testul chi 6)
void testChi(char *caleimagine){
    unsigned int *fib, *fig, *fir;
    unsigned int length,height,padding=0,i,j;
    double val=0,fi1=0,xr=0,xg=0,xb=0;
    unsigned char c,c1,c2;
    fib=malloc(256* sizeof(unsigned int));
    fig=malloc(256* sizeof(unsigned int));
    fir=malloc(256* sizeof(unsigned int));
    for(i=0;i<=256;i++){
        fib[i]=0;
        fig[i]=0;
        fir[i]=0;
    }
    FILE *f=fopen(caleimagine,"rb");
    fseek(f,18,SEEK_SET);
    fread(&length,4,1,f);
    fread(&height,4,1,f);
    if(length%4!=0) padding=4-(3*length)%4;
    fi1=(double)(length*height)/256;
    fseek(f,54,SEEK_SET);
    for(i=0;i<height;i++){
        for(j=0;j<length;j++){
            fread(&c,1,1,f);
            fread(&c1,1,1,f);
            fread(&c2,1,1,f);
            fib[c]++;
            fig[c1]++;
            fir[c2]++;
        }
        fseek(f,padding,SEEK_CUR);
    }
    for(i=0;i<=255;i++){
        val=fib[i]-fi1;
        val=val*val;
        xb+=(double)val/fi1;
        val=fig[i]-fi1;
        val=val*val;
        xg+=(double)val/fi1;
        val=fir[i]-fi1;
        val=val*val;
        xr+=(double)val/fi1;
    }

    printf("\nB:%f G:%f R:%f",xb,xg,xr);
    fclose(f);
}

int main() {
    char *caleimagine, *caleimaginecreata, *calecheie, *caleimaginedecriptata;
    int *v,*header;
    caleimagine=malloc(100);
    caleimaginecreata=malloc(100);
    calecheie=malloc(100);
    caleimaginedecriptata=malloc(100);
    printf("Introduceti calea spre imaginea ce doriti sa o criptati:");
    fgets(caleimagine,100,stdin);
    caleimagine[strlen(caleimagine)-1]='\0';
    printf("Introduceti calea unde doriti sa salvati imaginea criptata:");
    fgets(caleimaginecreata,100,stdin);
    caleimaginecreata[strlen(caleimaginecreata)-1]='\0';
    printf("Introduceti calea spre cheia secreta:");
    fgets(calecheie,100,stdin);
    calecheie[strlen(calecheie)-1]='\0';
    printf("Introduceti calea unde doriti sa se salveze imaginea decriptata:");
    fgets(caleimaginedecriptata,100,stdin);
    caleimaginedecriptata[strlen(caleimaginedecriptata)-1]='\0';
    criptare(caleimagine,caleimaginecreata,calecheie);
    decriptare(caleimaginecreata,caleimaginedecriptata,calecheie);

}



#include <stdio.h>

#define PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define b2b(b) \
  (b & 0x8000000000 ? '1' : '0'), \
  (b & 0x4000000000 ? '1' : '0'), \
  (b & 0x2000000000 ? '1' : '0'), \
  (b & 0x1000000000 ? '1' : '0'), \
  (b & 0x0800000000 ? '1' : '0'), \
  (b & 0x0400000000 ? '1' : '0'), \
  (b & 0x0200000000 ? '1' : '0'), \
  (b & 0x0100000000 ? '1' : '0'), \
  (b & 0x80000000 ? '1' : '0'), \
  (b & 0x40000000 ? '1' : '0'), \
  (b & 0x20000000 ? '1' : '0'), \
  (b & 0x10000000 ? '1' : '0'), \
  (b & 0x08000000 ? '1' : '0'), \
  (b & 0x04000000 ? '1' : '0'), \
  (b & 0x02000000 ? '1' : '0'), \
  (b & 0x01000000 ? '1' : '0'), \
  (b & 0x800000 ? '1' : '0'), \
  (b & 0x400000 ? '1' : '0'), \
  (b & 0x200000 ? '1' : '0'), \
  (b & 0x100000 ? '1' : '0'), \
  (b & 0x080000 ? '1' : '0'), \
  (b & 0x040000 ? '1' : '0'), \
  (b & 0x020000 ? '1' : '0'), \
  (b & 0x010000 ? '1' : '0'), \
  (b & 0x8000 ? '1' : '0'), \
  (b & 0x4000 ? '1' : '0'), \
  (b & 0x2000 ? '1' : '0'), \
  (b & 0x1000 ? '1' : '0'), \
  (b & 0x0800 ? '1' : '0'), \
  (b & 0x0400 ? '1' : '0'), \
  (b & 0x0200 ? '1' : '0'), \
  (b & 0x0100 ? '1' : '0'), \
  (b & 0x80 ? '1' : '0'), \
  (b & 0x40 ? '1' : '0'), \
  (b & 0x20 ? '1' : '0'), \
  (b & 0x10 ? '1' : '0'), \
  (b & 0x08 ? '1' : '0'), \
  (b & 0x04 ? '1' : '0'), \
  (b & 0x02 ? '1' : '0'), \
  (b & 0x01 ? '1' : '0')



int main(int argc, char **argv, char **env)
{
 if (argc<3) {
  printf("[file] [ch]\n");
  return 0;
 }
 FILE *f = fopen(argv[1],"rb");
 // FILE *demod = fopen("demod.dat", "w+");
 int ch = 0;
 sscanf(argv[2],"%d",&ch);

 unsigned char buf[1024];
 int bitmask = 1<<ch;
 double fil[7] = {0};
 double old = 0;
 int dist = 0;
 unsigned int first;
 //
 // int taps[] = {0xD3675,0x90C2D, // 8_seconds_8_bit_fixed_Data_20180805134454.8.dat _CHAN_INIT_Data_20180805155528.8.dat
 //               0xB85F3,0xC058B, // _CHAN_AFTER_BUTTON_Data_20180805170303.8.dat
 //               0x937B3, 0xF4607,
 //             };

int taps[] = {
                   // x^17 + x^13 + x^12 + x^10 + x^7 + x^4 + x^2 + x^1 + 1
                   //   0123456789ABCDEF01
                   // 0b11101001001011000
                  0x0001D258,
                  // x^17 + x^14 +  x^7 +  x^6 + x^5 + x^4 + x^3 + x^2 + 1
                  //  0123456789ABCDEF01
                  //0b10111111000000100
                  0x00017E04,
                  0x0001FF6B, 0x00013F67,
                  0x0001B9EE, 0x000198D1,
                  // x^17 + x^16 + x^14 + x^12 + x^10 + x^7 + x^5 + x^1 + 1
                  // 0b11000101001010101
                  // 0b11000101001010101
                  0x000178C7, 0x00018A55,
                  0x00015777, 0x0001D911,
                  0x00015769, 0x0001991F,
                  0x00012BD0, 0x0001CF73,
                  0x0001365D, 0x000197F5,
                  0x000194A0, 0x0001B279,
                  0x00013A34, 0x0001AE41,
                  0x000180D4, 0x00017891,
                  0x00012E64, 0x00017C72,
                  0x00019C6D, 0x00013F32,
                  0x0001AE14, 0x00014E76,
                  0x00013C97, 0x000130CB,
                  0x00013750, 0x0001CB8D,
                  0x000D3675, 0x00090C2D,
      };
 const int numtaps = sizeof(taps)/sizeof(taps[0]);
 int detect[numtaps] = {0};
 unsigned int sh = 0;
 while(1) {
  int n = fread(buf,1,sizeof(buf),f);
  // for(int x = 0; x < 1024; x++){
  //   printf("%x\n",buf[x]);
  // }
  // printf("%x\n\n",buf);
  // return 0;
  int on = 0;
  if (n<1) break;
  for(int i=0;i<n;i++) {
   // printf("%d", buf[i]);
   double in = (buf[i]&bitmask)?1.0:-1.0;
   // Filter it using simple FIR to get rid of some one-sample glitches around edges
   fil[0]=fil[1];fil[1]=fil[2];fil[2]=fil[3];fil[3]=fil[4];fil[4]=fil[5];fil[5]=in;
   double v=fil[0]*0.0280+fil[1]*0.1597+fil[2]*0.2779+fil[3]*0.3396+fil[4]*0.2779+fil[5]*0.1597+fil[6]*0.0280;
   // v = in;
   double diff = in*old;
   // printf("%f\n",diff);
   // return 0;
   old = v;
   dist++;
   if (diff<0 && dist>10) { // 10 = little bit more than one symbol length
    if (dist>100) { // long run of same bits between bursts
      sh = 0;
     int max = 0;
     int maxat = 0;
     for(int j=0;j<numtaps;j++) {
      printf("%06X:%08d   \n",taps[j],detect[j]); // detailed LFSR report
      if (detect[j]>max) {
       max=detect[j];
       maxat=j;
      }
     }
     printf("found LFSR %06X bit %d",taps[maxat],maxat&1); // print best one to get bits
     printf("\n");
     for(int k=0;k<numtaps;k++) detect[k]=0;
    } else {
     int bit = (v>0)?1:0;
     // printf("%d",bit); // print raw demodulated bits if you want that

     sh=(sh<<1)|bit;

     // printf("%x\n", sh);


     // printf(PATTERN"\n", b2b(sh));


     for(int j=0;j<numtaps;j++) {
     // for(int j=0;j<1;j++) {
      unsigned int b = sh&taps[j];
      printf("%x\n",sh);
      printf(PATTERN"\n", b2b(sh));
      // printf(PATTERN"\n", b2b(b));
      b^=b>>16;
      // printf(PATTERN"\n", b2b(b));
      b^=b>>8;
      // printf(PATTERN"\n", b2b(b));
      b^=b>>4;
      // printf(PATTERN"\n", b2b(b));
      b^=b>>2;
      // printf(PATTERN"\n", b2b(b));
      b^=b>>1;
      // printf(PATTERN"\n", b2b(b));
      b&=1; // parity
      // printf(PATTERN"\n", b2b(b));
      // printf("---\n");
      if (!b){
        // printf("%x,\n", b);
        detect[j]++; // count zeroes
      }
    }
    }
    dist=0;
   }
 }
  // printf("\n");
 }
 fclose(f);
 return 0;
}

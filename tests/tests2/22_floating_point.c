#include <stdio.h>
#include <math.h>

float fd;

int
test()
{
   // was an internal tcc compiler error with arm64 backend until 2019-11-08
   if (fd < 5.5) {
     return 1;
   } else {
     return 0;
   }
}

int main()
{
   static int e1 = -1.0 == 0.0;
   static int e2 = -1.0 != 0.0;
   static int e3 = -1.0 < 0.0;
   static int e4 = -1.0 >= 0.0;
   static int e5 = -1.0 <= 0.0;
   static int e6 = -1.0 > 0.0;
   // variables
   float a = 12.34 + 56.78;
   printf("%f\n", a);

   // infix operators
   printf("%f\n", 12.34 + 56.78);
   printf("%f\n", 12.34 - 56.78);
   printf("%f\n", 12.34 * 56.78);
   printf("%f\n", 12.34 / 56.78);

   // comparison operators
   printf("%d %d %d %d %d %d\n", 12.34 < 56.78, 12.34 <= 56.78, 12.34 == 56.78, 12.34 >= 56.78, 12.34 > 56.78, 12.34 != 56.78);
   printf("%d %d %d %d %d %d\n", 12.34 < 12.34, 12.34 <= 12.34, 12.34 == 12.34, 12.34 >= 12.34, 12.34 > 12.34, 12.34 != 12.34);
   printf("%d %d %d %d %d %d\n", 56.78 < 12.34, 56.78 <= 12.34, 56.78 == 12.34, 56.78 >= 12.34, 56.78 > 12.34, 56.78 != 12.34);
   printf("%d %d %d %d %d %d\n", e1, e2, e3, e4, e5, e6);

   // assignment operators
   a = 12.34;
   a += 56.78;
   printf("%f\n", a);

   a = 12.34;
   a -= 56.78;
   printf("%f\n", a);

   a = 12.34;
   a *= 56.78;
   printf("%f\n", a);

   a = 12.34;
   a /= 56.78;
   printf("%f\n", a);

   // prefix operators
   printf("%f\n", +12.34);
   printf("%f\n", -12.34);

   // type coercion
   a = 2;
   printf("%f\n", a);
   printf("%f\n", sin(2));

   return 0;
}

/* vim: set expandtab ts=4 sw=3 sts=3 tw=80 :*/

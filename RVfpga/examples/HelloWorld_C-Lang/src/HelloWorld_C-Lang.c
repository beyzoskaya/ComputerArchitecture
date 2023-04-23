#if defined(D_NEXYS_A7)
   #include <bsp_printf.h>
   #include <bsp_mem_map.h>
   #include <bsp_version.h>
#else
   PRE_COMPILED_MSG("no platform was defined")
#endif
#include <psp_api.h>

int fibonacci(int number)
{
   int fib[number+1];
   int itr;

   fib[0] = 0;
   fib[1] = 1;

   for(itr = 2; itr <= number; itr++){
      fib[itr] = fib[itr-1] + fib[itr -2];
   }

   return fib[number];
}

int leastCommonMultip(int number_1, int number_2)
{
   static int maximum = 1;
   if(maximum % number_1 ==0 && maximum % number_2 == 0)
   {
      return maximum;
   } else {
      maximum++;
      leastCommonMultip(number_1,number_2);
      return maximum;
   }
}

int main(void)
{
   int i;


   /* Initialize Uart */
   uartInit();
   printfNexys("%d",fibonacci(4));
   printfNexys("%d",leastCommonMultip(20,30));

   while(1){
      /* Print "hello world" message on the serial output (be careful not all the printf formats are supported) */
      printfNexys("hello world\n");
      /* Delay */
      for (i=0;i<10000000;i++){};
   }

}
#include <stdio.h> // printf


#include "malloc_perso.h"
#include "free_perso.h"
#include <unistd.h>
#include <stdlib.h>


#if !defined(MALLOC_ALIGN)
 #define MALLOC_ALIGN 16
#endif

int main5()
{
       int* a = malloc_perso(16);

       printf("\n");

       a = realloc_perso(a, 0);

       return 0;
}


int main3()
{
       char* test = (char*)calloc_perso(1, 1);

       for (int i=0; i<16; i++)
              test[i] = i + 'a';

       test[16] = 'q';

       char* test2 = (char*)calloc_perso(1, 1);
       //test2[0] = 'c';

       printf("\n%s %s", test, test2);

       return 0;
}


// Ce main de test va vous permettre de tester votre malloc et votre free
// Commentez les sections utilisant des fonctions que vous n’avez pas encore codees.
// Comparez votre resultat avec celui attendu en telechargeant le fichier resultat_type.txt et une commande shell du genre :
// ./malloc_exe > mon_resultat.txt
// puis : diff mon_resultat.txt resultat_type.txt
// ATTENTION !!! Ce n’est qu’un main de test. En soutenance un main plus severe essaiera de tester les limites de votre malloc. Codez de maniere robuste en evitant les crash !!!
int main()
{
  printf(
	"This is a malloc test - you may not get the same memory addresses than me\n"
	"All lines prefixed [MALLOC INTERNAL] are printed inside malloc to better understand what's going on\n"
	"Feel free to add your version of them or copy them to the right place to debug your own malloc!\n"
	"But the output should roughly be the same, i.e. when an address is reused,"
	" it should be reused in your case too.\n"
);


  printf("----------------------------\nSTEP 1: First malloc: no bloc - SHOULD EXTEND HEAP\n\n");
  int* test = (int*)malloc_perso(4096);
  printf("\nCHECK: Pointer %p is aligned on %d ? %s\n",
         (void*)test, MALLOC_ALIGN, ((size_t)test % MALLOC_ALIGN) == 0 ? "yes" : "no" );

  test[0] = 1;
  test[1] = 2;
  test[2] = 2;


  printf("\n----------------------------\nSTEP 2 : First free\n\n");
  free_perso(test);


  printf("\n----------------------------\nSTEP 3: Second malloc fitting bloc SHOULD SPLIT\n\n");
  int* test2 = (int*)malloc_perso(16);  
  test2[0] = 2323;


  printf("\nCHECK: Pointer test2 should be equal to test (reuse first block): %p = %p\n",
         (void*)test, (void*)test2);
  



  printf("\n----------------------------\nSTEP 4: third malloc should take new split block\n");
  int* test3 = (int*)malloc_perso(4030);
  test3[0] = 111111;


  printf("\nCHECK: Pointer %p is aligned on %d ? %s\n",
         (void*)test3, MALLOC_ALIGN, ((size_t)test3 % MALLOC_ALIGN) == 0 ? "yes" : "no" );




  printf("\n----------------------------\nSTEP 5: Second free should free first block (%p)\n\n", (void*)(((t_block*)test2)-1));
  free_perso(test2);
  
  
  printf("\n----------------------------\nSTEP 6: Fourth malloc should take first block without split\n\n");
  int* test4 = (int*)malloc_perso(12);
  test4[1] = 3;



  printf("\nCHECK: Pointer test4 should be equal to test (reuse first block): %p = %p\n",
         (void*)test, (void*)test4);


  
  printf("\n----------------------------\nSTEP 7: Fifth malloc(%d) no available bloc - SHOULD EXTEND HEAP\n\n", 5555);
  int* test5 = (int*)malloc_perso(5555);
  test5[33] = 3;


  printf("\nCHECK: Pointer test5 should be a new block (no block reuse): %s\n",
         (test5 != test && test5 != test3) ? "OK" : "FAIL");


  printf("\nCHECK: Pointer %p is aligned on %d ? %s\n",
         (void*)test5, MALLOC_ALIGN,  ((size_t)test5 % MALLOC_ALIGN) == 0 ? "yes" : "no" );
    
  printf("\n----------------------------\nSTEP 8: Free the two first blocks - SHOULD FUSION\n");
  
  printf("\nFirst free\n\n"); 
  free_perso(test3);
  
  printf("\nSecond free\n\n"); 
  free_perso(test4);

  printf("\n----------------------------\nSTEP 9: Final free - should merge\n");
  free_perso(test5);
 
  return 0;
}

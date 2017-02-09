#include <stdio.h>

int main(){
  char name[100];
  char final[700];
  float preco;
  float total = 0;
  int qtd;
  int count;
  printf("%s\n%s\n", "Bem Vindo ao Ex.4", "Digite o número de produtos que deseja adicionar:");
  scanf("%d", &count);
  char head[] = "Produto | PreçosUnd | Qtd | PreçoTotal" ;
  sprintf(final,"%s\n",head);

  for (int i = 1; i <= count; i++) {
    printf("Nome produto %d :\n", i);
    scanf("%s", name);
    printf("Preço produto %d :\n", i);
    scanf("%f", &preco);
    printf("Quantidade produto %d :\n", i);
    scanf("%d", &qtd);
    total += preco*qtd;
    sprintf(final,"%s\n%s | %.2f | %d | %.2lf\n",final,name,preco,qtd,preco*qtd);
  }
  sprintf(final,"%s\n      |       |        |  %.2lf\n", final, total);

  puts(final);

  return 0;
}

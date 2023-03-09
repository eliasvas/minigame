#include "base.h"

char *a = NULL;

void push_text(char *str){
	for (int i = 0; i < strlen(str); ++i){
		da_push(a, str[i]);
	}
	da_push(a,'\n');
}
int main(){
	push_text("  _ __ ___  ");
	push_text(" | '_ ` _ \\ ");
	push_text(" | | | | | |");
	push_text(" |_| |_| |_|");
	printf("%s", &a[0]);
	da_free(a);
}
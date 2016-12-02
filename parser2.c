/*Alumno Oscar Marcelo Caballero*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define TAMCOMPLEX 		20  // longitud del componente lexema 
#define TAMLEX			50		// tamaño del lexema


FILE *archivo;			// archivo Fuente 
FILE *salida;



char id[TAMLEX];		// Utilizado por el analizador lexico
int numLinea=1;
char msg[100];

struct token {
	char componente_lexico[TAMCOMPLEX];
	char lexema[TAMLEX];
	
}t;

void error(const char* mensaje)
{
	printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
}
/*----------------------------------------------Analisis Lexico----------------------------------------------------*/
void getToken()
{
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	
	struct token e;


	while((c=fgetc(archivo))!=EOF)
	{
		
		if (c==' ' || c=='\t'){
			//fprintf(salida,"%c",c);
			continue;	
		}	
		else if(c=='\n')
		{
			numLinea++;
			//fprintf(salida,"%c",c);
			continue;

		} 
	//Reconocimiento del string
		else if(c == '"'){
			i=0;
			id[i]=c;
			c=fgetc(archivo);
			while(c!='"' && c!='\n'){
				if(i <= (TAMLEX-3)){
					i++;
					id[i]=c;
				}else{
					error("Longitud del string excede tamaño de buffer, busca \\n o \" \n");
					/*Busca el salto de linea o comilla doble*/					
					while(c != '\n' && c != '"'){
						c=fgetc(archivo);
						if(c == EOF){
							error("Fin de archivo inesperado \n");
							exit(1);
						}
					}

					
					break;
				}

				c = fgetc(archivo);
				
			
			}

			if(c == '"'){
				id[i+1] = '"';
				id[i+2] = '\0';
				strcpy(t.componente_lexico,"STRING");
				strcpy(t.lexema,id); 
			}
			if(c == '\n'){
				error("Se encontro salto de linea, se ignora la lectura");
				//strcpy(t.componente_lexico,"");
				//stpcpy(t.lexema,"");
				ungetc(c,archivo);
				continue;
				
			}
			c=0;
			break;
	
		}

	
		//Reconociendo numeros
		else if (isdigit(c))
		{
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;
				
				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;
					
					case 1://un punto, debe seguir un digito 
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							error(msg);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							error(msg);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							error(msg);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						
						strcpy(t.componente_lexico,"NUMBER");
						strcpy(t.lexema,id);
						
						break;
					/*En caso de error en la lectura del numero*/
					case -1:
						if(c != EOF){
							/*Busca el salto de linea o espacio*/
							while(c != '\n' && c!= ' '){
								c=fgetc(archivo);
									if(c == EOF){
										error("Fin de archivo inesperado");
										exit(-1);
									}
							}
				
							ungetc(c,archivo);
							strcpy(t.componente_lexico,"");
							strcpy(t.lexema,"");
						}else{ 
								error("Fin de archivo");
								exit(1);
						}	
						acepto=1;
						c=0;
						break;
					}
					/*Controla que el numero leido no exceda el tamaño del buffer*/
					if(i==TAMLEX-2){
							sprintf(msg,"Longitud del numero excede el tamaño de buffer, se almacenara hasta %d cifras",TAMLEX-1);
							error(msg);
							while(c != '\n' && c != ' '){
								c=fgetc(archivo);
								if(c == EOF){
									error("Fin de archivo inesperado");
								exit(1);
								}
							
							}
							ungetc(c,archivo);
							id[++i]='\0';
							strcpy(t.componente_lexico,"NUMBER");
							strcpy(t.lexema,id);
							break;				
					}
				}
				if(strcmp(t.componente_lexico,"") == 0){
					continue;
				}
			break;
		}
		//Reconocimiento de palabras reservadas
		
		else if(isalpha(c)){
			i=0;
			id[i]=c;
			c=fgetc(archivo);
			while(isalpha(c)){
				if(i <= (TAMLEX-2)){
					i++;
					id[i]=c;
				}else{
					sprintf(msg,"Longitud de la palabra reservada excede tamaño de buffer, se almacenara hasta %d caracteres",TAMLEX-1);
					error(msg);
					/*Busca el salto de linea o espacio*/
					while(c != '\n' && c != ' '){
						c=fgetc(archivo);
						if(c == EOF){
							error("Fin de archivo inesperado");
							exit(1);
						}
					}
					
					break;
				}

				c = fgetc(archivo);
				
			
			}
			id[i+1] = '\0';
		    
			
			if (strcmp("false",id) == 0 || strcmp("FALSE",id) == 0){
				strcpy(t.componente_lexico, "FALSE");/*aqui deberia escribir el archivo*/
				strcpy(t.lexema,id);
			}else if (strcmp("true",id) == 0 || strcmp("TRUE",id) == 0){
				strcpy(t.componente_lexico,"TRUE");/*aqui*/
				strcpy(t.lexema,id);
			}else if (strcmp("null",id) == 0||strcmp("NULL",id) == 0){
				strcpy(t.componente_lexico,"NULL");/*aqui*/
				strcpy(t.lexema,id);
			}else{
				printf("Lin %d: No se reconoce  '%s' como palabra reservada.\n" ,numLinea,id);
				/*Se busca salto de de linea o espacio*/
				while(c != '\n' && c != ' '){
						c=fgetc(archivo);
						if(c == EOF){
							error("Fin de archivo inesperado");
							exit(1);
						}
				}
				
				ungetc(c,archivo);
				//strcpy(t.componente_lexico,"");
				//strcpy(t.lexema,"");	
				continue;
			}
			ungetc(c,archivo);
			c=0;
			break;
		}
		//Reconocimiento de caracteres validos
		else if (c==':')
		{
			strcpy(t.componente_lexico,":");
			stpcpy(t.lexema,":");
			break;
		}
		else if (c==',')
		{
			strcpy(t.componente_lexico,",");
			stpcpy(t.lexema,",");
			break;
		}
		else if (c=='{')
		{
			strcpy(t.componente_lexico,"{");
			stpcpy(t.lexema,"{");
			break;
		}
		else if (c=='}')
		{
			strcpy(t.componente_lexico,"}");
			stpcpy(t.lexema,"}");
			break;
		}
		else if (c=='[')
		{
			strcpy(t.componente_lexico,"[");
			stpcpy(t.lexema,"[");
			break;
		}
		else if (c==']')
		{
			strcpy(t.componente_lexico,"]");
			stpcpy(t.lexema,"]");
			break;
		}
		//Descartando  caracteres no validos
		else if (c!=EOF)
		{
			sprintf(msg,"%c no esperado",c);
			error(msg);
			/*Busca salto de linea o espacio*/
			while(c != '\n' && c != ' '){
					c=fgetc(archivo);
					if(c == EOF){
						error("Fin de archivo inesperado");
						exit(1);						}
				}
			ungetc(c,archivo);
			//strcpy(t.componente_lexico,"");
			//strcpy(t.lexema,"");	
			continue;
			
			
		}
	}
	//Finalizacion del archivo
	if (c==EOF)
	{
		strcpy(t.componente_lexico,"EOF");
		strcpy(t.lexema,"EOF");

	}
	
}

/*-----------------------------------------Analisis Sintactico-------------------------------------------------*/

/*Gramatica
json-> element eof
element->[array] |{object}
array-> element element_list| E
element_list-> , element element_list | E
object -> attribute attribute_list | E
attribute_list -> , attribute attribute_list | E
attribute -> string : attribute_value
attribute_value -> string | number | true | false | null | element
*/

//Estructura de un conjunto de tokens, todos los conjuntos de tokens tienen un valor centinela al final
typedef char token[7];
token fin[] = {"EOF",""}; // conjunto que representa fin de archivo y su valor centinela


void json(void);
void element(token *,token *);
void array(token *,token *);
void element_list(token *,token *);
void object(token *,token *);
void attribute_list(token *,token *);
void attribute(token *,token *);
void attribute_value(token *,token *);

int aceptar = 1;
int cantTab = 0;

char msj[50];

void imprimirTab(void)
{
	int z = 0;
	for(z;z<cantTab;z++)
	{
		fprintf(salida,"%c",'\t');
	}
}

void eliminar_comillas(char * cadena)
{
	int l = strlen(cadena);
	int i;
	cadena[l-1] = '\0';
	for(i=0;i<l;i++)
	{
		cadena[i] = cadena[i+1];
	}

}

void listar_pri(token * c)
{
	
	int i=0;
	int j=0;
	while(strcmp(c[i],"")!=0){
		int z = 0;
		msj[j] ='\'';
		j=j+1;
		while(c[i][z] != '\0' ){
			msj[j] = c[i][z];
			j=j+1;
			z=z+1;
		}
		msj[j] ='\'';
		j=j+1;
		msj[j] =' ';
		j=j+1;
		i=i+1;
	}
	msj[j] = '\0' ;
	//msj[49] = '\0' ;
}

int pertenece(token * conjunto)
{
	int i=0;
	int esta = 0;
	 while(strcmp(conjunto[i],"") != 0) // valor centinela indica fin del conjunto.
    {
        if(strcmp(conjunto[i],t.componente_lexico) == 0)
		{
			esta=1;
		}
        i++;
    }
	return esta;
}

void buscar(token * pri, token * sgte)
{
	while(!pertenece(pri) && !pertenece(sgte) && !pertenece(fin))
	{
		getToken();
		
	}
}

void s_error(const char* mensaje){
	printf("Lin %d: Error Sintactico. %s.\n",numLinea,mensaje);
}

void match(const char* token_esperado){
	if(strcmp(t.componente_lexico,token_esperado) == 0){
		getToken();	
	}else{ 
		aceptar=0;
		sprintf(msg,"se esperaba '%s' encontrado '%s'",token_esperado, t.componente_lexico);
		s_error(msg);
	}
}

/*-----*/
void json(){
	
	token pri[] = {"{","[",""};
	if(pertenece(fin))
	{
		sprintf(msg,"no se esperaba 'EOF'");
		s_error(msg);
	}
	if(!pertenece(pri) && !pertenece(fin))
	{
		aceptar = 0;
		sprintf(msg,"se esperaba '{' o '[ ' encontrado '%s'",t.componente_lexico);
		s_error(msg);
		buscar(pri,fin);
	}
	if(pertenece(pri))
	{	
		if(strcmp(t.componente_lexico,"{") == 0)
		{
			token priE[] = {"{",""};
			token sgtE[] = {"}",""};
			element(priE,sgtE);
		}

		if(strcmp(t.componente_lexico,"[") == 0)
		{
			token priE[] = {"[",""};
			token sgtE[] = {"]",""};
			element(priE,sgtE);
		}
		
	}
	match("EOF");
}

void element(token * pri, token * sgte){
	if(pertenece(fin))
	{
		sprintf(msg,"no se esperaba 'EOF'");
		s_error(msg);
		aceptar = 0;
	}
	if(pertenece(sgte))
	{
		listar_pri(pri);
		sprintf(msg,"se esperaba %s encontrado '%s'",msj,t.componente_lexico);
		s_error(msg);
		aceptar=0;
	}
	if(!pertenece(pri) && !pertenece(sgte)){
		listar_pri(pri);
		sprintf(msg,"se esperaba  %s encontrado '%s'",msj,t.componente_lexico);
		s_error(msg);
		buscar(pri,sgte);
		aceptar=0;
	}
	if(pertenece(pri)){
		if(strcmp(t.componente_lexico,"[") == 0){
			token priA[] = {"[","{",""};
			token sgteA[] = {"]",""};		
			match("[");
			array(priA,sgteA);
			match("]");
		}
	 	if(strcmp(t.componente_lexico,"{") == 0){	
			token priO[] = {"STRING",""};
			token sgteO[] = {"}",""};
			match("{");
			object(priO,sgteO);
			match("}");
		}
	}
}

void array(token * pri, token * sgte){
	if(pertenece(fin))
	{
		sprintf(msg,"no se esperaba 'EOF'");
		s_error(msg);
		aceptar = 0;
	}
	if(!pertenece(pri) && !pertenece(sgte))
	{
		
		sprintf(msg,"se esperaba '[' o '{' o ']' encontrado '%s'",t.componente_lexico);
		s_error(msg);
		buscar(pri,sgte);
		aceptar=0;

	}
	if(pertenece(pri)){

		cantTab=cantTab+1;
		fprintf(salida,"%c",'\n');
		imprimirTab();

		if(strcmp(t.componente_lexico,"{") == 0){
			
			
			token priE[] = {"{",""};
			token sgteE[]= {"}",""};
			
			
			fprintf(salida,"<item>");
			cantTab=cantTab+1;

			element(priE,sgteE);

			cantTab=cantTab-1;
			fprintf(salida,"%c",'\n');
			imprimirTab();
			fprintf(salida,"</item>");
			
		}
		if(strcmp(t.componente_lexico,"[") == 0){
			token priE[] = {"[",""};
			token sgteE[]= {"]",""};
			
			fprintf(salida,"<item>");
			cantTab=cantTab+1;

			element(priE,sgteE);

			cantTab=cantTab-1;
			fprintf(salida,"%c",'\n');
			imprimirTab();
			fprintf(salida,"</item>");
			
		}
		fprintf(salida,"%c",'\n');
		imprimirTab();

		token priEL[] = {",",""};
		token sgteEL[] = {"]",""};
		element_list(priEL,sgteEL);

		cantTab=cantTab-1;
		fprintf(salida,"%c",'\n');
		imprimirTab();

	}
}

void element_list(token * pri,token * sgte){
	if(pertenece(fin))
	{
		sprintf(msg,"no se esperaba 'EOF'");
		s_error(msg);
		aceptar = 0;
	}
	if(!pertenece(pri) && !pertenece(sgte))
	{
		sprintf(msg,"se esperaba ',' o ']' encontrado '%s'",t.componente_lexico);
		s_error(msg);
		buscar(pri,sgte);
		aceptar=0;

	}
	if(pertenece(pri)){
		match(",");
		token priE[] = {"{","[",""};
		token sgteE[] = {",","}","]",""};
		
		fprintf(salida,"<item>");
		cantTab=cantTab+1;
		
		element(priE,sgteE);

		cantTab=cantTab-1;
		fprintf(salida,"%c",'\n');
		imprimirTab();
		fprintf(salida,"</item>");

		token priEL[] = {",",""};
		token sgteEL[] = {"]",""};
		element_list(priEL,sgteEL);
		
		
	}
}

void object(token * pri,token * sgte){
	if(pertenece(fin))
	{
		sprintf(msg,"no se esperaba 'EOF'");
		s_error(msg);
		aceptar = 0;
	}
	if(!pertenece(pri) && !pertenece(sgte))
	{
		sprintf(msg,"se esperaba 'STRING' o '}' encontrado '%s'",t.componente_lexico);
		s_error(msg);
		buscar(pri,sgte);
		aceptar=0;
	}
	if(pertenece(pri)){
		token priA[] = {"STRING",""};
		token sgteA[] = {",","}",""};
		attribute(priA,sgteA);
		token priAL[] = {",",""};
		token sgteAL[] = {"}",""};
		attribute_list(priAL,sgteAL);	
	}

}

void attribute(token * pri,token * sgte){
	if(pertenece(fin))
	{
		sprintf(msg,"no se esperaba 'EOF'");
		s_error(msg);
		aceptar = 0;
	}
	if(pertenece(sgte))
	{
		sprintf(msg,"se esperaba 'STRING' encontrado '%s'",t.componente_lexico);
		s_error(msg);
		aceptar = 0;	
	}
	if(!pertenece(pri) && !pertenece(sgte))
	{
		sprintf(msg,"se esperaba 'STRING' encontrado '%s'",t.componente_lexico);
		s_error(msg);
		
		buscar(pri,sgte);
		aceptar=0;
	}
	if(pertenece(pri)){
		eliminar_comillas(t.lexema);
		char caracter [TAMLEX];
		strcpy(caracter,t.lexema);

		fprintf(salida,"%c",'\n');
		imprimirTab();
		fprintf(salida,"<%s>",caracter);

		match("STRING");
		match(":");
		token priAV[] = {"STRING","NUMBER","TRUE","FALSE","NULL","{","[",""};
		token sgteAV[] = {"}",""};
		attribute_value(priAV,sgteAV);

		fprintf(salida,"</%s>",caracter);
		
		
	}
}

void attribute_list(token * pri ,token * sgte){
	if(pertenece(fin))
	{
		sprintf(msg,"no se esperaba 'EOF'");
		s_error(msg);
		aceptar = 0;
	}
	if(!pertenece(pri) && !pertenece(sgte)){
		sprintf(msg,"se esperaba ',' o '}' encontrado '%s'",t.componente_lexico);
		s_error(msg);
		buscar(pri,sgte);
		buscar(sgte,sgte);
		aceptar=0;
	}
	if(pertenece(pri)){
		match(",");
		token priA[] = {"STRING",""};
		token sgteA[] = {",","}",""};
		attribute(priA,sgteA);
		token priAL[] = {",",""};
		token sgteAL[] = {"}",""};
		attribute_list(priAL,sgteAL);
	}
}

void attribute_value(token * pri, token * sgte){
	if(pertenece(fin))
	{
		sprintf(msg,"no se esperaba 'EOF'");
		s_error(msg);
		aceptar = 0;
	}
	if(pertenece(sgte))
	{
		sprintf(msg,"se esperaba 'STRING','NUMBER','FALSE','NULL','{','[' encontrado '%s'",t.componente_lexico);
		s_error(msg);
		aceptar = 0;	
	}
	if(!pertenece(pri) && !pertenece(sgte)){
		sprintf(msg,"se esperaba 'STRING','NUMBER','FALSE','NULL','{','[' encontrado '%s'",t.componente_lexico);
		s_error(msg);
		buscar(pri,sgte);
		aceptar=0;
	}
	if(pertenece(pri)){
		if(strcmp(t.componente_lexico,"STRING") == 0){
			fprintf(salida,"%s",t.lexema);
			match("STRING");	
		}
		if(strcmp(t.componente_lexico,"NUMBER") == 0){
			fprintf(salida,"%s",t.lexema);
			match("NUMBER");	
		}
		if(strcmp(t.componente_lexico,"TRUE") == 0){
			fprintf(salida,"%s",t.lexema);
			match("TRUE");	
		}
		if(strcmp(t.componente_lexico,"FALSE") == 0){
			fprintf(salida,"%s",t.lexema);
			match("FALSE");	
		}
		if(strcmp(t.componente_lexico,"NULL") == 0){
			fprintf(salida,"%s",t.lexema);
			match("NULL");	
		}
		if(strcmp(t.componente_lexico,"[") == 0){
			token priE[] = {"[",""}; 
			token sgteE[] = {"]",""};
			
			element(priE,sgteE);
		}
		if(strcmp(t.componente_lexico,"{") == 0){
			token priE[] = {"{",""}; 
			token sgteE[] = {"}",""};
			
			element(priE,sgteE);
			
		}
	}
}


int main (int argc,char* args[]){
	
	
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo fuente no encontrado.\n");
			exit(1);
		}
		salida = fopen("salida.txt","w");
		getToken();
		if (strcmp(t.componente_lexico,"EOF") != 0){	
			json();	
			if(aceptar)
			{
				printf("Se acepta\n");
			}else{
				printf("No se acepta\n");
			}		
		}else{
			printf("Archivo vacio\n");
		}

		fclose(archivo);
		fclose(salida);
		
		
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}

	return 0;
	
}

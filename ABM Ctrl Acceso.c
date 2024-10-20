#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct INGRESANTE_S {
    int dni;
    char nombreCompleto[71];
    char categoriaIngreso[6];
    int numTarjeta;
    char fechaART[15];
    bool activo;
} Ingresante;

int menuPrincipal(FILE*, FILE*);
bool buscarIngresanteTex(FILE*,int);
bool buscarIngresanteBin(FILE*,int,Ingresante*);
void altaIngresante (FILE*, FILE*);
void modifIngresante(FILE*);
void bajaIngresante(FILE*);
void separarEnTokens (Ingresante*,char*);
void reacomodar(char*, char*, int);
void generarBinDesdeCsv(FILE*, FILE*);
void mostrarIngresante(Ingresante);
void mostrarIngresanteCompleto(Ingresante);
void mostrarActivos(Ingresante);
void mostrarOrdenado(FILE*);
bool esFechaValida(char*);
void mostrarTitulo();
void mostrarTituloCompleto();
void leerBinario(FILE*);
void leerBinarioCompleto(FILE*);
void cargarVectorIngr(Ingresante*,FILE*);
int mostrarVectorIngr(Ingresante*, int);
void ordenarPorDNI(Ingresante*,int, char);
void ordenarPorNombre(Ingresante*,int, char);
void ordenarPorCateg(Ingresante*,int, char);
void ordenarPorTarj(Ingresante*,int, char);
void ordenarPorFecha(Ingresante*,int, char);
int calcularCantidadRegs (FILE *f);

void leerCsv(FILE*);
void leerCadena(char*,char*);
void leerEntero(char*,int*);
void leerFloat (char*,float*);
void leerCaracter(char*,char*);
int pausa (char*);

int main () {
    FILE *tex, *bin;
    tex = fopen("ingresantes.csv","rt+");
    bin = fopen("ingresantes.bin","rb+");
    if (!tex) {
        system("color 47");
        printf("No se encontr%c ingresantes.csv.\nPresione una tecla para continuar . . .\n",162);
        getch();
    }
    system("color 1f");
    if (!bin) {
        printf("No se encontr%c ingresantes.bin, se generar%c uno vac%co.\nPresione una tecla para continuar . . .\n",162,160,161);
        getch();
        bin = fopen("ingresantes.bin","wb+");
    }
    menuPrincipal(tex,bin);
    fclose(tex);
    fclose(bin);
    return 0;
}

int menuPrincipal (FILE *tex, FILE *bin) {
    int op;
    do {
        system("cls");
        printf("MENU PRINCIPAL:\n\n"
               "<1>\tCargar binario desde ingresantes.csv\n\n"
               "<2>\tDar de alta ingresante\n\n"
               "<3>\tModificar ingresante\n\n"
               "<4>\tDar de baja (l%cgica) ingresante\n\n"
               "==========================================\n\n"
               "<5>\tLeer binario\n\n"
               "<6>\tLeer binario (COMPLETO)\n\n"
               "<7>\tLeer ingresantes.csv\n\n"
               "<8>\tMostrar ingresantes ordenados\n\n"
               "\n\n"
               "<0>\tSalir\n\n"
               ,162);
        scanf("%d",&op);
        switch (op) {
            case 0:
                    break;
            case 1:
                    generarBinDesdeCsv(tex,bin);
                    break;
            case 2:
                    altaIngresante(NULL, bin);
                    pausa("\nPresione una tecla para continuar . . . ");
                    break;
            case 3:
                    modifIngresante(bin);
                    pausa("\nPresione una tecla para continuar . . . ");
                    break;

            case 4: bajaIngresante(bin);
                    pausa("\nPresione una tecla para continuar . . . ");
                    break;
            case 5:
                    leerBinario(bin);
                    pausa("\nPresione una tecla para continuar . . . ");
                    break;
            case 6:
                    leerBinarioCompleto(bin);
                    pausa("\nPresione una tecla para continuar . . . ");
                    break;
            case 7:
                    leerCsv(tex);
                    pausa("\nPresione una tecla para continuar . . . ");
                    break;
            case 8:
                    mostrarOrdenado(bin);
                    pausa("\nPresione una tecla para continuar . . . ");
                    break;
            default:
                    pausa("Por favor, ingrese una opcion valida.\nPresione una tecla para continuar . . . ");
                    break;
        }
    } while (op != 0);
    return op;
}

void altaIngresante (FILE *tex, FILE *bin) {
    Ingresante i;
    system("cls");
    leerEntero("Ingrese el numero de DNI del ingresante a dar de alta (o 0 para cancelar):",&i.dni);
    if (i.dni == 0) {
        puts("Operacion cancelada. ");
        return;
    }
    while (buscarIngresanteBin(bin, i.dni,NULL)) { // <--- el NULL es para que no se rompa el buscar (en modif paso por direccion un ingresante)
        leerEntero("El numero de DNI ingresado ya se encuentra dado de alta, por favor ingrese otro DNI: ",&i.dni);
        if (i.dni == 0) {
            puts("Operacion cancelada. ");
            return;
        }
    }
    leerCadena("Ingrese el nombre completo del ingresante (APELLIDOS, NOMBRES): ",i.nombreCompleto);
    leerCadena("Ingrese la categoria del ingresante (PER, PRO, VIS, RES): ",i.categoriaIngreso);
    if (strcmpi(i.categoriaIngreso,"VIS") == 0 || strcmp(i.categoriaIngreso,"RES") == 0) {
        i.numTarjeta = 0;
        strcpy(i.fechaART,"");
    }
    else {
        leerEntero("Ingrese el numero de tarjeta a asignar: ",&(i.numTarjeta));
        leerCadena("Ingrese la fecha de ART (FORMATO AAAA-MM-DD): ", i.fechaART);
        while (!esFechaValida(i.fechaART)) {
            leerCadena("Por favor, ingrese una fecha de ART valida (FORMATO AAAA-MM-DD): ", i.fechaART);
        }
    }
    i.activo = true;
    printf("El ingresante a dar de alta es:\n");
    mostrarTitulo();
    mostrarActivos(i);
    fseek(bin,0L,SEEK_END);
    fwrite(&i,sizeof(Ingresante),1,bin);
}

void modifIngresante (FILE *f) {
    char c;
    int n;
    Ingresante i;
    system("cls");
    leerEntero("Ingrese el numero de DNI del ingresante a modificar (o 0 para cancelar): ",&n);
    if (n == 0) {
        puts("Operacion cancelada. ");
        return;
    }
    while (!buscarIngresanteBin(f,n,&i)) {
        leerEntero("El numero de DNI ingresado no se encuentra dado de alta, por favor ingrese otro DNI: ",&n);
        if (n == 0) {
            puts("Operacion cancelada. ");
            return;
        }
    }
    if  (!i.activo) {
        printf("El ingresante encontrado:\n");
        mostrarTituloCompleto();
        mostrarIngresanteCompleto(i);
        leerCaracter("se encuentra INACTIVO. Presione S si desea reactivarlo, o cualquier tecla para salir: ",&c);
        if (c == 'S' || c == 's') {
            i.activo = true;
            fseek(f,-1*(long long)sizeof(Ingresante),SEEK_CUR);
            fwrite(&i,sizeof(Ingresante),1,f);
            printf("El ingresante ha sido REACTIVADO.\n");
        }
        return;
    }
    printf("El ingresante a modificar es:\n");
    mostrarTitulo();
    mostrarActivos(i);
    printf("%cQu%c campo del ingresante desea modificar?\n"
           "(D para DNI, N para NOMBRE, C para CATEGORIA, T para TARJETA, F para FECHA ART): ",168,130);
    printf("\n(Para cancelar, ingrese cualquier otro caracter): ");
    leerCaracter("",&c);
    switch (c) {
        case 'D':
        case 'd':
                    leerEntero("Ingrese el nuevo DNI: ",&(i.dni));
                    break;
        case 'N':
        case 'n':
                    leerCadena("Ingrese el nuevo nombre: ",i.nombreCompleto);
                    break;
        case 'C':
        case 'c':
                    leerCadena("Ingrese la nueva categoria\n(PER, PRO, VIS, RES): ",i.categoriaIngreso);
                    if (strcmpi(i.categoriaIngreso,"PER") == 0 || strcmpi(i.categoriaIngreso,"PRO") == 0) {
                        leerEntero("Ingrese el numero de tarjeta a asignar: ",&(i.numTarjeta));
                        leerCadena("Ingrese la nueva fecha de ART (FORMATO AAAA-MM-DD): ",i.fechaART);
                        while (!esFechaValida(i.fechaART)) {
                            leerCadena("Por favor, ingrese una fecha de ART valida (FORMATO AAAA-MM-DD): ", i.fechaART);
        }               }
                    if (strcmpi(i.categoriaIngreso,"VIS") == 0 || strcmpi(i.categoriaIngreso,"RES") == 0) {
                        i.numTarjeta = 0;
                        strcpy(i.fechaART,"");
                    }
                    break;
        case 'T':
        case 't':
                    leerEntero("Ingrese el nuevo numero de tarjeta: ",&(i.numTarjeta));
                    break;
        case 'F':
        case 'f':
                    leerCadena("Ingrese la nueva fecha de ART (FORMATO AAAA-MM-DD): ",i.fechaART);
                    while (!esFechaValida(i.fechaART)) {
                        leerCadena("Por favor, ingrese una fecha de ART valida (FORMATO AAAA-MM-DD): ", i.fechaART);
                    }
                    break;
        default:
                    printf("Modificacion de ingresante cancelada.\n");
    }

    fseek(f,-1*(long long)sizeof(Ingresante),SEEK_CUR);
    fwrite(&i,sizeof(Ingresante),1,f);
}

void bajaIngresante(FILE *f) {
    char c;
    int n;
    Ingresante i;
    system("cls");
    leerEntero("Ingrese el numero de DNI del ingresante a dar de baja (o 0 para cancelar): ",&n);
    if (n == 0) {
        puts("Operacion cancelada. ");
        return;
    }
    while (!buscarIngresanteBin(f,n,&i)) {
        leerEntero("El numero de DNI ingresado no se encuentra dado de alta, por favor ingrese otro DNI: ",&n);
        if (n == 0) {
            puts("Operacion cancelada. ");
            return;
        }
    }
    printf("El ingresante a dar de baja es:\n");
    mostrarTitulo();
    mostrarActivos(i);
    printf("%cEst%c seguro de dar de baja al siguiente ingresante?\nPresione S para continuar, o cualquier otra tecla para cancelar: ",168,160);
    leerCaracter("",&c);
    switch (c) {
        case 'S':
        case 's':
                    i.activo = false;
                    break;
                    printf("Baja de ingresante cancelada.\n");
    }
    fseek(f,-1*(long long)sizeof(Ingresante),SEEK_CUR);
    fwrite(&i,sizeof(Ingresante),1,f);
}

bool buscarIngresanteBin(FILE *f, int dni, Ingresante *j) {
    bool flag = false;
    Ingresante i;
    rewind(f);
    while (fread(&i,sizeof(Ingresante),1,f)) {
        if (dni == i.dni) {
            flag = true;
            if (j) { // para no escribir dos funciones que basicamente harian lo mismo
                j ->dni = i.dni;
                strcpy(j->nombreCompleto,i.nombreCompleto);
                strcpy(j->categoriaIngreso,i.categoriaIngreso);
                j ->numTarjeta = i.numTarjeta;
                strcpy(j->fechaART, i.fechaART);
                j->activo = i.activo;
            }
            break;
        }
    }
    return flag;
}

void generarBinDesdeCsv (FILE *tex, FILE *bin) {
    char linea[81];
    Ingresante aux;
    if (!tex) {
        printf("No se encontr%c ingresantes.csv. Presione una tecla para continuar . . . ",162);
        getch();
        return;
    }
    rewind(tex);
    rewind(bin);
    fgets(linea,81,tex); /// descartar encabezado
    while (fgets(linea,81,tex)) {
        separarEnTokens(&aux,linea);
        fwrite(&aux,sizeof(Ingresante),1,bin);
    }
    printf("\nArchivo binario cargado con %cxito.\nPresione una tecla para continuar . . . ",130);
    getch();
}

void leerBinarioCompleto (FILE *f) {
    Ingresante i;
    system("cls");
    rewind(f);
    mostrarTituloCompleto();
    fread(&i,sizeof(Ingresante),1,f);
    mostrarIngresanteCompleto(i);
    while (fread(&i,sizeof(Ingresante),1,f)) {
        mostrarIngresanteCompleto(i);
    }
    printf("Cantidad de registros: %d", (int) ftell(f) / sizeof(Ingresante));
}

void leerBinario (FILE *f) {
    int cant = 0;
    Ingresante i;
    system("cls");
    rewind(f);
    mostrarTitulo();
    fread(&i,sizeof(Ingresante),1,f);
    mostrarActivos(i);
    if(i.activo) {
        cant++;
    }
    while (fread(&i,sizeof(Ingresante),1,f)) {
        mostrarActivos(i);
        if (i.activo) {
            cant++;
        }
    }
    printf("Cantidad de registros: %d", cant);
}

void mostrarOrdenado (FILE *f) {
    int i = calcularCantidadRegs(f), cant;;
    char campo, orden;
    Ingresante vec[i];
    system("cls");
    cargarVectorIngr(vec,f);
    printf("%cPor cu%cl campo del ingresante desea ordenar?\n"
           "(D para DNI, N para NOMBRE, C para CATEGORIA, T para TARJETA, F para FECHA ART): ",168,160);
    leerCaracter("",&campo);
    if (!(campo == 'D' || campo == 'N' || campo == 'C' || campo == 'T' || campo == 'F' ||
        campo == 'd' || campo == 'n' || campo == 'c' || campo == 't' || campo == 'f' )) {
        printf("No se eligi%c campo para ordenar.",162);
        return;
    }
    leerCaracter("\nPresione A para orden ASCENDENTE o D para orden DESCENDENTE: ",&orden);
    printf("\n");
    if (orden == 'a' || orden == 'A' || orden == 'd' || orden == 'D') {
        switch (campo) {
            case 'D':
            case 'd':
                        ordenarPorDNI(vec,i,orden);
                        cant = mostrarVectorIngr(vec,i);
                        break;
            case 'N':
            case 'n':
                        ordenarPorNombre(vec,i,orden);
                        cant = mostrarVectorIngr(vec,i);
                        break;
            case 'C':
            case 'c':
                        ordenarPorCateg(vec,i,orden);
                        cant = mostrarVectorIngr(vec,i);
                        break;
            case 'T':
            case 't':
                        ordenarPorTarj(vec,i,orden);
                        cant = mostrarVectorIngr(vec,i);
                        break;
            case 'F':
            case 'f':
                        ordenarPorFecha(vec,i,orden);
                        cant = mostrarVectorIngr(vec,i);
                        break;
        }
        printf("\nCantidad de registros: %d.", cant);
    }
    else {
        printf("No se eligi%c tipo de orden.",162);
    }
}

void cargarVectorIngr(Ingresante *vec, FILE *f) {
    int i = 0;
    Ingresante aux;
    rewind(f);
    fread(vec+i,sizeof(Ingresante),1,f);    //fread(&aux,sizeof(Ingresante),1,f); copiarIngresante(&(vec[i]),aux);
    i++;
    while (fread(vec+i,sizeof(Ingresante),1,f)) { //while (fread(&aux,sizeof(Ingresante),1,f)) { copiarIngresante(&(vec[i]),aux);
        i++;
    }
}

int mostrarVectorIngr (Ingresante *vec, int j) {
    int i, ord = 0;
    printf("%-4s ","ORD");
    mostrarTitulo();
    for (i = 0; i < j; i++) {
        if (vec[i].activo) {
            ord++;
            printf("%-4d ",ord);
        }
        mostrarActivos(vec[i]);
    }
    return ord;
}

void leerCsv (FILE *f) {
    char linea[81];
    system("cls");
    rewind(f);
    while (fgets(linea,81,f)) {
        printf("%s",linea);
    }
}

void separarEnTokens(Ingresante *ingr, char* st) {
    char *pri, *seg, *ter, *cua, *qui, stCua[10], stQui[11];
    pri = strtok(st,"\"");
    strtok(NULL,"\"");
    seg = strtok(NULL,"\"");
    strtok(NULL,"\"");
    ter = strtok(NULL,"\"");
    cua = strtok(NULL,",");
    if (strstr(cua,"NULL")) {
        ingr->numTarjeta = 0;
        strcpy(ingr->fechaART,"");
    }
    else {
        qui = strtok(NULL,",");
        reacomodar(cua, stCua,5);
        reacomodar(qui, stQui,10);
        ingr->numTarjeta = atoi(stCua);
        strcpy(ingr->fechaART,stQui);
    }
    ingr->dni = atoi(pri);
    strcpy(ingr->nombreCompleto,seg);
    strcpy(ingr->categoriaIngreso,ter);
    ingr->activo = true;
}

void reacomodar (char *src, char *dst, int rango) {
    int i;
    for (i = 0; i < rango; i++) {
        dst[i] = src[i+1];
    }
    dst[i] = '\0';
}

void mostrarIngresanteCompleto (Ingresante i) {
    if (i.activo) {
        if (i.numTarjeta) {
            printf("%-10d\t%-35s\t%-5s\t%-8d\t%-12s%-10s\n", i.dni, i.nombreCompleto, i.categoriaIngreso, i.numTarjeta, i.fechaART, "ACTIVO");
        }
        else {
            printf("%-10d\t%-35s\t%-5s\t%-8s\t%-12s%-10s\n", i.dni, i.nombreCompleto, i.categoriaIngreso, "NO TIENE", "NO TIENE", "ACTIVO");
        }
    }
    else {
        if (i.numTarjeta) {
            printf("%-10d\t%-35s\t%-5s\t%-8d\t%-12s%-10s\n", i.dni, i.nombreCompleto, i.categoriaIngreso, i.numTarjeta, i.fechaART, "INACTIVO");
        }
        else {
            printf("%-10d\t%-35s\t%-5s\t%-8s\t%-12s%-10s\n", i.dni, i.nombreCompleto, i.categoriaIngreso, "NO TIENE", "NO TIENE", "INACTIVO");
        }
    }
}

void mostrarActivos (Ingresante i) {
    if (i.activo) {
        if (i.numTarjeta) {
            printf("%-10d\t%-35s\t%-5s\t%-8d\t%-12s\n", i.dni, i.nombreCompleto, i.categoriaIngreso, i.numTarjeta, i.fechaART);
        }
        else {
            printf("%-10d\t%-35s\t%-5s\t%-8s\t%-12s\n", i.dni, i.nombreCompleto, i.categoriaIngreso, "NO TIENE", "NO TIENE");
        }
    }
}

void mostrarTitulo() {
    printf("%-10s\t%-35s\t%-5s\t%-8s\t%-12s\n","NRO DNI","NOMBRE COMPLETO","CATEG","NUM TARJ","FECHA ART");
}

void mostrarTituloCompleto() {
    printf("%-10s\t%-35s\t%-5s\t%-8s\t%-12s%-10s\n","NRO DNI","NOMBRE COMPLETO","CATEG","NUM TARJ","FECHA ART", "ESTADO");
}

int calcularCantidadRegs (FILE *f) {
    fseek(f,0L,SEEK_END);
    return (int) ftell(f)/sizeof(Ingresante);
}

void ordenarPorDNI (Ingresante *vec, int rango, char orden) { // rango = y
    int i, cota = rango - 1, k = 1;
    Ingresante aux;
    if (orden == 'D' || orden == 'd') { // descendente
        while (k) {
            k = 0;
            for (i = 0; i < cota ; i++){
                if (vec[i].dni < vec[i+1].dni) {
                    aux = vec[i];
                    vec[i] = vec[i+1];
                    vec[i+1] = aux;
                    k = i;
                }
            }
            cota = k;
        }
    }
    else { // ascendente
        while (k) {
            k = 0;
            for (i = 0; i < cota ; i++){
                if (vec[i].dni > vec[i+1].dni) {
                    aux = vec[i];
                    vec[i] = vec[i+1];
                    vec[i+1] = aux;
                    k = i;
                }
            }
            cota = k;
        }
    }
}

void ordenarPorNombre (Ingresante *vec,int rango, char orden) {
    int i, cota = rango - 1, k = 1;
    Ingresante aux;
    if (orden == 'D' || orden == 'd') { // descendente
        while (k) {
            k = 0;
            for (i = 0; i < cota ; i++){
                if (strcmpi(vec[i].nombreCompleto,vec[i+1].nombreCompleto) < 0) {
                    aux = vec[i];
                    vec[i] = vec[i+1];
                    vec[i+1] = aux;
                    k = i;
                }
            }
            cota = k;
        }
    }
    else { // ascendente
        while (k) {
            k = 0;
            for (i = 0; i < cota ; i++){
                if (strcmpi(vec[i].nombreCompleto,vec[i+1].nombreCompleto) > 0) {
                    aux = vec[i];
                    vec[i] = vec[i+1];
                    vec[i+1] = aux;
                    k = i;
                }
            }
            cota = k;
        }
    }
}

void ordenarPorCateg (Ingresante *vec, int rango, char orden) {
    int i, cota = rango - 1, k = 1;
    Ingresante aux;
    if (orden == 'D' || orden == 'd') { // descendente
        while (k) {
            k = 0;
            for (i = 0; i < cota ; i++){
                if (strcmpi(vec[i].categoriaIngreso,vec[i+1].categoriaIngreso) < 0) {
                    aux = vec[i];
                    vec[i] = vec[i+1];
                    vec[i+1] = aux;
                    k = i;
                }
            }
            cota = k;
        }
    }
    else { // ascendente
        while (k) {
            k = 0;
            for (i = 0; i < cota ; i++){
                if (strcmpi(vec[i].categoriaIngreso,vec[i+1].categoriaIngreso) > 0) {
                    aux = vec[i];
                    vec[i] = vec[i+1];
                    vec[i+1] = aux;
                    k = i;
                }
            }
            cota = k;
        }
    }
}

void ordenarPorTarj (Ingresante *vec, int rango,char orden) {
    int i, cota = rango - 1, k = 1;
    Ingresante aux;
    if (orden == 'D' || orden == 'd') { // descendente
        while (k) {
            k = 0;
            for (i = 0; i < cota ; i++){
                if (vec[i].numTarjeta < vec[i+1].numTarjeta) {
                    aux = vec[i];
                    vec[i] = vec[i+1];
                    vec[i+1] = aux;
                    k = i;
                }
            }
            cota = k;
        }
    }
    else { // ascendente
        while (k) {
            k = 0;
            for (i = 0; i < cota ; i++){
                if (vec[i].numTarjeta > vec[i+1].numTarjeta) {
                    aux = vec[i];
                    vec[i] = vec[i+1];
                    vec[i+1] = aux;
                    k = i;
                }
            }
            cota = k;
        }
    }
}

void ordenarPorFecha (Ingresante *vec, int rango, char orden) {
    int i, cota = rango - 1, k = 1;
    Ingresante aux;
    if (orden == 'D' || orden == 'd') { // descendente
        while (k) {
            k = 0;
            for (i = 0; i < cota ; i++){
                if (strcmpi(vec[i].fechaART,vec[i+1].fechaART) < 0) {
                    aux = vec[i];
                    vec[i] = vec[i+1];
                    vec[i+1] = aux;
                    k = i;
                }
            }
            cota = k;
        }
    }
    else { // ascendente
        while (k) {
            k = 0;
            for (i = 0; i < cota ; i++){
                if (strcmpi(vec[i].fechaART,vec[i+1].fechaART) > 0) {

                    aux = vec[i];
                    vec[i] = vec[i+1];
                    vec[i+1] = aux;
                    k = i;
                }
            }
            cota = k;
        }
    }
}

/** AUXILIARES **/

void leerCadena (char *msj, char *cad) {
    printf("%s",msj);
    fflush(stdin);
    scanf("%[^\n]",cad);
    fflush(stdin);
    return;
}

void leerCaracter (char *msj, char *c) {
    printf("%s",msj);
    fflush(stdin);
    scanf("%c",c);
    fflush(stdin);
    return;
}

void leerEntero (char *msj, int *i) {
    printf("%s",msj);
    fflush(stdin);
    scanf("%d",i);
    fflush(stdin);
    return;
}

void leerFloat (char *msj, float *f) {
    printf("%s",msj);
    fflush(stdin);
    scanf("%f",f);
    fflush(stdin);
    return;
}

bool esFechaValida (char *f) {
    int d, m, a, vm[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    char *pri, *seg, *ter, fecha[15];
    strcpy(fecha,f);
    pri = strtok(fecha,"-");
    seg = strtok(NULL,"-");
    ter = strtok(NULL,"-");
    a = atoi(pri);
    m = atoi(seg);
    d = atoi(ter);
    if ((a % 4 == 0) && ((a % 400 == 0) || (a % 100 != 0))) { // mult 4 y (no mult100 o mult400)
        vm[1] = 29;
    }
    return (0 <= d && d <= vm[m-1] && m - 1 < 12 );
}

int pausa (char *msj) {
    printf("%s",msj);
    return getch();
}

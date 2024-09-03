#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Numero maximo de aeroportos e voos */ 
#define MAX_AERO 40
#define MAX_VOOS 30000

/* Numero maximo de caracteres */
#define CIDADE_MAX 51
#define PAIS_MAX 31
#define ID_MAX 4
#define CODVOO_MAX 7
#define BUFFER_MAX 65536

/* Capacidade maxima e minima inclusive e duracao maxima*/
#define CAP_MIN 10
#define DUR_MAX 12

/* Data maxima */
#define ULTIMO_DIA 31
#define ULTIMO_MES 12
#define ULTIMO_ANO 2023

#define TRUE 1
#define FALSE 0

#define PARTIDA 1
#define CHEGADA 0

typedef struct { /* Estrutura de Data */
    int dia, mes, ano;
} Data;

typedef struct { /* Estrutura de Horário */
    int hora, minuto;
} Hora;

typedef struct { /* Estrutura de Aeroportos */
    char cidade[CIDADE_MAX];
    char pais[PAIS_MAX];
    char id[ID_MAX];
    int n_voos;
} Aeroporto;

typedef struct { /* Estrutura de Voos */
    int capacidade;
    char codigoVoo[CODVOO_MAX], IDpartida[ID_MAX], IDchegada[ID_MAX];
    Data data;
    Hora horario;
    Hora duracao;
    int *n_voos;
} Voo;

typedef struct node { /* Estrutura de Reservas */
int numPassageiros;
Data data;
char codigoVoo[CODVOO_MAX];
char *codigoReserva;
struct node *next;
} * Reserva;

/* Variaveis Globais */

int numAeroportos = 0;		    /* número de aeroportos existentes */
Aeroporto aeroportos[MAX_AERO];	/* vetor de aeroportos */

int numVoos = 0		/* número de voos existentes */;
Voo voos[MAX_VOOS];	/* vetor de voos */

Data hoje = { 1, 1, 2022 };	/* data atual do sistema */
int meses[12] = {31,28,31,30,31,30,31,31,30,31,30,31}; /* tabela com os dias por mês */

Reserva head; 


/* Verificação de datas */

#define hora_menor(a,b) ((a.horario.hora*100 + a.horario.minuto) < (b.hora*100 + b.minuto))
#define data_menor(a,b) ((a.data.ano*10000 + a.data.mes*100 + a.data.dia) < (b.ano*10000 + b.mes*100 + b.dia))
#define data_igual(a,b) ((a.data.ano*10000 + a.data.mes*100 + a.data.dia) == (b.ano*10000 + b.mes*100 + b.dia))

#define eh_anterior(a,b) (data_menor(a,b.data) || (data_igual(a,b.data) && hora_menor(a,b.horario)))
#define eh_futuro(a) ((a.ano*10000 + a.mes*100 + a.dia) > ((hoje.ano+1)*10000 + hoje.mes*100 + hoje.dia))
#define eh_passado(a) ((a.ano*10000 + a.mes*100 + a.dia) < (hoje.ano*10000 + hoje.mes*100 + hoje.dia))

#define eh_duracao_superior(a) ((a.duracao.hora*100 + a.duracao.minuto) > (DUR_MAX*100))


/* COMANDO 'a' */
/* adiciona um aeroporto ao sistema */
void adiciona_aeroporto() {
    int n, okay=1;
    Aeroporto aux; /* aeroporto auxiliaur */

    /* <IDAeroporto> <país> <cidade> */
    scanf("%s %s %51[^\n]", aux.id, aux.pais, aux.cidade);

    for (n = 0; aux.id[n] != '\0'; n++) { 
        /* identificador de aeroporto não ser
        uma string apenas com maíusculas */
        if (!(aux.id[n] >= 'A' && aux.id[n] <= 'Z')) {  
            okay=0;    
            printf("invalid airport ID\n");
            break;
        }
    }
    if (numAeroportos == MAX_AERO) {
        /* se o numero de aeroportos exceder o 
        limite máximo permitidos pelo sistema */
        okay=0;
        printf("too many airports\n");
    }
    for (n = 0; n < numAeroportos; n++) {
        /* se já existir um aeroporto com o mesmo ID*/
        if (strcmp(aeroportos[n].id, aux.id) == 0) {
            okay=0;
            printf("duplicate airport\n");
            break;
        }
    }
    if (okay) {
        /* após verificar guarda no sistema */
        strcpy(aeroportos[numAeroportos].id, aux.id);
		strcpy(aeroportos[numAeroportos].pais, aux.pais);
		strcpy(aeroportos[numAeroportos].cidade, aux.cidade);
        aeroportos[numAeroportos].n_voos = 0;
        numAeroportos++;
        printf("airport %s\n", aux.id);
    }
}


/* COMANDO 'l\n' */
/* lista todos os aeroportos por ordem alfabética */
void lista_alfabetica() {
    int i, j, done;
    char str[ID_MAX], ids[MAX_AERO][ID_MAX];  /* vetor auxiliar de IDs */

    for (i = 0; i < numAeroportos; i++) { /* copia IDs do sistema*/
        strcpy(ids[i],aeroportos[i].id);
    }
    for (i = 0; i < numAeroportos; i++) { /* bubblesort de IDs*/
        done = 1;
        for (j = 0; j < numAeroportos-i-1; j++) {
            if (strcmp(ids[j],ids[j+1])>0) {
                done = 0;
                strcpy(str,ids[j]);
                strcpy(ids[j],ids[j+1]);
                strcpy(ids[j+1],str);
            }
        }
        if (done)
            break;
    }
    for (i = 0; i < numAeroportos; i++) {
        for (j = 0; j < numAeroportos; j++) {
            /*lista os aeroportos pelo ordem do vetor */
            if (strcmp(aeroportos[j].id, ids[i]) == 0)
                printf("%s %s %s %d\n", aeroportos[j].id, 
                       aeroportos[j].cidade, aeroportos[j].pais, 
                       aeroportos[j].n_voos);    
        }
    }
}


/* COMANDO 'l' */
/* lista os aeroportos */
void lista() {
    int i, c, okay = FALSE;
    char str[ID_MAX];

    do {
        /* [<IDAeroporto> <IDAeroporto> ...] */
        scanf("%s", str);

        for (i = 0; i < numAeroportos; i++) {
            if (strcmp(aeroportos[i].id, str) == 0) {
                okay = TRUE;
                /* <IDAeroporto> <cidade> <país> #voosPartida */
                printf("%s %s %s %d\n", aeroportos[i].id, 
                       aeroportos[i].cidade, aeroportos[i].pais, 
                       aeroportos[i].n_voos);
                break;
            }
        }
        if (okay == FALSE) { /*no caso de não existir o aeroporto indicado*/
            printf("%s: no such airport ID\n", str);
        }
    } while ((c = getchar()) != '\n'); /* pára função no final da linha*/
}

int eh_codigoVoo(char* cod) {
    int i = 0, l = strlen(cod);
	if (l < 3)
		return FALSE;
	for (i = 0; i < 2; i++)
		if (!(cod[i] >= 'A' && cod[i] <= 'Z'))
			return FALSE;

	while (cod[i] != '\0') {
		if (!(cod[i] >= '0' && cod[i] <= '9'))
			return FALSE;
		i++;
	}
	return TRUE;
}

/* COMANDO 'v' */
/* adiciona um voo ao sistema */
void adiciona_voo() {
    Voo aux; /* vetor do voo a adicionar */
    int i, done, okay=1, a;

    scanf("%s %s %s", aux.codigoVoo, aux.IDpartida, aux.IDchegada);
    scanf("%02d-%02d-%d %d:%d", &aux.data.dia, 
          &aux.data.mes, &aux.data.ano,
          &aux.horario.hora, &aux.horario.minuto);
    scanf("%d:%d %d", &aux.duracao.hora, &aux.duracao.minuto, &aux.capacidade);

    /* numero de voos existentes tiver excidido o maximo */
    if (numVoos == MAX_VOOS) {
        printf("too many flights\n");
        okay=0;
    }
    /* as duas letras do codigo devem ser maiusculas */
    else if (eh_codigoVoo(aux.codigoVoo) == FALSE) { 
        printf("invalid flight code\n");
        okay=0;
    }
    /* data no passado ou mais de um ano no futuro */
    else if (eh_passado(aux.data) || eh_futuro(aux.data)) {
        printf("invalid date\n");
        okay=0;
    }
    /* duração de voo superior a 12 horas */
    else if (eh_duracao_superior(aux)) {
        printf("invalid duration\n");
        okay=0;
    }
    /* capacidade não for entre 10 e 100 passageiros */
    else if (!(aux.capacidade >= CAP_MIN)) {
        printf("invalid capacity\n");
        okay=0;
    }
    /* verifica se existe o aeroporto (partida ou chegada) indicado */
    for (i = 0; i < numAeroportos; i++) {
        done = 1;
        if (strcmp(aeroportos[i].id, aux.IDchegada) == 0) {
            done = 0;
            break;
        }
    }
    if (done) {
        printf("%s: no such airport ID\n", aux.IDchegada);
        okay=0;
    }
    for (i = 0; i < numAeroportos; i++) {
        done = 1;
        if (strcmp(aeroportos[i].id, aux.IDpartida) == 0) {
            done = 0;
            a = i;
            break;
        }
    }
    if (done) {
        printf("%s: no such airport ID\n", aux.IDpartida);
        okay=0;
    }
    /* verifica se existe um voo com o mesmo código para o mesmo dia*/
    for (i = 0; i < numVoos; i++) {
        if ((strcmp(voos[i].codigoVoo, aux.codigoVoo) == 0) && data_igual(voos[i],aux.data)) {
            printf("flight already exists\n");
            okay=0;
            break;
        }
    }
    if (okay) {
        /* após verificar adiciona ao sistema */
        strcpy(voos[numVoos].codigoVoo, aux.codigoVoo);
        strcpy(voos[numVoos].IDpartida, aux.IDpartida);
        strcpy(voos[numVoos].IDchegada, aux.IDchegada);
        voos[numVoos].data = aux.data;
        voos[numVoos].horario = aux.horario;
        voos[numVoos].duracao = aux.duracao;
        voos[numVoos].capacidade = aux.capacidade;
        aeroportos[a].n_voos++;
        voos[numVoos].n_voos = &aeroportos[a].n_voos;
        numVoos++;
    }
}


/* COMANDO 'v\n' */
/* lista todos os voos pela ordem de criação */
void lista_voos() {
    int i;
    
    for (i = 0; i < numVoos; i++) {
        /*<códigoVoo> <IDAeroportoPartida> <IDAeroportoChegada>*/ 
        printf("%s %s %s ", voos[i].codigoVoo, 
               voos[i].IDpartida, voos[i].IDchegada);
        /*<dataPartida> <horaPartida>*/
        printf("%02d-%02d-%d %02d:%02d\n", voos[i].data.dia, 
               voos[i].data.mes, voos[i].data.ano, 
               voos[i].horario.hora, voos[i].horario.minuto);
    }
}


/* Função auxiliar que ordena datas */
/* recebe vetor auxiliar ord com voos
e o numero de voos de partida ou chegada*/
void ordena_datas(Voo ord[], int n, int partida) {
    int i, j, done;
    Voo aux; 

    for (i = 0; i < n; i++) { /*Bubblesort de datas*/
        done = 1;
        for (j = 0; j < n-i-1; j++) {
            if (eh_anterior(ord[j+1],ord[j])) {
                done = 0;
                aux = ord[j+1];
                ord[j+1] = ord[j];
                ord[j] = aux;
            } 
        }
        if (done) 
                break;
    }
    /* output das partidas */
    if (partida)
        for (i = 0; i < n; i++) {
            printf("%s %s %02d-%02d-%d ", ord[i].codigoVoo, ord[i].IDchegada,
            ord[i].data.dia, ord[i].data.mes, ord[i].data.ano);
            printf("%02d:%02d\n", ord[i].horario.hora, ord[i].horario.minuto);
        }
    /* output das chegadas */
    else
        for (i = 0; i < n; i++) {
            printf("%s %s %02d-%02d-%d ", ord[i].codigoVoo, ord[i].IDpartida,
            ord[i].data.dia, ord[i].data.mes, ord[i].data.ano);
            printf("%02d:%02d\n", ord[i].horario.hora, ord[i].horario.minuto);
        }
}


/* COMANDO 'p' */
/* lista os voos com partida de um aeroporto */
/* recebe todos os voos e aeroportos e o numero de aeroportos exixtentes */
void lista_partidas() {
    int i, j=0, done, n;
    char IDpartida[ID_MAX];
    Voo ord[MAX_VOOS];

    /* <IDAeroporto> */
    scanf("%s", IDpartida);

    /* verificar se existe o aeroporto indicado 
    e obter o numero de partidas do mesmo */
    for (i = 0; i < numAeroportos; i++) {
        done = 1;
        if (strcmp(aeroportos[i].id, IDpartida) == 0) {
            n = aeroportos[i].n_voos;
            j = n;
            done = 0;
            break;
        }
    }
    if (done) 
        printf("%s: no such airport ID\n", IDpartida);
    else {
        /* copia voos de partida para o vetor auxiliar*/
        for (i = 0; i < numVoos; i++) {
            if (strcmp(voos[i].IDpartida, IDpartida) == 0) {
                j--;
                ord[j] = voos[i];
            }
            if (j == 0)
                break;
        }
    ordena_datas(ord,n,PARTIDA);
    }
}


/* COMANDO 'c' */
/* lista os voos com chegada a um aeroporto */
void lista_chegadas() {
    int i, done, n=0, horas, minutos;
    char IDchegada[ID_MAX];
    Voo ord[MAX_VOOS];
    Data aux;

    /* <IDAeroporto> */
    scanf("%s", IDchegada);

    /* verificar se existe o aeroporto indicado */
    for (i = 0; i < numAeroportos; i++) {
        done = 1;
        if (strcmp(aeroportos[i].id, IDchegada) == 0) {
            done = 0;
            break;
        }
    }
    if (done)
        printf("%s: no such airport ID\n", IDchegada);
    else {
        /* calcular data e hora de chegada */
        for (i = 0; i < numVoos; i++) {
            if (strcmp(voos[i].IDchegada, IDchegada) == 0) {
                ord[n] = voos[i];

                minutos = (voos[i].duracao.minuto + voos[i].horario.minuto);
                horas = (minutos/60 + voos[i].duracao.hora + voos[i].horario.hora);

                ord[n].horario.minuto = (minutos%60);
                ord[n].horario.hora = (horas%24);

                aux = voos[i].data;
                aux.dia = aux.dia + (horas/24);
                /* mudança de mes */
                if (meses[aux.mes-1] < aux.dia) {
                    aux.dia = 1;
                    aux.mes++;
                }
                /* mudança de ano */
                if (aux.mes > ULTIMO_MES) {
                    aux.mes = 1;
                    aux.ano++;
                }
                ord[n].data = aux;
                n++;
            }
        }
    ordena_datas(ord,n,CHEGADA);
    }
}

/* COMANDO 't' */
/* avança a data do sistema */
void avanca() {
    Data aux;

    /* <DD-MM-AAAA> */
    scanf("%02d-%02d-%4d", &aux.dia, &aux.mes, &aux.ano);

    /* data no passado ou mais de um ano no futuro */
    if (eh_passado(aux) || eh_futuro(aux) || 
    (aux.ano > ULTIMO_ANO || aux.dia > ULTIMO_DIA || aux.mes > ULTIMO_MES)) {
        printf("invalid date\n");
    }
    else {
        hoje = aux;
        printf("%02d-%02d-%4d\n", hoje.dia, hoje.mes, hoje.ano);
    }
}



/* Funções auxiliares 'r' */


Reserva NEW(char* codRes, Data d, int n, char codVoo[]) {
    Reserva x = (Reserva) malloc(sizeof(struct node));
    x->codigoReserva = (char*) malloc(sizeof(char)*(strlen(codRes)+1));
    
    strcpy(x->codigoReserva, codRes);
    x->data = d;
    x->numPassageiros = n;
    strcpy(x->codigoVoo, codVoo);
    
    return x;
}

char *criaReserva(char b[]) {
    int i;
    char *codRes;

    if (strlen(b) < 10)
        return NULL;
    for(i = 0; b[i] != '\0'; i++)
        if (!((b[i] >= 'A' && b[i] <= 'Z') || 
        (b[i] >= '0' && b[i] <= '9')))
            return NULL;
    
    codRes = (char*) malloc(sizeof(char)*(strlen(b)+1));
    strcpy(codRes, b);
    return codRes;
}

Reserva procuraReserva(char* cod) {
    Reserva x;
    for(x = head; x != NULL; x = x->next)
        if(strcmp(x->codigoReserva, cod) == 0)
            return x;
    return NULL;
}

int verificaVoo(char cod[], Data d) {
    int i;
    for (i = 0; i < numVoos; i++) {
            if (strcmp(voos[i].codigoVoo, cod) == 0 && 
            data_igual(voos[i],d)) {
                return TRUE;
            }
    }
    return FALSE;
}

int verificaCapacidade(int n, char cod[], Data d, char* codRes) {
    int i;
    int total = n;
    Reserva x;
    for(x = head; x != NULL; x = x->next) {
        if(strcmp(x->codigoVoo, cod) == 0 &&
        data_igual((*x),d))
            total += x->numPassageiros;
        else if(strcmp(x->codigoReserva, codRes) == 0) {
            printf("%s: flight reservation already used", codRes);
            return FALSE;
        }
    }
    for (i = 0; i < numVoos; i++) {
        if (strcmp(voos[i].codigoVoo, cod) == 0 && 
        data_igual(voos[i],d) && total <= voos[i].capacidade) {
            return TRUE;
        }
    }
    printf("too many reservations");
    return FALSE;
}

void lista_reservas(char cod[], Data d) {
    Reserva x;
    for(x = head; x != NULL; x = x->next)
        if (strcmp(x->codigoVoo, cod) == 0 && data_igual((*x),d))
            printf("%s %d\n", x->codigoReserva, x->numPassageiros);
}

void insere_ordenado(Reserva new) {
    int okay = FALSE;
    Reserva x, y;
    if (head == NULL) {
        new->next = head;
        head = new;
    }
    else {
        for(x = head, y = NULL; x != NULL; y = x, x = x->next) {
            if (strcmp(x->codigoReserva,new->codigoReserva) > 0) {
                if (x == head) {
                    new->next = head;
                    head = new;
                }
                else {
                    new->next = y->next;
                    y->next = new;
                }
                okay=TRUE;
                break;
            }
        }
        if (okay == FALSE) {
            new->next = y->next;
            y->next = new;
        }
    }
}

/* COMANDO 'r' */
/* adiciona uma reserva ou lista as reserva de um voo */
void adiciona_reserva(){
    int numPassageiros, c;
    Data data;
    Reserva new;
    char codigoVoo[CODVOO_MAX];
    char buffer[BUFFER_MAX], *codigoReserva;

    /* <códigoVoo> <data> */
    scanf("%s %d-%d-%d", codigoVoo, &data.dia, &data.mes, &data.ano);
    if (verificaVoo(codigoVoo,data) == FALSE)
        printf("%s: flight does not exist\n", codigoVoo);
    else if (eh_passado(data) || eh_futuro(data))
        printf("invalid date\n");

    else if ((c = getchar()) != '\n') {
        scanf("%s %d", buffer, &numPassageiros);
        codigoReserva = criaReserva(buffer);

        if (codigoReserva == NULL)
            printf("invalid reservation code\n");
        else if (numPassageiros <= 0)
            printf("invalid passenger number\n");
        else if (verificaCapacidade(numPassageiros,codigoVoo,data,codigoReserva) == FALSE)
            printf("\n");
        else {
            new = NEW(codigoReserva, data, numPassageiros, codigoVoo);
            insere_ordenado(new);
        }   
        free(codigoReserva);
    }
    else
        lista_reservas(codigoVoo,data);
}



/* Funcões auxiliares 'e' */

void apagaReserva(char* cod) {
    int done=FALSE;
    Reserva t, prev;
    for(t = head, prev = NULL; t != NULL;
        prev = t, t = t->next) {
        if(strcmp(t->codigoReserva, cod) == 0) {
            done=TRUE;
            if(t == head) {head = t->next;} 
            else {prev->next = t->next;}
            free(t->codigoReserva);
            free(t);
            break;
        }
    }
    if (done == FALSE)
        printf("not found\n");
}

void apagaVoos(char* cod) {
    int i, j, n, okay=FALSE;
    Reserva x, t, prev;
    for (i = 0; i < numVoos; i++) {
        if (strcmp(voos[i].codigoVoo, cod) == 0) {
            okay=TRUE;
            n = *voos[i].n_voos;
            *voos[i].n_voos = n-1;
            for (j = i; j < numVoos-1; j++)
                voos[j] = voos[j+1];
            numVoos--;
            i--;
        }
    }
    if (okay == TRUE)
        for(t = head, prev = NULL; t != NULL;) {
            if(strcmp(t->codigoVoo, cod) == 0) {
                x = t;
                if(t == head) {head = t->next;} 
                else {prev->next = t->next;}
                t = t->next;
                free(x->codigoReserva);
                free(x);
            }    
            else {
                prev = t;
                t = t->next;
            }
        }
    else
        printf("not found");
}


/* COMANDO 'e' */
/* elimina voos ou reserva */
void elimina_codigo() { 
    char buffer[BUFFER_MAX], *cod;

    scanf("%s", buffer);
    cod = (char*) malloc(sizeof(char)*strlen(buffer)+1);
    strcpy(cod,buffer);

    if (eh_codigoVoo(cod) == TRUE)
        apagaVoos(cod);
    else
        apagaReserva(cod);
    free(cod);
}


/* COMANDO 'q' */
void liberta() {
    Reserva x = head;
    head = x->next;
    free(x->codigoReserva);
    free(x);
}

int tamanho(Reserva head) {
    int count = 0;
    Reserva x;
    for(x = head; x != NULL; x = x->next)
        count++;
    return count;
}


int main() {
    int next, t, f;
    char command; 

    /* comando inserido no input */
    while (1) {
        command = getchar();
        switch (command) {
        case 'a': /*adiciona um novo aeroporto ao sistema*/
            adiciona_aeroporto();
            break;
        case 'l':
            if ((next = getchar()) == '\n')
                lista_alfabetica();
            else
                lista();
            break;
        case 'v':
            if ((next = getchar()) == '\n')
                lista_voos();
            else
                adiciona_voo();
            break;
        case 'p':
            lista_partidas();
            break;
        case 'c':
            lista_chegadas();
            break;
        case 't':
            avanca();
            break;
        case 'r':
            adiciona_reserva();
            break;
        case 'e':
            elimina_codigo();
            break;
        case 'q':
            f = tamanho(head);
            for (t=0; t < f; t++)
                liberta();
            return 0; /*termina o programa*/
        } 
    }
}
#include <stdio.h>
#include "KmsFncs.h"
static def_dtm_shift *find_1_step(int dtm1, int dtm2,def_dtm_shift *datum_shifts, int n_shifts);
static int find_2_steps(int dtm1, int dtm2,def_dtm_shift **o_shift1,def_dtm_shift **o_shift2, def_dtm_shift *datum_shifts, int n_shifts);

#ifdef BOMBJACK
typedef void* GRIM;
typedef void  PR;
typedef struct{
	int dno1;
	int dno2;
	char *mlb1; 
	char *mlb2;
	double len; /*a weight*/
	int n_tabs;
	GRIM *g;  /*a sequence of methods - possibly spanning different areas...*/
} def_dtm_shift;

typedef struct {
	int n_shifts;
	def_dtm_shift *shifts[4];
	int direction[4];
	double len;
	PR *clab;
} htr_route;


typedef struct {
	char *mlb;
	int no;
} dtm;


dtm DATUMS[]={
	{"E",200},{"dvr90",201},{"msl",202},{"dnn",203},{"pop",204},{"hop",205},{"koko",206},{"choko",207},{"egm",208},{"fvr10",209}};

def_dtm_shift SHIFTS[]={
	{203,201,"dnn","dvr90",1,0,NULL},{201,204,"dvr90","pop",1,0,NULL},{204,206,"pop","choko",1.0,NULL},{208,206,"egm","koko",1,0,NULL},{209,205,"fvr10","hop",1,0,NULL},
	{200,201,"E","dvr90",1.5,0,NULL},{200,208,"E","egm",1.5,0,NULL},{200,209,"E","fvr10",1.5,0,NULL},{200,202,"E","msl",1.5,0,NULL}};


#endif

static def_dtm_shift *find_1_step(int dtm1, int dtm2,def_dtm_shift *datum_shifts, int n_shifts){
	def_dtm_shift *shift;
	int i;
	#ifdef BOMBJACK
	printf("*** one step called with: %d to %d\n",dtm1,dtm2);
	#endif
	for(i=0; i<n_shifts; i++){
		shift=datum_shifts+i;
		if ((shift->dno1==dtm1 && shift->dno2==dtm2) || (shift->dno1==dtm2 && shift->dno2==dtm1))
			return shift;
	}
	return NULL;
}




	
/*will find the FIRST route, if more exist the ordering of dtm_shifts is important!! Geoid dtm_shifts should be 'longer' than dh_shifts*/
static int find_2_steps(int dtm1, int dtm2,def_dtm_shift **o_shift1,def_dtm_shift **o_shift2, def_dtm_shift *datum_shifts, int n_shifts){
	int i,j,vertex;
	def_dtm_shift *shift1,*shift2=NULL;
	#ifdef BOMBJACK
	printf("*** two steps called with: %d to %d\n",dtm1,dtm2);
	#endif
	for(i=0; i<n_shifts && shift2==NULL; i++){
		shift1=datum_shifts+i;
		vertex=-1;
		if (shift1->dno1==dtm1){
			vertex=shift1->dno2;
		}
		else if (shift1->dno2==dtm1){
			vertex=shift1->dno1;
		}
		if (vertex==-1)
			continue;
		/*intermediate vertex found, we could also assume it's not dtm2, as we might already have found it by applying find_1_step...*/
		if (vertex==dtm2){
			shift2=shift1;
			#ifdef BOMBJACK
			puts("Found a 1-link connection!");
			printf("vertex: %d\n",vertex);
			printf("i: %d, shift: %d to %d, %s to %s\n",i,shift1->dno1,shift1->dno2,shift1->mlb1,shift1->mlb2);
			#endif
		}
		else {
			#ifdef BOMBJACK
			printf("Found possible vertex: %d\n",vertex);
			#endif
			shift2=find_1_step(vertex,dtm2,datum_shifts,n_shifts);
		}
	}
	if (shift2!=NULL){ /*found*/
		*o_shift1=shift1;
		*o_shift2=shift2;
		return 0;
	}
	*o_shift1=NULL;
	*o_shift2=NULL;
	return 1;
}


int find_htr_route(int dtm1,int dtm2,htr_route *route,def_dtm_shift *shifts, int n_shifts){
	def_dtm_shift *shift1=NULL,*shift2=NULL,*shift3=NULL,*shift4=NULL;
	route->n_shifts=0;
	route->len=0;
	if (dtm1==dtm2){
		#ifdef BOMBJACK
		puts("Equal datums...");
		#endif
		return 0; /*no brainer*/
	}
	shift1=find_1_step(dtm1,dtm2,shifts,n_shifts);
	if (shift1!=NULL){
		route->shifts[0]=shift1;
		route->n_shifts=1;
		route->direction[0]=(shift1->dno1==dtm1)?1: -1;
		route->len=shift1->len;
		return 0;
	}
	if (find_2_steps(dtm1,dtm2,&shift1,&shift2,shifts,n_shifts)==0){
		#ifdef BOMBJACK
		printf("In two gos from %d to %d...\n",dtm1,dtm2);
		#endif
		route->n_shifts=2;
		route->shifts[0]=shift1;
		route->shifts[1]=shift2;
		route->direction[0]=(shift1->dno1!=dtm1)?1:-1;
		route->direction[1]=(shift2->dno2!=dtm2)?1:-1;
		route->len=shift1->len+shift2->len;
		return 0;
	}
	if ( dtm1==200 || dtm2==200){
			return 1;
	}
	/*if not found check if we can go via 'ground'==E */
	if (find_2_steps(200,dtm1,&shift1,&shift2,shifts,n_shifts)==0 && find_2_steps(200,dtm2,&shift3,&shift4,shifts,n_shifts)==0){
		/*dtm1 to E*/
		route->shifts[0]=shift2;
		route->direction[0]=(shift2->dno1==dtm1)?1:-1;
		route->n_shifts=1;
		route->len+=shift2->len;
		if (shift2!=shift1){
			route->shifts[1]=shift1;
			route->direction[1]=(shift1->dno2==200)?1:-1;
			route->n_shifts=2;
			route->len+=shift1->len;
		}
		/*E to dtm2*/
		route->shifts[route->n_shifts]=shift3;
		route->direction[route->n_shifts]=(shift3->dno1==200)?1:-1;
		route->n_shifts++;
		route->len+=shift3->len;
		if (shift3!=shift4){
			route->shifts[3]=shift4;
			route->direction[3]=(shift4->dno2==dtm2)?1:-1;
			route->n_shifts++;
			route->len+=shift4->len;
		}
		return 0;
	}		
	
	return 1;
	
}

#ifdef BOMBJACK

int main(int argc, char **argv){
	int i,j,res,N_DTM,N_SHIFTS;
	htr_route route;
	N_DTM=sizeof(DATUMS)/sizeof(dtm);
	N_SHIFTS=sizeof(SHIFTS)/sizeof(def_dtm_shift);
	i=atoi(argv[1]);
	j=atoi(argv[2]);
	res=find_htr_route(i,j,&route,SHIFTS,N_SHIFTS);
	if (res==0){
		printf("Found route of length: %.2f\n",route.len);
		for(i=0; i<route.n_shifts; i++)
			printf("From %s to %s, direction: %d\n",route.shifts[i]->mlb1,route.shifts[i]->mlb2,route.direction[i]);
		
	}
	else
		printf("No route from %d to %d",i,j);
	return 0;
}
#endif
	

	
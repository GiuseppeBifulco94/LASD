#include "func.h"
#include "utility.h"
#include "grafo.h"

void setlib(struct func *funz, int impl_grafo, int tipo_el) {
	funz->impl_grafo = impl_grafo;
	funz->tipo_el = tipo_el;
	if(impl_grafo == 1) { //liste
		funz->initAdj = &init_list_adj;
		funz->init = &init_list_grafo;
		funz->getMappa = &getmappa_list;
		funz->getAdj = &mappa_list_edge;
		funz->getPeso = &getpeso_list;
		funz->printAllAdj = &stampa_list_adj;
		funz->printAllVertex = &stampa_list_vertici;
		funz->addVertex = &add_list_vertex;
		funz->addEdge = &add_list_edge;
		funz->removeVertex = &remove_list_vertex;
		funz->removeEdge = &remove_list_edge;
		funz->trasposta = &trasposta_list;
		funz->convertiRappresentazione = &converti_list_to_matrix;
		funz->deallocaAdj = &dealloca_list_adj_complete;
	} else { //matrice
		funz->initAdj = &init_matrix_adj;
		funz->init = &init_matrix_grafo;
		funz->getMappa = &getmappa_matrix;
		funz->getAdj = &mappa_matrix_edge;
		funz->getPeso = &getpeso_matrix;
		funz->printAllAdj = &stampa_matrix_adj;
		funz->printAllVertex = &stampa_matrix_vertici;
		funz->addVertex = &add_matrix_vertex;
		funz->addEdge = &add_matrix_edge;
		funz->removeVertex = &remove_matrix_vertex;
		funz->removeEdge = &remove_matrix_edge;
		funz->trasposta = &trasposta_matrix;
		funz->convertiRappresentazione = &converti_matrix_to_list;
		funz->deallocaAdj = &dealloca_matrix_adj;
	}
	switch(tipo_el) {
		case 0:
		default:
			funz->getInput = &getinputstringa;
			funz->print = &stampastringa;
			funz->cmp = &cmpstr;
			funz->libera = &dealloca_oggetto;
			funz->fprintf = &fprintfstringa;
			funz->stringToType = &string_to_string;
			funz->checkType = &is_string;
			funz->allocaval = &allocastringa;
		break;
		case 1:
			funz->getInput = &getinputintero;
			funz->print = &stampaintero;
			funz->cmp = &cmpint;
			funz->libera = &dealloca_oggetto;
			funz->fprintf = &fprintfintero;
			funz->stringToType = &string_to_int;
			funz->checkType = &is_int;
			funz->allocaval = &allocaintero;
		break;
		case 2:
			funz->getInput = &getinputdouble;
			funz->print = &stampadouble;
			funz->cmp = &cmpdouble;
			funz->libera = &dealloca_oggetto;
			funz->fprintf = &fprintfdouble;
			funz->stringToType = &string_to_double;
			funz->checkType = &is_double;
			funz->allocaval = &allocadouble;
		break;
		case 3:
			funz->getInput = &getinputpersona;
			funz->print = &stampapersona;
			funz->cmp = &cmppers;
			funz->libera = &dealloca_persona;
			funz->fprintf = &fprintfpersona;
			funz->stringToType = &string_to_persona;
			funz->checkType = &is_persona;
			funz->allocaval = &allocapersona;
		break;
	}
}
void switch_func(struct func *funz) {
	funz->impl_grafo = funz->impl_grafo == 1 ? 0 : 1;
	setlib(funz, funz->impl_grafo, funz->tipo_el);
}
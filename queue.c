#include <stdlib.h>
#include <stdio.h>

#include "beep.h"
#include "queue.h"


void q_push(q_node **queue, Beep *beep) {
	q_node* newnode;
	newnode = (q_node*)malloc(sizeof(q_node));
	newnode->next = NULL;
	newnode->beep = beep;

	if (*queue == NULL) {
		*queue = newnode;	
	} else {
		q_node *aux = *queue;
		while (aux->next != NULL) {aux = aux->next;}
		aux->next = newnode;
	}
}

Beep* q_peek(q_node *queue) {
	if (queue == NULL) {
		return NULL;
	}

	return queue->beep;
}

Beep* q_pop(q_node **queue) {
	if (*queue == NULL) {
		return NULL;
	}

	q_node *poped = (*queue);

	*queue = (*queue)->next;

	Beep *beep = poped->beep;
	free(poped);

	return beep;
}

int q_samples_left(q_node* queue) {
	int samples_left = 0;
	q_node* aux = queue;
	while (aux != NULL) {
		samples_left += aux->beep->samples_left;
		aux = aux->next;
	}

	return samples_left;
}

int q_duration(q_node* queue) {
	int duration = 0;
	q_node* aux = queue;
	while (aux != NULL) {
		duration += aux->beep->duration;
		//duration += aux->beep->duration * 1000;
		aux = aux->next;
	}

	return duration;
}

void q_print(q_node *queue) {
	q_node *aux = queue;
	while (aux != NULL) {
		beep_print(aux->beep);
		aux = aux->next;
	}
}

/*void q_free(q_node *queue) {
	q_node *aux;
	while (queue != NULL) {
		aux = queue;
		queue = queue->next;
		free(aux->beep);
		free(aux);
	}
}*/

void q_free(q_node **queue) {
	q_node *aux;
	while (*queue != NULL) {
		aux = *queue;
		*queue = (*queue)->next;
		free(aux->beep);
		free(aux);
	}
}
/*
int main() {
	Beep *beep1 = beep_new(100, 440.0, 44100);
	Beep *beep2 = beep_new(200, 450.0, 44100);
	Beep *beep3 = beep_new(300, 460.0, 44100);

	q_node *queue = NULL;
	q_push(&queue, beep1);
	q_push(&queue, beep2);
	q_push(&queue, beep3);

	q_print(queue);
	printf("\n-----\n\n");

	Beep* poped = q_pop(&queue);

	q_print(queue);

	printf("\npoped: ");
	beep_print(poped);

	Beep* peeked = q_peek(queue);
	printf("\npeeked: ");
	beep_print(peeked);
	
	free(poped);
	q_free(queue);
}*/	

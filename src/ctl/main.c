/* libdogma
 * Copyright (C) 2016 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <dogma.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../core/tables.h"

/* XXX: copied from dogma_internal.h */
static const char* assoctypes[] = {
	"PreAssign",
	"PreMul",
	"PreDiv",
	"ModAdd",
	"ModSub",
	"PostMul",
	"PostDiv",
	"PostPercent",
	"PostAssign",
	"SkillPointCalc",
	NULL
};

/* XXX: copied from dogma_internal.h */
static const char* envtypes[] = {
	"Self",
	"Char",
	"Ship",
	"Target",
	"Area",
	"Other",
	NULL
};

void dump_tree(dogma_expressionid_t, unsigned int, const char*);
void print_operanddef(const dogma_expression_t*, const dogma_operand_t*, const char*, const char*);

void usage(char* me) {
	fprintf(stderr, "Usage:\n%s help\n", me);
	fprintf(stderr, "%s dump-effect <effect-id>\n", me);
	fprintf(stderr, "%s dump-expression-tree <expression-id>\n", me);
	fprintf(stderr, "%s dump-operand <operand-key>\n", me);
	exit(1);
}

int main(int argc, char** argv) {
	if(argc < 2) usage(argv[0]);
	
	dogma_init();

	if(!strcmp(argv[1], "dump-effect")) {
		if(argc != 3) usage(argv[0]);
		dogma_effectid_t id = strtol(argv[2], NULL, 10);
		const dogma_effect_t* e;
		if(dogma_get_effect(id, &e) == DOGMA_NOT_FOUND) {
			fprintf(stderr, "Effect %i not found\n", id);
			exit(1);
		}
		printf("Effect %i\n", id);
		printf("Category: %i\n", e->category);
		printf("PreExpression: %i\n", e->preexpressionid);
		printf("PostExpression: %i\n", e->postexpressionid);
		return 0;
	}

	if(!strcmp(argv[1], "dump-expression-tree")) {
		if(argc != 3) usage(argv[0]);
		dogma_expressionid_t id = strtol(argv[2], NULL, 10);
		dump_tree(id, 0, "");
		return 0;
	}

	if(!strcmp(argv[1], "dump-operand")) {
		if(argc != 3) usage(argv[0]);
		const dogma_operand_t* op;
		dogma_operandid_t id = 1;
		while(dogma_get_operand(id, &op) == DOGMA_OK) {
			if(strcmp(op->key, argv[2])) {
				++id;
				continue;
			}

			printf("Operand %i\n", op->id);
			printf("Key: %s\n", op->key);
			printf("Desc: %s\n", op->description);
			printf("Format: %s\n", op->format);

			return 0;
		}
		fprintf(stderr, "Operand not found\n");
		exit(1);
	}

	usage(argv[0]);
	return 1;
}

void dump_tree(dogma_expressionid_t id, unsigned int indent, const char* prefix) {
	const dogma_expression_t* exp;
	const dogma_operand_t* op;
	
	for(unsigned int i = 0; i < indent; ++i)
		fputs("|   ", stdout);

	if(dogma_get_expression(id, &exp) == DOGMA_NOT_FOUND) {
		fprintf(stderr, "Expression %i not found\n", id);
		exit(1);
	}

	dogma_get_operand(exp->operandid, &op);

	printf("%s%i, %s", prefix, id, op->key);
	print_operanddef(exp, op, "(", ")");
	fputs("\n", stdout);

	if(exp->arg1) dump_tree(exp->arg1, indent + 1, "arg1: ");
	if(exp->arg2) dump_tree(exp->arg2, indent + 1, "arg2: ");
}

void print_operanddef(const dogma_expression_t* exp, const dogma_operand_t* op, const char* prefix, const char* suffix) {	
	switch(op->id) {
	case DOGMA_DEFASSOCIATION:
		printf("%s%i:%s%s", prefix, exp->assoctype, assoctypes[exp->assoctype], suffix);
		break;
		
	case DOGMA_DEFENVIDX:
		printf("%s%i:%s%s", prefix, exp->envidx, envtypes[exp->envidx], suffix);
		break;
		
	case DOGMA_DEFTYPEID:
		printf("%s%i%s", prefix, exp->typeid, suffix);
		break;
		
	case DOGMA_DEFATTRIBUTE:
		printf("%s%i%s", prefix, exp->attributeid, suffix);
		break;
		
	case DOGMA_DEFGROUP:
		printf("%s%i%s", prefix, exp->groupid, suffix);
		break;
		
	case DOGMA_DEFBOOL:
		printf("%s%i%s", prefix, exp->boolv, suffix);
		break;
		
	case DOGMA_DEFINT:
		printf("%s%i%s", prefix, exp->intv, suffix);
		break;
		
	case DOGMA_DEFFLOAT:
		printf("%s%f%s", prefix, exp->floatv, suffix);
		break;

	case DOGMA_DEFSTRING:
		printf("%s%s%s", prefix, exp->value, suffix);
		break;

	default:
		return;
	}
}

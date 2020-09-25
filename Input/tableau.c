#include <stdio.h>
#include <string.h>   /* for all the new-fangled string functions */
#include <stdlib.h>     /* malloc, free, rand */


int Fsize=50; /*maximum formula length*/
int inputs =10;/* number of formulas expected in input.txt*/
int ThSize=100;/* maximum size of set of formulas, if needed*/
int TabSize=500; /*maximum length of tableau queue, if needed*/


/* A set will contain a list of words. Use NULL for emptyset.  */
  struct set{
    char *item;/*first word of non-empty set*/
    struct set *tail;/*remaining words in the set*/
  };

/* A tableau will contain a list of pointers to sets (of words).  Use NULL for empty list.*/
struct tableau {
  struct set * S; /* pointer to first set in non-empty list */
  struct tableau *rest; /*list of pointers to other sets*/
};

int main(){
    char *partone(char *g);
    char *parttwo(char *g);
    int parse (char *g);
   struct tableau* complete(struct tableau *t);
    int closed(struct tableau *t) ;

    char *name = malloc(Fsize);
    struct set   *S = (struct set *)malloc(sizeof(struct set));
    struct tableau *t = (struct tableau *)malloc(sizeof(struct tableau));
/*You should not need to alter the input and output parts of the program below.*/


 /*input 10 strings from "input.txt" */
    FILE *fp, *fpout;

  /* reads from input.txt, writes to output.txt*/
    if ((  fp=fopen("input.txt","r"))==NULL){printf("Error opening file");exit(1);}
    if ((  fpout=fopen("output.txt","w"))==NULL){printf("Error opening file");exit(1);}

    int j;

    for(j=0;j<inputs;j++)
    {
        fscanf(fp, "%s",name);/*read formula*/
        switch (parse(name))
        {
            case(0): fprintf(fpout, "%s is not a formula.  \n", name);break;
            case(1): fprintf(fpout, "%s is a proposition. \n ", name);break;
            case(2): fprintf(fpout, "%s is a negation.  \n", name);break;
            case(3):fprintf(fpout, "%s is a binary. The first part is %s and the second part is %s  \n", name, partone(name), parttwo(name));break;
            default:fprintf(fpout, " Unexpected ");
        }



        if (parse(name)!=0){

          *S = (struct set) {name, NULL};/*initialise S with single formula, called name*/
          *t = (struct tableau) {S, NULL};/* initialise tableau with one set, S*/
           struct tableau *newT = complete(t);
            if (closed(newT))  fprintf(fpout, "%s is not satisfiable.\n", name);
            else fprintf(fpout, "%s is satisfiable.\n", name);
        }
        else  fprintf(fpout, "I told you, %s is not a formula.\n", name);
    }


    fclose(fp);
    fclose(fpout);
    free(name);
    free(S);
    free(t);

  return(0);
}


char *getSubstring(char* base, int start, int end){
    char *result = (char*)calloc(sizeof(char), (end - start) + 1);
    int i = 0;

    while(start < end){
        *(result + i) = base[start];
        start++; i++;
    }
    return result;
}

int getHighLevelConnective(char *g){
    int count = 0; int i = 0;
    while(*(g + i)){
        if((*(g+i) == 'v' || *(g+i) == '^' || *(g+i) == '>') & count == 1){
            return i;
        }
        else if(*(g+i) == '('){
            count++;
        }
        else if(*(g+i) == ')'){
            count--;
        }
        i++;
    }
    return -1;

}

char *partone(char *g){
  int n = getHighLevelConnective(g);
  char *firstPart = getSubstring(g,1,n);
  return firstPart;

}

char *parttwo(char *g){
    int n = getHighLevelConnective(g);
    char *secondPart = getSubstring(g,n+1,strlen(g) - 1);
    return secondPart;
}


int parse (char *g){
  if(strlen(g) == 1 && (*g == 'p' || *g == 'q' || *g == 'r')){
    return 1;
  }

  else if (*g == '-' && strlen(g) > 1){
    char *remainingString = getSubstring(g,1,strlen(g));
    if(parse(remainingString) != 0){
      free(remainingString);
      return 2;
    }
  }

  else if (*g == '('){
    int n = getHighLevelConnective(g);
    if(n != -1){
      char* leftside = partone(g); char* rightside = parttwo(g);
      if((parse(leftside) != 0) && (parse(rightside) != 0)){
        free(leftside); free(rightside);
        return 3;
      }
  }
}

return 0;
}
//------------------------------------------------------------------------------------
struct set* removeAllOccurrences(char* nonLiteral, struct set* theoryPointer){
  struct set* previous = NULL; struct set* start = theoryPointer;
  while(theoryPointer){
    if(strcmp(nonLiteral, theoryPointer->item) == 0){
      if(previous == NULL){
        start = theoryPointer ->tail;
      }
      else{
        previous->tail = theoryPointer -> tail;
      }
      theoryPointer = theoryPointer ->tail;
    }
    else{
      previous = theoryPointer;
      theoryPointer = theoryPointer->tail;
    }
  }
  return start;
}

struct set* addChildren(struct set* children, struct set* theoryPointer){
  struct set *previous = NULL;struct set *start = theoryPointer;
   if(theoryPointer == NULL){
        return children;
    }
    while(theoryPointer){
        previous = theoryPointer;
        theoryPointer = theoryPointer ->tail;
    }
    previous->tail = children;
    return start;
}

_Bool isLiteral(char *formula){
  return (strlen(formula) == 1 || strlen(formula) == 2);
}

char* pickNonLiteral(struct set* theoryPointer){
  while(theoryPointer){
    if(!isLiteral(theoryPointer->item)){
      return theoryPointer->item;
    }
    theoryPointer = theoryPointer->tail;
  }

}
struct tableau* dequeue(struct tableau *t){
  struct tableau *tab = t->rest;
  return tab;
}

struct tableau* enqueue(struct tableau *t, struct set *newSet){
    struct tableau *previous = NULL;struct tableau *start = t;
    struct tableau *myTab = (struct tableau *)malloc(sizeof(struct tableau));
    myTab -> S = newSet;
    myTab -> rest = NULL;
    if(t == NULL){
        return myTab;
    }
    while(t){
        previous = t;
        t = t ->rest;
    }
    previous->rest = myTab;
    return start;

}

void populateArray(int* myArray, char *formula, int propIndex){
  if(formula[propIndex] == 'p' && myArray[0] == 0){
    myArray[0]++;
  }
  else if(formula[propIndex] == 'q' && myArray[1] == 0){
    myArray[1]++;
  }
  else if(formula[propIndex] == 'r' && myArray[2] == 0){
    myArray[2]++;
  }
}

_Bool auxContra(int* prop, int* negatedProp){

    for(int i = 0; i < 3; i++){
        if(prop[i] + negatedProp[i] > 1){
            return 1;
        }
    }
    return 0;
}


_Bool contradictory(struct set *theoryPointer){
  int prop[] = {0,0,0};
  int negatedProp[] = {0,0,0};
  while(theoryPointer){
    char *formula = theoryPointer ->item;
    if(strlen(formula) == 1){
      populateArray(prop,formula,0);
    }
    else if(strlen(formula) == 2){
      populateArray(negatedProp,formula,1);
    }
    theoryPointer = theoryPointer -> tail;
  }
  return auxContra(prop,negatedProp);
}



_Bool expanded(struct set *theoryPointer){
  while(theoryPointer){
    char *formula = theoryPointer ->item;
    if(!isLiteral(formula)){
      return 0;
    }
    theoryPointer = theoryPointer -> tail;
    }
    return 1;
}

_Bool isIn(char* element,struct set* set1){
    while(set1){
        if(strcmp(element,set1->item) == 0){
           return 1;
        }
        set1 = set1->tail;
    }
    return 0;
}

_Bool isSubset(struct set* set1, struct set* set2){
    while(set1){
        char* element = set1->item;
        if(!isIn(element,set2)){
            return 0;
        }
        set1 = set1->tail;
    }
    return 1;
}

_Bool identical(struct set* set1, struct set* set2){
    return (isSubset(set1,set2) && isSubset(set2,set1));
}




_Bool isMember(struct set* theoryPointer, struct tableau* tab){
    while(tab){
        if(identical(theoryPointer,tab->S)){
            return 1;
        }
        tab = tab->rest;
    }
    return 0;
}


struct set* duplicate(struct set* aSet){
    struct set   *S = NULL; struct set *previous = NULL;
    struct set   *start = NULL; int n = 1;
    while(aSet){
        S = (struct set *)malloc(sizeof(struct set));
        S->tail = NULL;
        if(n == 1){
            start = S;
            n++;
        }
        S->item = aSet ->item;
        if(previous != NULL){
        previous->tail = S;
        }
        previous = S;
        aSet = aSet->tail;
    }
    return start;
}


int getType(char* word){
    if(word[0] == '('){
        int n = getHighLevelConnective(word);
        if(word[n] == '^'){
            return 1;
        }
         else if(word[n] == 'v'){
            return 5;
        }
        else if(word[n] == '>'){
            return 7;
        }
    }
    else if(word[0] == '-'){
        if(word[1] == '('){
            char* newWord = getSubstring(word,1,strlen(word));
            int n = getHighLevelConnective(newWord);
            if(newWord[n] == 'v'){
                return 3;
            }
            else if(newWord[n] == '>'){
                return 4;
            }
              else if(newWord[n] == '^'){
                return 6;
            }
        }
        else if(word[1] == '-'){
            return 2;
        }
    }
    return 0;
}

char* negate(char* aFormula){
    char* negated = (char*)calloc(sizeof(char),strlen(aFormula) + 1);
   *negated = '-';
  return strcat(negated,aFormula);
}

struct set* getSubformulasAlpha(char* formula, int n){
  struct set *one = (struct set *)malloc(sizeof(struct set));
  if(n == 1){
    char* first = partone(formula);
    char* second = parttwo(formula);
    struct set *two = (struct set *)malloc(sizeof(struct set));
    *one = (struct set){first,two};
    *two = (struct set){second,NULL};
  }
  else if(n==2){
    char* first = getSubstring(formula,2,strlen(formula));
    *one = (struct set){first,NULL};
  }
  else if(n == 3 || n==4){
    char* subString = getSubstring(formula,1,strlen(formula));
    char* first;
    if(n == 3){first = negate(partone(subString));}
    else{first = partone(subString);}
    char* second = negate(parttwo(subString));
    struct set *two = (struct set *)malloc(sizeof(struct set));
    *one = (struct set){first,two};
    *two = (struct set){second,NULL};
  }


  return one;
}



struct set* getLeftformulB(char* formula, int n){
  struct set *one = (struct set *)malloc(sizeof(struct set));
  if(n==5){
    char* first = partone(formula);
    *one = (struct set){first,NULL};
  }
  else if(n==6){
    char* subString = getSubstring(formula,1,strlen(formula));
    char* first = negate(partone(subString));
    *one = (struct set){first,NULL};
  }
  else if(n==7){
    char* first = negate(partone(formula));
    *one = (struct set){first,NULL};
  }
  return one;
}

struct set* getRightformulB(char* formula, int n){
 struct set *one = (struct set *)malloc(sizeof(struct set));
  if(n==5){
    char* first = parttwo(formula);
    *one = (struct set){first,NULL};
  }
  else if(n == 6){
    char* subString = getSubstring(formula,1,strlen(formula));
    char* first = negate(parttwo(subString));
    *one = (struct set){first,NULL};
  }
  else if(n==7){
    char* first = parttwo(formula);
    *one = (struct set){first,NULL};
  }
  return one;
}

struct tableau* complete(struct tableau *t){
    while(t){
    struct set *theoryPointer = t->S;
    t = dequeue(t);
    if(expanded(theoryPointer) && !contradictory(theoryPointer)){
        t = enqueue(t,theoryPointer);
        return t;
    }
    char* nonLiteral = pickNonLiteral(theoryPointer);
    int n = getType(nonLiteral);
    if(n==1|| n==2 || n==3|| n==4){
         theoryPointer = removeAllOccurrences(nonLiteral,theoryPointer);
         struct set* children = getSubformulasAlpha(nonLiteral,n);
         theoryPointer = addChildren(children,theoryPointer);
         if(!contradictory(theoryPointer) && !isMember(theoryPointer,t)){
            t = enqueue(t,theoryPointer);

         }
    }
     else if(n == 5|| n == 6 || n == 7){
          for(int i = 0; i < 2;i++){
          struct set* fmula;
          struct set *tp1 = duplicate(theoryPointer);
          tp1 = removeAllOccurrences(nonLiteral,tp1);
          if(i == 0){
            fmula = getLeftformulB(nonLiteral,n);
          }
          else{
            fmula = getRightformulB(nonLiteral,n);
          }
          tp1 = addChildren(fmula,tp1);
          if(!contradictory(tp1) && !isMember(tp1,t)){
              t = enqueue(t,tp1);
          }
          }
      }
    }
       return t;

}

int closed(struct tableau *t) {
  if(t == NULL){
    return 1;
  }
  return 0;
}


